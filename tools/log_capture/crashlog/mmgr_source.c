/* * Copyright (C) Intel 2010
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file mmgr_source.c
 * @brief File containing functions to handle modem manager source and events
 * coming from this source.
 *
 * This file contains the functions to handle the modem manager source (init,
 * closure, events reading from socket) and the functions to process the
 * different kinds of events read from this source.
 */

#include "mmgr_source.h"
#include "crashutils.h"
#include "privconfig.h"
#include "fsutils.h"
#include "log.h"
#include "tcs_wrapper.h"
#include "mdm_cli.h"
#include "ingredients.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <cutils/properties.h>

/*Constants*/
#define MODEMVERSION_CHK_PERIOD_MS          500
#define MODEMVERSION_CHK_TIMEOUT_SECONDS    180
#define NB_DATA 6
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

/* private structure */
struct mmgr_data {
    char string[MMGRMAXSTRING];     /* main string representing mmgr data content */
    int  extra_int;                 /* optional integer data (mailly used for error code) */
    int  extra_nb_causes;                 /* optional integer data that could be used to store number of optionnal data */
    char extra_tab_string[6][MMGRMAXEXTRA];/* optional string that could be used to store data0 to data5 */
};

struct mmgr_ct_context {
    unsigned char instance;
    mdm_cli_hdle_t *mmgr_hdl;
    int mmgr_monitor_fd[2];
};

static struct mmgr_ct_context mmgr_ctx[MAX_MMGR_INST];

static bool is_mmgr_fake_event() {
    char prop_mmgr[PROPERTY_VALUE_MAX];

    if(property_get(PROP_REPORT_FAKE, prop_mmgr, NULL)){
        if (!strcmp(prop_mmgr, "modem")) {
            return 1;
        }
    }
    return 0;
}

static int write_mmgr_monitor_with_dbg_info(const char *chain, int fd,
        const mdm_cli_dbg_info_t *dbg_info) {
    struct mmgr_data cur_data;
    int first_data = 0;

    if (!dbg_info->data)
        return -1;

    // cur_data.extra_int is the merge of the type and the logging rules
    cur_data.extra_int = (dbg_info->type & 0xFF) + (dbg_info->log_attached << 8);

    if (chain) {
        //APIMR and MPANIC case
        snprintf(cur_data.string, sizeof(cur_data.string), chain);
    } else {
        snprintf(cur_data.string, sizeof(cur_data.string), dbg_info->data[0]);
        first_data = 1;
    }

    cur_data.extra_nb_causes = dbg_info->nb_data - first_data;
    if (dbg_info->nb_data > 1) {
        int i;
        for (i=0; i<cur_data.extra_nb_causes; i++) {
            if (cur_data.extra_tab_string[i]) {
                // MMGR guarantees that all strings are NULL terminated: No need to check the length
                snprintf(cur_data.extra_tab_string[i], sizeof(cur_data.extra_tab_string[i]),
                        dbg_info->data[first_data + i]);
            } else {
                cur_data.extra_tab_string[i][0] = '\0';
            }
        }

    } else {
        cur_data.extra_nb_causes = 0;
    }

    write(fd, &cur_data, sizeof(struct mmgr_data));
    return 0;
}

static void write_mmgr_monitor_with_extras(char *chain,
                                           int extra_int,
                                           struct mmgr_ct_context *ctx) {
    struct mmgr_data cur_data;
    strncpy(cur_data.string, chain, sizeof(cur_data.string));
    cur_data.extra_int = extra_int;
    write(ctx->mmgr_monitor_fd[1], &cur_data, sizeof(struct mmgr_data));
}

