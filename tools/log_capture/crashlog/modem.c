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
 * @file modem.c
 * @brief File containing functions to handle the processing of modem events.
 *
 * This file contains the functions to handle the processing of modem events and
 * modem shutdown events.
 */

#include "inotify_handler.h"
#include "crashutils.h"
#include "fsutils.h"
#include "privconfig.h"
#include "config_handler.h"
#include "tcs_wrapper.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

extern pconfig g_first_modem_config;

static int copy_modemcoredump(char *spath, char *dpath) {
    char src[PATHMAX];
    char des[PATHMAX];
    struct stat st;
    DIR *d;
    struct dirent *de;

    if (stat(spath, &st))
        return -errno;
    if (stat(dpath, &st))
        return -errno;

    src[0] = 0;
    des[0] = 0;

    d = opendir(spath);
    if (d == 0) {
        LOGE("%s: opendir failed - %s\n", __FUNCTION__, strerror(errno));
        return -errno;
    }
    while ((de = readdir(d)) != 0) {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;
        if (de->d_name[0] == 'c' && de->d_name[1] == 'd'
                && strstr(de->d_name, ".tar.gz")) {
            /* file form is cd_xxx.tar.gz */
            snprintf(src, sizeof(src), "%s/%s", spath, de->d_name);
            snprintf(des, sizeof(des), "%s/%s", dpath, de->d_name);
            do_copy_tail(src, des, 0);
            remove(src);
        }
    }
    if (closedir(d) < 0){
        LOGE("%s: closedir failed - %s\n", __FUNCTION__, strerror(errno));
        return -errno;
    }
    return 0;

}

int crashlog_check_mpanic_abort(){
    char destion[PATHMAX];
    char *dir;
    char *key;
    const char *dateshort = get_current_time_short(1);
    char bplogs_copied = 0;
    int mdm_inst = get_modem_count();
    mdm_inst = (mdm_inst<0) ? 0 : mdm_inst;

    while (--mdm_inst >= 0) {
        snprintf(destion, sizeof(destion), "%s-%d", MCD_PROCESSING, mdm_inst);
        if (file_exists(destion))
            break;
    }

    if (mdm_inst >= 0) {
        key = generate_event_id(CRASHEVENT, MDMCRASH_EVNAME);
        dir = generate_crashlog_dir(MODE_CRASH, key);
        if (!dir) {
            LOGE("%s: generate_crashlog_dir failed\n", __FUNCTION__);
            raise_event(key, CRASHEVENT, MDMCRASH_EVNAME, NULL, NULL);
            LOGE("%-8s%-22s%-20s%s\n", CRASHEVENT, key, get_current_time_long(0), MDMCRASH_EVNAME);
            free(key);
            return -1;
        }

        do_log_copy(MDMCRASH_EVNAME,dir,dateshort,APLOG_TYPE);
        do {
            snprintf(destion, sizeof(destion), "%s-%d", MCD_PROCESSING, mdm_inst);
            if (file_exists(destion)) {
                if ((cfg_collection_mode_modem() != COLLECT_BPLOG_CRASHING_ALL) ||
                    !bplogs_copied)
                    do_bplog_copy(MDMCRASH_EVNAME,dir,dateshort,BPLOG_TYPE_OLD, mdm_inst);
                remove(destion);
                bplogs_copied = 1;
            }
        } while (--mdm_inst >= 0);

        FILE *fp;
        char fullpath[PATHMAX];
        snprintf(fullpath, sizeof(fullpath)-1, "%s/%s_crashdata", dir, MDMCRASH_EVNAME);

        fp = fopen(fullpath,"w");
        if (fp == NULL){
            LOGE("%s: can not create file: %s\n", __FUNCTION__, fullpath);
        }else{
            fprintf(fp,"DATA0=%s\n", MPANIC_ABORT);
            fprintf(fp,"_END\n");
            fclose(fp);
            do_chown(fullpath, PERM_USER, PERM_GROUP);
        }

        raise_event(key, CRASHEVENT, MDMCRASH_EVNAME, NULL, dir);
        LOGE("%-8s%-22s%-20s%s %s\n", CRASHEVENT, key,
                get_current_time_long(0), MDMCRASH_EVNAME, dir);
        free(key);
        free(dir);
    }
    return 0;
}
