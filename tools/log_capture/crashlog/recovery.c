/* Copyright (C) Intel 2013
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
 * @file recovery.c
 * @brief File containing the function to detect and process recovery events.
 */

#include "crashutils.h"
#include "fsutils.h"
#include "privconfig.h"

#include <stdlib.h>

int crashlog_check_recovery() {
    char destination[PATHMAX];
    char *dir;
    char *key;

    //Check if trigger file exists
    if ( !file_exists(RECOVERY_ERROR_TRIGGER) ) {
        /* Nothing to do */
        return 0;
    }

    key = generate_event_id(CRASHEVENT, RECOVERY_ERROR);
    dir = generate_crashlog_dir(MODE_CRASH, key);
    if (!dir) {
        LOGE("%s: Cannot get a valid new crash directory...\n", __FUNCTION__);
        raise_event(key, CRASHEVENT, RECOVERY_ERROR, NULL, NULL);
        LOGE("%-8s%-22s%-20s%s\n", CRASHEVENT, key, get_current_time_long(0), RECOVERY_ERROR);
        remove(RECOVERY_ERROR_TRIGGER);
        free(key);
        return -1;
    }

    //copy log
    destination[0] = '\0';
    snprintf(destination, sizeof(destination), "%s/%s", dir, "recovery_last_log");
    if (do_copy(RECOVERY_ERROR_LOG, destination, MAXFILESIZE) < 0)
        LOGE("%s: %s copy failed", __FUNCTION__, RECOVERY_ERROR_LOG);
    do_last_kmsg_copy(dir);
    raise_event(key, CRASHEVENT, RECOVERY_ERROR, NULL, dir);
    LOGE("%-8s%-22s%-20s%s %s\n", CRASHEVENT, key, get_current_time_long(0), RECOVERY_ERROR, dir);
    remove(RECOVERY_ERROR_TRIGGER);
    free(key);
    free(dir);

    return 0;
}
