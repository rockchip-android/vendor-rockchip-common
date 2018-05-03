/* Copyright (C) Intel 2015
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
 * @file vmmtrap.c
 * @brief File containing functions used to handle vmm crashes.
 *
 */
#include "trigger.h"
#include "crashutils.h"
#include "privconfig.h"
#include "fsutils.h"
#include "config_handler.h"

#include <fcntl.h>
#include <stdlib.h>

#define COREDUMP_DIR            "Coredump"
#define VMM_DUMP_EXTENSION      ".istp"
#define VMM_DUMP_INFO_EXTENSION ".txt"
#define VMM_DUMP_DONE_EXTENSION ".inf"
#define VMM_DUMP_IN_PROGRESS_EXTENSION ".tmp"
#define VMM_DUMP_PATTERN        "coredump_"
#define VMM_DUMP_DONE_LEGACY ".info"
#define ISTP_LIMIT  (200*MB)
#define VDUMP_TRAP_INFO_PATH    "/dev/vdump"
#define GET_INFO_CMD      "get_coredump_prev_boot"
#define SUCCESS_FLAG      "Owner:"

static bool sdcard_write_failure = 0;
static bool crash_generated;
static char sd_path[PATHMAX];
static char coredump_dir_path[PATHMAX];

static int excec_get_command(){
    int status;
    FILE *fd;

    if (!file_exists(VDUMP_TRAP_INFO_PATH)) {
        LOGW("%s: file %s does not exist\n", __FUNCTION__, VDUMP_TRAP_INFO_PATH);
        return -ENOENT;
    }

    fd = fopen(VDUMP_TRAP_INFO_PATH, "r+");
    if (fd == NULL) {
        LOGE("%s: can not open file : %s\n", __FUNCTION__, VDUMP_TRAP_INFO_PATH);
        return -errno;
    }
    fprintf(fd, GET_INFO_CMD);
    fclose(fd);

    return 0;
}

static int extract_trap_info(const char *filename) {
    char line[MAXLINESIZE];
    char name[MAXLINESIZE] = { '\0', };
    int trap_detected = 0;
    FILE *fd, *fp;

    //should be opened as an empty file
    fp = fopen(filename,"w+");
    if (fp == NULL)
    {
        LOGE("%s: can not open file: %s\n", __FUNCTION__, filename);
        return 0;
    }
    //warning, trap content could be read only once after a excec_get_command function call
    //if file should be read again, be sure to run excec_get_command() before
    fd = fopen(VDUMP_TRAP_INFO_PATH, "r");
    if (fd == NULL) {
        LOGE("%s: can not open file: %s\n", __FUNCTION__, VDUMP_TRAP_INFO_PATH);
        //need to clean trap_info file
        fclose(fp);
        remove(filename);
        return 0;
    }
    while ( freadline(fd, line) > 0 ) {
        fprintf(fp,"%s", line);
        if (strstr(line, SUCCESS_FLAG)) {
            trap_detected = 1;
        }
    }
    fclose(fd);
    fclose(fp);
    return trap_detected;
}

static int read_trap_info(const char *filename) {
    if (excec_get_command() < 0)
        //get command failed => no event to generate
        return 0;
    return extract_trap_info(filename);
}