void *monitor_modem_version(void *arguments) {
    unsigned char *args = (unsigned char *)arguments;
    unsigned int retry_counter;
    char *name, modem_name[PROPERTY_VALUE_MAX];
    int status;
    static unsigned int running[MAX_MMGR_INST];
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    // decreased by one, because telephony counts from 1 the instance.
    unsigned char instance = *args - 1;
    free(args);

    /*if no description is available for the requested modem */
    if (instance >= MAX_MMGR_INST) {
        LOGW("No specific modem prop/path for instance %d, abort version detection", instance);
        return NULL;
    }

    pthread_mutex_lock(&lock);
    if (running[instance]) {
        // reload the counter
        running[instance] = 1;
        pthread_mutex_unlock(&lock);

        LOGD("monitor_modem_version[%d] - thread already started. Counter reloaded!",
             instance, running[instance]);
        return NULL;
    }
    running[instance] = 1;
    pthread_mutex_unlock(&lock);

    status = fetch_modem_name(instance, &name);
    pthread_mutex_lock(&lock);
    if (status > 0) {
        running[instance] = 0;
        pthread_mutex_unlock(&lock);
        LOGD("monitor_modem_version[%d] modem version set to %s", instance, name);
        return NULL;
    }
    pthread_mutex_unlock(&lock);

    snprintf(modem_name, sizeof(modem_name), "%s", name);
    retry_counter = MODEMVERSION_CHK_TIMEOUT_SECONDS * 1000 / MODEMVERSION_CHK_PERIOD_MS;

    pthread_mutex_lock(&lock);
    while (running[instance] != 0 && running[instance]++ <= retry_counter) {
        pthread_mutex_unlock(&lock);
        status = fetch_modem_name(instance, &name);

        if (strcmp(name, modem_name)) {
            LOGD("monitor_modem_version[%d] modem version set to %s", instance, name);
            break;
        }
        else if (status == -EINVAL) {
            LOGW("No specific modem prop/path for instance %d, abort version detection", instance);
            break;
        }

        /* Wait */
        usleep(MODEMVERSION_CHK_PERIOD_MS * 1000);
        pthread_mutex_lock(&lock);
    }

    running[instance] = 0;
    pthread_mutex_unlock(&lock);
    if (status < 0)
        LOGE("monitor_modem_version[%d] modem version could not be set.", instance);

    return NULL;
}

static int launch_modem_version_check(unsigned char instance) {
    pthread_t thread;
    int ret;
    unsigned char *inst =  malloc(sizeof(unsigned char));
    if (!inst) {
        LOGE("Cannot allocate thread data");
        return -1;
    }
    *inst = instance;
    ret = pthread_create(&thread, NULL, monitor_modem_version, (void *)inst);
    if (ret < 0) {
        LOGE("pthread_create error");
        return -1;
    }
    return 0;

}

int mdm_UP(const mdm_cli_callback_data_t * ev) {
    LOGD("Received MDM_UP[%d]", ((struct mmgr_ct_context *)ev->context)->instance);
    return launch_modem_version_check(((struct mmgr_ct_context *)ev->context)->instance);
}

int mdm_SHUTDOWN(const mdm_cli_callback_data_t * ev) {
    LOGD("Received MDM_SHUTDOWN[%d]", ((struct mmgr_ct_context *)ev->context)->instance);
    write_mmgr_monitor_with_extras("MMODEMOFF", 0,
                                   (struct mmgr_ct_context *)ev->context);
    return 0;
}

int mdm_REBOOT(const mdm_cli_callback_data_t * ev) {
    LOGD("Received DBG_TYPE_PLATFORM_REBOOT[%d]", ((struct mmgr_ct_context *)ev->context)->instance);
    write_mmgr_monitor_with_extras("MSHUTDOWN", 0,
                                   (struct mmgr_ct_context *)ev->context);
    return 0;
}


int mdm_OOS(const mdm_cli_callback_data_t * ev) {
    LOGD("Received MDM_OOS[%d]", ((struct mmgr_ct_context *)ev->context)->instance);
    write_mmgr_monitor_with_extras("MOUTOFSERVICE", 0,
                                   (struct mmgr_ct_context *)ev->context);
    return 0;
}

int mdm_MRESET(const mdm_cli_callback_data_t * ev) {
    LOGD("Received DBG_TYPE_SELF_RESET[%d]", ((struct mmgr_ct_context *)ev->context)->instance);
    write_mmgr_monitor_with_extras("MRESET", 0,
                                   (struct mmgr_ct_context *)ev->context);
    return 0;
}

int mdm_DUMP_END(const mdm_cli_callback_data_t * ev) {
    LOGD("Received DBG_TYPE_DUMP_END[%d]", ((struct mmgr_ct_context *)ev->context)->instance);

    mdm_cli_dbg_info_t *dbg_info = (mdm_cli_dbg_info_t *)ev->data;
    if (!dbg_info || !dbg_info->data) {
        LOGE("%s: empty data", __FUNCTION__);
    } else {
        struct mmgr_ct_context *ctx = (struct mmgr_ct_context *)ev->context;
        write_mmgr_monitor_with_dbg_info("MPANIC", ctx->mmgr_monitor_fd[1], dbg_info);
    }

    return 0;
}

