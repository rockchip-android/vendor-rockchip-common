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
 * @file usercrash.c
 * @brief File containing functions to process 'generic' events (javacrash,
 * tombstone, hprof and apcore events).
 */

#include "inotify_handler.h"
#include "crashutils.h"
#include "usercrash.h"
#include "dropbox.h"
#include "fsutils.h"

#include "log.h"
#include <stdlib.h>

typedef struct {
    int eventtype;
    char *keyword;
    char *tail;
    char *suffix;
} crashevent_type;

crashevent_type crashevent_fakes_array[] = {
    {TOMBSTONE_TYPE, "bionic-unit-tests-cts", "<<<", FAKE_EVENT_SUFFIX}
};

static void backup_apcoredump(char *dir, char* name, char* path) {

    char des[512] = { '\0', };
    snprintf(des, sizeof(des), "%s/%s", dir, name);
    int status = do_copy_tail(path, des, 0);
    if (status < 0)
        LOGE("backup ap core dump status: %d.\n",status);
    else
        remove(path);
}

static char * priv_filter_crashevent(int eventtype, char* path) {
    int entries = sizeof(crashevent_fakes_array) / sizeof(crashevent_fakes_array[0]);

    while (entries--) {
        if ((crashevent_fakes_array[entries].eventtype == eventtype)
                && (find_str_in_file(path, crashevent_fakes_array[entries].keyword, crashevent_fakes_array[entries].tail) == 1)) {
            return crashevent_fakes_array[entries].suffix;
        }
    }
    return "\0";
}

/*
* Name          : __process_usercrash_event
* Description   : processes hprof, apcoredump, javacrash and tombstones events
* Parameters    :
* entry: watch_entry which triggered the notification
* event: inotify_event received
*/
static int priv_process_usercrash_event(struct watch_entry *entry, struct inotify_event *event) {
    char path[PATHMAX];
    char destion[PATHMAX];
    char eventname[PATHMAX];
    char *key;
    char *dir;
    /* Check for duplicate dropbox event first */
    if ((entry->eventtype == JAVACRASH_TYPE || entry->eventtype == JAVACRASH_TYPE2 || entry->eventtype == JAVATOMBSTONE_TYPE )
            && manage_duplicate_dropbox_events(event) )
        return 1;

    snprintf(path, sizeof(path),"%s/%s", entry->eventpath, event->name);
    snprintf(eventname, sizeof(eventname),"%s%s", entry->eventname,
            priv_filter_crashevent(entry->eventtype, path));
    key = generate_event_id(CRASHEVENT, eventname);
    dir = generate_crashlog_dir(MODE_CRASH, key);
    if (!dir || !file_exists(path)) {
        if (!dir)
            LOGE("%s: Cannot get a valid new crash directory...\n", __FUNCTION__);
        else {
            LOGE("%s: Cannot access %s\n", __FUNCTION__, path);
            free(dir);
        }
        raise_event(key, CRASHEVENT, entry->eventname, NULL, NULL);
        LOGE("%-8s%-22s%-20s%s\n", CRASHEVENT, key, get_current_time_long(0), entry->eventname);
        free(key);
        return -1;
    }

    snprintf(destion,sizeof(destion),"%s/%s", dir, event->name);
    do_copy_tail(path, destion, MAXFILESIZE);
    switch (entry->eventtype) {
        case APCORE_TYPE:
            backup_apcoredump(dir, event->name, path);
            do_log_copy(eventname, dir, get_current_time_short(1), APLOG_TYPE);
            break;
        case TOMBSTONE_TYPE:
        case JAVATOMBSTONE_TYPE:
        case JAVACRASH_TYPE2:
        case JAVACRASH_TYPE:
            usleep(TIMEOUT_VALUE);
            do_log_copy(eventname, dir, get_current_time_short(1), APLOG_TYPE);
            break;
        case HPROF_TYPE:
            remove(path);
            break;
        default:
            LOGE("%s: Unexpected type of event(%d)\n", __FUNCTION__, entry->eventtype);
            break;
    }
    raise_event(key, CRASHEVENT, eventname, NULL, dir);
    LOGE("%-8s%-22s%-20s%s %s\n", CRASHEVENT, key, get_current_time_long(0), entry->eventname, dir);
    switch (entry->eventtype) {
    case TOMBSTONE_TYPE:
    case JAVACRASH_TYPE2:
    case JAVACRASH_TYPE:
#ifdef FULL_REPORT
        start_dumpstate_srv(dir, key);
        break;
#endif
    default:
        /* Event is nor JAVACRASH neither TOMBSTONE : no dumpstate necessary*/
        break;
    }
    free(key);
    free(dir);
    return 1;
}

int process_usercrash_event(struct watch_entry *entry, struct inotify_event *event) {

    return priv_process_usercrash_event(entry , event);
}

int process_hprof_event(struct watch_entry *entry, struct inotify_event *event) {
    char value[PROPERTY_VALUE_MAX] = "0";

    if (property_get(PROP_COREDUMP, value, "") <= 0 || value[0] != '1') {
        LOGE("Core dump capture is disabled - %s: %s\n", PROP_COREDUMP, value);
        return -1;
    }

    return priv_process_usercrash_event(entry , event);
}

int process_apcore_event(struct watch_entry *entry, struct inotify_event *event) {
    char value[PROPERTY_VALUE_MAX] = "0";

    if (property_get(PROP_COREDUMP, value, "") <= 0 || value[0] != '1') {
        LOGE("Core dump capture is disabled - %s: %s\n", PROP_COREDUMP, value);
        return -1;
    }

    return priv_process_usercrash_event(entry , event);
}