static void handle_istp_file(bool copy_istp, const char *dir, const char *cd_path){
    char des[PATHMAX];
    char istp_path[PATHMAX] = { '\0', };
    char base_coredump_path[PATHMAX] = { '\0', };
    bool create_istp_link = TRUE;
    char *istp_name;
    struct stat info;

    if (strstr(cd_path, VMM_DUMP_INFO_EXTENSION)) {
        strncpy(base_coredump_path, cd_path, strlen(cd_path) - strlen(VMM_DUMP_INFO_EXTENSION));
        snprintf(istp_path, sizeof(istp_path), "%s%s", base_coredump_path, VMM_DUMP_EXTENSION);
    }
    else if (strstr(cd_path, VMM_DUMP_EXTENSION)) {
            strncpy(istp_path, cd_path, strlen(cd_path));
    }
    else {
        LOGE("%s : neither a coredump.txt nor a istp file - %s \n", __FUNCTION__, cd_path);
        return;
    }
    if (stat(istp_path, &info) < 0) {
        LOGE("%s : stat error %d for istp file - %s \n", __FUNCTION__, errno, istp_path);
        return;
    }

    istp_name = strrchr(istp_path, '/') + 1;
    if (copy_istp) {
        if (info.st_size > ISTP_LIMIT) {
            //create an empty file to indicate filter rule
            snprintf(des, sizeof(des), "%s/%s_%dMB", dir, "istp_too_big", (uint) info.st_size/MB);
            create_empty_file(des);
        } else {
            snprintf(des, sizeof(des), "%s/%s", dir, istp_name);
            do_copy_tail(istp_path, des, 0);
            create_istp_link = FALSE;
        }
    }
    if (create_istp_link) {
        snprintf(des, sizeof(des), "%s/noupload_%s", dir, istp_name);
        if (symlink(istp_path, des) == -1) {
            LOGE("%s : create link error %d for %s -> %s\n", __FUNCTION__, errno, des, istp_path);
        }
    }
}

static void generate_event_by_type(char *type, const char *cd_path,
        const char *name, bool copy_file, bool copy_istp) {
    char *dir, *key, *event ;
    char des[PATHMAX];

    //we should generate only one crash per boot
    if (!crash_generated) {
        event = CRASHEVENT;
        key = generate_event_id(event, type);
        dir = generate_crashlog_dir(MODE_CRASH, key);
        crash_generated = TRUE;
    } else {
        event = ERROREVENT;
        key = generate_event_id(event, type);
        dir = generate_crashlog_dir(MODE_STATS, key);
    }

    if (dir != NULL) {
        do_last_kmsg_copy(dir);
        if (cd_path != NULL) {
            if (copy_file) {
                do_log_copy(type, dir, get_current_time_short(1), APLOG_TYPE);
                snprintf(des, sizeof(des), "%s/%s", dir, name);
                do_copy_tail((char *)cd_path, des, 0);
            }
            if (strstr(cd_path, EXTSDCARD_DIR))
                handle_istp_file(copy_istp, dir, cd_path);
        }
    }
    raise_event(key, event, type, NULL, dir);
    LOGE("%-8s%-22s%-20s%s %s\n", event, key, get_current_time_long(0),
         name, (dir != NULL) ? dir : "");

    free(dir);
    free(key);
}

static void handle_dump_file(const char *path, const char *name) {
    char owner[PATHMAX] = {'\0',};
    char cause[PATHMAX] = {'\0',};
    int res;

    res = get_value_in_file((char *)path, "Owner:", owner, sizeof(owner));
    if (res < 0) {
        LOGE("%s : got error %s from function call \n",
                __FUNCTION__, strerror(-res));
        return;
    }
    if (!res) {
        LOGI("%s - found owner: %s \n", __FUNCTION__, owner);
        //switch for known owner
        if (strstr(owner, "VMM")){
            generate_event_by_type("VMMTRAP", path, name, TRUE, FALSE);
        } else if (strstr(owner, "LINUX")) {
            get_value_in_file((char *)path, "Cause:", cause, sizeof(cause));
            if (strstr(cause, "Watchdog"))
                generate_event_by_type("SWWDT", path, name, TRUE, FALSE);
            else
                generate_event_by_type("IPANIC", path, name, TRUE, FALSE);
        } else if (strstr(owner, "MODEM")) {
            generate_event_by_type("MPANIC", path, name, TRUE, TRUE);
        } else if (strstr(owner, "SECURITY")) {
            generate_event_by_type("SECPANIC", path, name, TRUE, FALSE);
        } else {
        //unhandled if owner is not known
            generate_event_by_type("VMM_UNHANDLED", path, name, TRUE, FALSE);
        }
    } else {
        LOGI("no owner found in: %s - %d \n", path, res);
        generate_event_by_type("VMM_UNHANDLED", path, name, TRUE, FALSE);
    }
}