int mdm_APIMR(const mdm_cli_callback_data_t * ev) {
    LOGD("Received DBG_TYPE_APIMR[%d]", ((struct mmgr_ct_context *)ev->context)->instance);

    mdm_cli_dbg_info_t *dbg_info = (mdm_cli_dbg_info_t *)ev->data;
    if ((!dbg_info || !dbg_info->data)) {
        LOGE("%s: empty data", __FUNCTION__);
    } else {
        struct mmgr_ct_context *ctx = (struct mmgr_ct_context *)ev->context;
        LOGD("%s: AP reset asked by: %s", __FUNCTION__, dbg_info->data[0]);
        write_mmgr_monitor_with_dbg_info("APIMR", ctx->mmgr_monitor_fd[1], dbg_info);
    }

    return 0;
}

int mdm_DUMP_START(const mdm_cli_callback_data_t * ev) {
    LOGD("Received DBG_TYPE_DUMP_START[%d]", ((struct mmgr_ct_context *)ev->context)->instance);
    struct mmgr_data cur_data;
    struct mmgr_ct_context *ctx = (struct mmgr_ct_context *)ev->context;
    strncpy(cur_data.string, "START_CD\0", sizeof(cur_data.string));
    //TO DO update with write_mmgr_monitor_with_extras

    write(ctx->mmgr_monitor_fd[1], &cur_data, sizeof(struct mmgr_data));
    return 0;
}

int mdm_DUMP_SKIP(const mdm_cli_callback_data_t * ev) {
    LOGD("Received DBG_TYPE_DUMP_SKIP[%d]", ((struct mmgr_ct_context *)ev->context)->instance);

    mdm_cli_dbg_info_t *dbg_info = (mdm_cli_dbg_info_t *)ev->data;
    if ((!dbg_info || !dbg_info->data)) {
        LOGE("%s: empty data", __FUNCTION__);
    } else {
        struct mmgr_ct_context *ctx = (struct mmgr_ct_context *)ev->context;
        LOGD("%s: Skip Core Dump for external interface: %s", __FUNCTION__, dbg_info->data[0]);
        write_mmgr_monitor_with_dbg_info("MSKIPCOREDUMP", ctx->mmgr_monitor_fd[1], dbg_info);
    }

    return 0;
}

int mdm_DBG_INFO(const mdm_cli_callback_data_t * ev) {
    LOGD("Received MDM_DBG_INFO[%d]", ((struct mmgr_ct_context *)ev->context)->instance);
    mdm_cli_dbg_info_t *dbg_info = (mdm_cli_dbg_info_t *) ev->data;

    if (dbg_info == NULL) {
        LOGE("%s: empty data", __FUNCTION__);
        return -1;
    }

    switch (dbg_info->type) {
        case DBG_TYPE_DUMP_START:
            mdm_DUMP_START(ev);
            break;
        case DBG_TYPE_DUMP_END:
            mdm_DUMP_END(ev);
            break;
        case DBG_TYPE_PLATFORM_REBOOT:
            mdm_REBOOT(ev);
            break;
        case DBG_TYPE_APIMR:
            mdm_APIMR(ev);
            break;
        case DBG_TYPE_SELF_RESET:
            mdm_MRESET(ev);
            break;
        case DBG_TYPE_DUMP_SKIP:
            mdm_DUMP_SKIP(ev);
            break;
        case DBG_TYPE_STATS:
        case DBG_TYPE_ERROR:
        case DBG_TYPE_INFO: {
            struct mmgr_ct_context *ctx = (struct mmgr_ct_context *)ev->context;
            write_mmgr_monitor_with_dbg_info(NULL, ctx->mmgr_monitor_fd[1], dbg_info);
            }
            break;
        default:
            /* Not handled */
            break;
    }

    return 0;
}

int mmgr_get_fd(unsigned int mdm_inst) {
    return mmgr_ctx[mdm_inst].mmgr_monitor_fd[0];
}

void init_mmgr_cli_source(unsigned int mdm_inst) {
    int ret = 0;
    char clientname[25];

    mdm_cli_register_t evts[] = {
        { MDM_OOS, mdm_OOS, (void *)&mmgr_ctx[mdm_inst]},
        { MDM_SHUTDOWN, mdm_SHUTDOWN, (void *)&mmgr_ctx[mdm_inst]},
        { MDM_DBG_INFO, mdm_DBG_INFO, (void *)&mmgr_ctx[mdm_inst]},
        { MDM_UP, mdm_UP, (void *)&mmgr_ctx[mdm_inst]},
    };

    if (!CRASHLOG_MODE_MMGR_ENABLED(g_crashlog_mode)) {
        LOGI("%s: MMGR source state is disabled", __FUNCTION__);
        mmgr_ctx[mdm_inst].mmgr_monitor_fd[0] = 0;
        mmgr_ctx[mdm_inst].mmgr_monitor_fd[1] = 0;
        return;
    }

    LOGD("%s : initializing MMGR[%d] source...", __FUNCTION__, mdm_inst);
    if (mmgr_ctx[mdm_inst].mmgr_hdl) {
        close_mmgr_cli_source(mdm_inst);
    }
    sprintf(clientname, "crashlogd_%d", mdm_inst);

    // telephony chooses to count from 1 rather than 0,
    // while our structures are starting from 0.
    mmgr_ctx[mdm_inst].instance = mdm_inst + 1;

    uint32_t iMaxTryConnect =
        MAX_WAIT_MMGR_CONNECT_SECONDS * 1000 / MMGR_CONNECT_RETRY_TIME_MS;

    while (iMaxTryConnect-- != 0) {

        /* Try to connect */
        mmgr_ctx[mdm_inst].mmgr_hdl = mdm_cli_connect(clientname,mmgr_ctx[mdm_inst].instance,
                ARRAY_SIZE(evts), evts);

        if (mmgr_ctx[mdm_inst].mmgr_hdl) {

            break;
        }

        LOGE("%s: Delaying connection to MMGR[%d] %d", __FUNCTION__, mdm_inst, ret);

        /* Wait */
        usleep(MMGR_CONNECT_RETRY_TIME_MS * 1000);
    }
    // pipe init
    if (pipe(mmgr_ctx[mdm_inst].mmgr_monitor_fd) == -1) {
        LOGE("%s: MMGR[%d] source init failed : Can't create the pipe - error is %s\n", __FUNCTION__, mdm_inst, strerror(errno));
        mmgr_ctx[mdm_inst].mmgr_monitor_fd[0] = 0;
        mmgr_ctx[mdm_inst].mmgr_monitor_fd[1] = 0;
    }
}

void close_mmgr_cli_source(unsigned int mdm_inst) {
    if (!CRASHLOG_MODE_MMGR_ENABLED(g_crashlog_mode))
        return;

    mdm_cli_disconnect(mmgr_ctx[mdm_inst].mmgr_hdl);
    mmgr_ctx[mdm_inst].mmgr_hdl = NULL;
}

/**
 * @brief Compute mmgr parameter
 *
 * Called when parameter are needed for a mmgr callback
 *
 * @param parameters needed to create event (logd, name,...)
 *
 * @return 0 on success, -1 on error.
 */
static int compute_mmgr_param(char *type, e_dir_mode_t *mode, char *name, int *aplog, int *bplog, int *log_mode) {

    if (strstr(type, "MODEMOFF" )) {
        //CASE MODEMOFF
        *mode = MODE_STATS;
        sprintf(name, "%s", INFOEVENT);
    } else if (strstr(type, "MSHUTDOWN" )) {
        //CASE MSHUTDOWN
        *mode = MODE_CRASH;
        sprintf(name, "%s", CRASHEVENT);
        *aplog = 1;
        *log_mode = APLOG_TYPE;
    } else if (strstr(type, "MOUTOFSERVICE" )) {
        //CASE MOUTOFSERVICE
        *mode = MODE_CRASH;
        sprintf(name, "%s", CRASHEVENT);
        *aplog = 1;
        *log_mode = APLOG_TYPE;
    } else if (strstr(type, "MPANIC" )) {
        //CASE MPANIC
        *mode = MODE_CRASH;
        sprintf(name, "%s",CRASHEVENT);
        *aplog = 1;
        *log_mode = APLOG_TYPE;
        *bplog = check_running_modem_trace();
    } else if (strstr(type, "APIMR" )) {
        //CASE APIMR
        *mode = MODE_CRASH;
        sprintf(name, "%s", CRASHEVENT);
        *aplog = 1;
        *log_mode = APLOG_TYPE;
        *bplog = check_running_modem_trace();
    } else if (strstr(type, "MRESET" )) {
        //CASE MRESET
        *mode = MODE_CRASH;
        sprintf(name, "%s",CRASHEVENT);
        *log_mode = APLOG_TYPE;
        *aplog = 1;
    } else if (strstr(type, "MSKIPCOREDUMP" ) ) {
        //CASE MSKIPCOREDUMP
        *mode = MODE_STATS;
        sprintf(name, "%s", INFOEVENT);
    } else  if (!strstr(type, "START_CD" )){
        //unknown event name
        LOGE("%s: wrong type found in mmgr_get_fd : %s.\n", __FUNCTION__, type);
        return -1;
    }
    return 0;
}