static void standard_check(const char *coredump_path, const char *name) {
    char processed_path[PATHMAX] = { '\0', };
    char temp_path[PATHMAX] = { '\0', };

    if (!strncmp(name, VMM_DUMP_PATTERN, strlen(VMM_DUMP_PATTERN))) {
        //pattern to exclude : status_ok
        if (strstr(name, "status_ok")) {
            LOGI("exclusion on : %s \n", name);
            return;
        }
        strncpy(processed_path, coredump_path, PATHMAX-1);
        str_simple_replace(processed_path, VMM_DUMP_INFO_EXTENSION, VMM_DUMP_DONE_EXTENSION);
        if (!file_exists(processed_path)) {
            LOGI("found new core dump: %s \n", coredump_path);
            strncpy(temp_path, coredump_path, PATHMAX-1);
            str_simple_replace(temp_path, VMM_DUMP_INFO_EXTENSION,
                VMM_DUMP_IN_PROGRESS_EXTENSION);
            if ((sdcard_write_failure = create_empty_file(temp_path)) == 0) {
                handle_dump_file(coredump_path, name);
                if (rename(temp_path, processed_path))
                    LOGE("%s: Could not rename '%s' to '%s', err:%s\n",
                        __FUNCTION__, temp_path, processed_path, strerror(errno));
            }
        }
    }
}

static void legacy_check(const char *coredump_path, const char *name) {
    char base_coredump_path[PATHMAX] = { '\0', };
    char txt_coredump_path[PATHMAX] = { '\0', };
    char processed_path[PATHMAX] = { '\0', };

    strncpy(base_coredump_path, coredump_path, strlen(coredump_path) - strlen(VMM_DUMP_EXTENSION));
    snprintf(txt_coredump_path, sizeof(txt_coredump_path), "%s%s", base_coredump_path, VMM_DUMP_INFO_EXTENSION);
    if (file_exists(txt_coredump_path)) {
        //not a legacy case !
        return;
    }
    strncpy(processed_path, coredump_path, PATHMAX-1);
    str_simple_replace(processed_path, VMM_DUMP_EXTENSION, VMM_DUMP_DONE_LEGACY);
    if (!file_exists(processed_path)) {
        LOGI("found legacy core dump: %s %s \n", coredump_path, txt_coredump_path);
        if ((sdcard_write_failure = create_empty_file(processed_path)) == 0)
            generate_event_by_type("VMMTRAP", coredump_path, name, FALSE, TRUE);
    }
}

static void compute_sd_card_path() {
    //read path from config
    strncpy(sd_path, cfg_vmmtrap_root_path(), PATHMAX);
    if (strlen(sd_path) <= 0) {
        //use default path
        strncpy(sd_path, EXTSDCARD_DIR, PATHMAX);
    }
    LOGI("vmmtrap - sd path computed : %s \n", sd_path);
}

static void update_sd_path() {
    FILE *fd;
    char mount_dev[256];
    char mount_dir[256];
    char mount_type[256];
    char mount_opts[256];
    int match, mount_passno, mount_freq;

    //store the real SDCard path extracted from /proc/mounts
    fd = fopen(PROC_MOUNTS, "r");
    if (fd == NULL) {
        LOGE("can not open mounts file \n");
        return ;
    }
    do {
        memset(mount_dev, '\0', sizeof(mount_dev));
        match = fscanf(fd, "%255s %255s %255s %255s %d %d\n",
                       mount_dev, mount_dir, mount_type,
                       mount_opts, &mount_freq, &mount_passno);
         if (strstr(mount_dir, sd_path)) {
             snprintf(coredump_dir_path, sizeof(coredump_dir_path), "%s/%s", mount_dir, COREDUMP_DIR);
             break;
         }
    } while (match != EOF);

    fclose(fd);
}