static void get_modem_version(unsigned int instance, char *version, unsigned int length) {
    char *name;

    if (!length)
      return;

    if (fetch_modem_name(instance, &name) < 0)
        LOGE("%s: Could not retrieve modem[%d] name", __FUNCTION__, instance);

    snprintf(version, length, "%s", name);
}

/**
 * @brief Handle mmgr call back
 *
 * Called when a call back function is triggered
 * depending on init subscription
 *
 * @param files nb max of logs destination directories (crashlog,
 * aplogs, bz... )
 *
 * @return 0 on success, -1 on error.
 */
int mmgr_handle(unsigned int mdm_inst) {
    e_dir_mode_t event_mode = MODE_CRASH;
    int aplog_mode;
    char *dir;
    int bplog_mode = BPLOG_TYPE;
    char *event_dir, *key;
    char event_name[MMGRMAXSTRING];
    char data[NB_DATA][MMGRMAXEXTRA];
    char modem_version[MMGRMAXEXTRA];
    char cd_path[MMGRMAXEXTRA];
    char cd_info_path[MMGRMAXEXTRA];
    char destion[PATHMAX];
    char type[20];
    int nbytes, copy_aplog = 0, copy_bplog = 0;
    struct mmgr_data cur_data;
    const char *dateshort = get_current_time_short(1);
    int i;

    // Initialize stack strings to empty strings
    event_name[0] = 0;
    for (i =0; i <NB_DATA; i++)
        data[i][0] = 0;
    modem_version[0] = 0;
    cd_path[0] = 0;
    destion[0] = 0;
    type[0] = 0;

    // get data from mmgr pipe
    nbytes = read(mmgr_get_fd(mdm_inst), &cur_data, sizeof(struct mmgr_data));
    strcpy(type, cur_data.string);

    if (nbytes == 0) {
        LOGW("No data found in mmgr_get_fd[%d].\n", mdm_inst);
        return 0;
    }
    if (nbytes < 0) {
        nbytes = -errno;
        LOGE("%s: Error while reading mmgr_get_fd[%d] - %s.\n", __FUNCTION__, mdm_inst, strerror(errno));
        return nbytes;
    }
    //find_dir should be done before event_dir is set
    LOGD("Received string from mmgr[%d]: %s  - %d bytes", mdm_inst , type,nbytes);
    // For "TFT" event, parameters are given by the data themselves
    if (strstr(type, "TFT")) {
        switch (cur_data.extra_int & 0xFF) {
             case DBG_TYPE_INFO:
                 sprintf(event_name, "%s", INFOEVENT);
                 break;
             case DBG_TYPE_ERROR:
                 sprintf(event_name, "%s", ERROREVENT);
                 break;
             case DBG_TYPE_STATS:
                 sprintf(event_name, "%s", STATSEVENT);
                 break;
             default:
                 break;
        }
        event_mode = MODE_STATS;
        aplog_mode = APLOG_STATS_TYPE;
        bplog_mode = BPLOG_STATS_TYPE;
        copy_aplog = (cur_data.extra_int >> 8) & DBG_ATTACH_AP_LOG;
        copy_bplog = ((cur_data.extra_int >> 8) & DBG_ATTACH_BP_LOG)
                && check_running_modem_trace();
    } else {
       if (compute_mmgr_param(type, &event_mode, event_name, &copy_aplog, &copy_bplog,
               &aplog_mode) < 0) {
           return -1;
       }
    }
    if (strstr(type, "MPANIC" )) {
        //set DATA0/1 value
        LOGD("Extra nb causes: %d", cur_data.extra_nb_causes);
        if (3 == cur_data.extra_nb_causes) {
            snprintf(data[0], sizeof(data[0]), cur_data.extra_tab_string[0]);
            if (!strcmp(cur_data.extra_tab_string[0], DUMP_STR_SUCCEED))
                snprintf(cd_path, sizeof(cd_path),"%s", cur_data.extra_tab_string[1]);
            else
                snprintf(data[1], sizeof(data[1]), cur_data.extra_tab_string[1]);
            // if available, this field will point to the path of the CD_INFO log file.
            snprintf(cd_info_path, sizeof(cd_info_path),"%s", cur_data.extra_tab_string[2]);
            snprintf(destion, sizeof(destion), "%s-%d", MCD_PROCESSING, mdm_inst);
            if (file_exists(destion))
                remove(destion);
        } else
            LOGE("MPANIC: wrong data");
    } else if (strstr(type, "APIMR" ) || strstr(type, "TFT") || strstr(type, "MSKIPCOREDUMP")){
        if(cur_data.extra_nb_causes > 0) {
            for (i = 0; i<cur_data.extra_nb_causes; i++) {
                snprintf(data[i], sizeof(data[i]), cur_data.extra_tab_string[i]);
                LOGD("Extra tab string value %d %s", i, data[i]);
            }
        }
    } else if (strstr(type, "START_CD" )){
        snprintf(destion, sizeof(destion), "%s-%d", MCD_PROCESSING, mdm_inst);
        if (write_binary_file(destion, (const unsigned char*)&mdm_inst, sizeof(mdm_inst))) {
            LOGE("can not create file: %s\n", destion);
        }
        return 0;
    }
    get_modem_version(mdm_inst, modem_version, MMGRMAXEXTRA);

    key = generate_event_id(CRASHEVENT, event_name);
    dir = generate_crashlog_dir(event_mode, key);
    if (!dir) {
        LOGE("%s: Cannot get a valid new crash directory...\n", __FUNCTION__);
        raise_event(key, CRASHEVENT, event_name, NULL, NULL);
        LOGE("%-8s%-22s%-20s%s\n", CRASHEVENT, key, get_current_time_long(0), event_name);
        free(key);
        return -1;
    }

    // update event_dir should be done after find_dir call
    event_dir = (event_mode == MODE_STATS ? STATS_DIR : CRASH_DIR);

    if (copy_aplog > 0) {
        do_log_copy(type, dir, dateshort, aplog_mode);
    }
    if (copy_bplog > 0) {
        do_bplog_copy(type, dir, dateshort, bplog_mode, mdm_inst);
    }
    // copying files (if required)
    do_mv_in_dir(cd_path, dir);
    do_mv_in_dir(cd_info_path, dir);
    // generating extra DATA (if required)
    if ((strlen(data[0]) > 0) || (strlen(data[1]) > 0) || (strlen(data[2]) > 0) || (strlen(data[3]) > 0) || (strlen(modem_version) > 0)){
        FILE *fp;
        char fullpath[PATHMAX];
        if (strstr(event_name , ERROREVENT)) {
            snprintf(fullpath, sizeof(fullpath)-1, "%s/%s_errorevent", dir,type );
        }else if (strstr(event_name , INFOEVENT)) {
            snprintf(fullpath, sizeof(fullpath)-1, "%s/%s_infoevent", dir,type );
        }else if (strstr(event_name , STATSEVENT)) {
            snprintf(fullpath, sizeof(fullpath)-1, "%s/%s_trigger", dir,type );
        }else{
            snprintf(fullpath, sizeof(fullpath)-1, "%s/%s_crashdata", dir,type );
        }

        fp = fopen(fullpath,"w");
        if (fp == NULL) {
            LOGE("%s: Cannot create file %s - %s\n", __FUNCTION__, fullpath, strerror(errno));
        } else {
            //Fill DATA fields
            for (i=0; i<NB_DATA; i++) {
                if (strlen(data[i]) > 0)
                    fprintf(fp,"DATA%d=%s\n", i, data[i]);
            }
            if (strlen(modem_version) > 0)
                fprintf(fp,"MODEMVERSIONUSED=%s\n", modem_version);
            fprintf(fp,"_END\n");
            fclose(fp);
            do_chown(fullpath, PERM_USER, PERM_GROUP);
        }
    }
    //last step : need to check if this event should be reported as FAKE
    if (is_mmgr_fake_event()){
        //add _FAKE suffix
        strcat(type, FAKE_EVENT_SUFFIX);
    }
    raise_event(key, event_name, type, NULL, dir);
    LOGE("%-8s%-22s%-20s%s %s\n", event_name, key, get_current_time_long(0), type, dir);
    free(key);
    free(dir);
    return 0;
}