static void crashlog_check_vmm(void) {
    char coredump_path[PATHMAX];

    DIR *d;
    struct dirent* de;
    char prop[PROPERTY_VALUE_MAX];

    LOGI("Check vmm core dump in %s: \n", coredump_dir_path);

    if (!file_exists(coredump_dir_path))
        return ;

    d = opendir(coredump_dir_path);
    if (!d) {
        LOGE("%s: failed to open %s: %s (%d)\n", __FUNCTION__, coredump_dir_path,
             strerror(errno), errno);
        return ;
    }
    while ((de = readdir(d)) != NULL) {
        char *extension;

        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..") ||
            de->d_type != DT_REG)
            continue;

        //first, check for extension
        extension = strrchr(de->d_name, '.' );
        if (!extension) continue;
        snprintf(coredump_path, sizeof(coredump_path), "%s/%s", coredump_dir_path, de->d_name);
        //pattern to check : "coredump_*.txt"
        if (!strncmp(extension, VMM_DUMP_INFO_EXTENSION, sizeof(VMM_DUMP_INFO_EXTENSION))) {
            standard_check(coredump_path, de->d_name);
        //pattern to check for legacy: "*.istp"
        } else if (!strncmp(extension, VMM_DUMP_EXTENSION, sizeof(VMM_DUMP_EXTENSION))) {
            legacy_check(coredump_path, de->d_name);
        }

        if (sdcard_write_failure != 0) {
            char *key;
            LOGI("Error while creating file on sdcard: %s\n", strerror(-sdcard_write_failure));
            key = generate_event_id(ERROREVENT, "SDCARD_FULL");
            raise_event(key, ERROREVENT, "SDCARD_FULL", NULL, NULL);
            LOGE("%-8s%-22s%-20s%s\n", ERROREVENT, key, get_current_time_long(0), "SDCARD_FULL");
            break;
        }
    }

    //clean istp file if istp clean property is set
    property_get(PROP_ISTP_CLEAN, prop, "");
    if (strncmp(prop, "1", 1)) {
        closedir(d);
        return;
    }

    rewinddir(d);
    while ((de = readdir(d)) != NULL) {
        char *extension;

        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..") ||
            de->d_type != DT_REG)
            continue;

        //first, check for extension
        extension = strrchr(de->d_name, '.' );
        if (!extension) continue;
        //if .istp file , remove it
        if (!strncmp(extension, VMM_DUMP_EXTENSION, sizeof(VMM_DUMP_EXTENSION))) {
            snprintf(coredump_path, sizeof(coredump_path), "%s/%s", coredump_dir_path, de->d_name);
            if (!unlink(coredump_path))
                LOGI("SDCARD : Removed %s\n", coredump_path);
        }
    }
    closedir(d);
}

static int check_external_sdcard_mounted() {
    if (find_str_in_standard_file(PROC_MOUNTS, sd_path, NULL)) {
        update_sd_path();
        return 1;
    }
    return 0;
}

static void external_sdcard_monitor_mainloop() {
    int fd;
    fd_set except_fds;

    if (check_external_sdcard_mounted() != 0) {
        crashlog_check_vmm();
        return;
    }

    fd = open(PROC_MOUNTS, O_RDONLY, 0);

    FD_ZERO(&except_fds);
    FD_SET(fd, &except_fds);

    while (select(fd+1, NULL, NULL, &except_fds, NULL) >= 0) {
        if (FD_ISSET(fd, &except_fds)) {
            LOGI("Change in " PROC_MOUNTS " detected.\n");
            if (check_external_sdcard_mounted()) {
                crashlog_check_vmm();
                return;
            }
        }

        FD_ZERO(&except_fds);
        FD_SET(fd, &except_fds);
    }
    LOGE("SDCard monitoring has failed: %s\n", strerror(errno));
}

void crashlog_check_vmmtrap(bool ipanic_generated) {
    compute_sd_card_path();
    crash_generated = ipanic_generated;
    //security clean for corner case
    remove(TRAP_INFO_FILE);
    if (read_trap_info(TRAP_INFO_FILE) == 1) {
        handle_dump_file(TRAP_INFO_FILE, TRAP_INFO);
    }
    if (check_external_sdcard_mounted() != 0) {
        crashlog_check_vmm();
        return;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, (void *)external_sdcard_monitor_mainloop, NULL) < 0)
        LOGE("pthread_create error on setting up sdcard monitoring");
}
