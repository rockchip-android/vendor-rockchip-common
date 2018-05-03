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

#include "crashutils.h"
#include "fsutils.h"
#include "log.h"
#include "privconfig.h"
#include "utils.h"
#include "watchdog.h"

#include <signal.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#ifdef CONFIG_BTDUMP
#include <libbtdump.h>
#endif

#define CRASHLOG_WD_SIGNAL   SIGRTMIN

static timer_t *crashlog_wd_timer = NULL;
static unsigned int crashlog_wd_timeout = 0;
static int pulse = 0;

static int update_crashlog_watchdog_timeout(int timeout) {
    struct itimerspec new_value, old_value;
    new_value.it_interval.tv_sec = new_value.it_value.tv_sec = timeout;
    new_value.it_interval.tv_nsec = new_value.it_value.tv_nsec = 0;

    if (!crashlog_wd_timer) {
        return 0;
    }

    if (timer_settime(*crashlog_wd_timer, 0, &new_value, &old_value) == 0) {
        return 1;
    }
    return 0;
}

static void stop_crashlog_watchdog() {
    update_crashlog_watchdog_timeout(0);
}

static void process_crashlogwd_event() {
    char destion[PATHMAX];
    char *key, *dir;

    key = generate_event_id("CRASHLOG_WATCHDOG", NULL);
    dir = generate_crashlog_dir(MODE_CRASH, key);
    if (!dir) {
        raise_event(key, CRASHEVENT, "CRASHLOG_WATCHDOG", NULL, NULL);
        LOGE("%-8s%-22s%-20s%s\n", CRASHEVENT, key, get_current_time_long(0), "CRASHLOG_WATCHDOG");
        free(key);
        return;
    }

#ifdef CONFIG_BTDUMP
    snprintf(destion,sizeof(destion),"%s/%s", dir, "btdump_crashlog.txt");
    FILE *f_btdump = fopen(destion, "w");
    if (f_btdump) {
        bt_process(getpid(), f_btdump);
        fclose(f_btdump);
    }
#endif

    snprintf(destion,sizeof(destion),"%s/%s", dir, "system_information.txt");
    dump_system_information(destion);

    raise_event(key, CRASHEVENT, "CRASHLOG_WATCHDOG", NULL, dir);
    LOGE("%-8s%-22s%-20s%s %s\n", CRASHEVENT, key, get_current_time_long(0),
            "CRASHLOG_WATCHDOG", dir);
    free(dir);
    free(key);
}

static void crashlog_wd_handler(int signal,
                                siginfo_t *info __attribute__((__unused__)),
                                void *context __attribute__((__unused__))) {
    if (signal == CRASHLOG_WD_SIGNAL) {

        if (!crashlog_wd_timer)
            return;

        int stage = ++pulse;
        if (stage == CRASHLOG_WD_GRANULARITY) {
            pthread_t thread;
            if (pthread_create(&thread, NULL, (void *)process_crashlogwd_event, NULL) < 0) {
                LOGE("%s - Crashlog watchdog timeout. Unable to launch corrective actions."
                        "Killing self ...\n", __FUNCTION__);
                raise(SIGKILL);
            }
            return;
        }
        else if (stage < 2 * CRASHLOG_WD_GRANULARITY) {
            return;
        }

        stop_crashlog_watchdog();
        // for some reason, comit suicide
        LOGE("%s - Crashlog watchdog timeout. Killing self ...\n", __FUNCTION__);
        raise(SIGKILL);
    }
}

int enable_watchdog(unsigned int timeout) {
    struct sigaction sigact;
    struct sigevent sevp;
    struct itimerspec new_value, old_value;

    if (timeout == 0)
        return -EINVAL;

    if (crashlog_wd_timer) {
        LOGI("%s - Crashlog watchdog is already enabled\n", __FUNCTION__);
        return -EPERM;
    }

    /* in order to avoid restarting crashlog because we were in a system
       wide suspend state, trigger multiple alarms that also require execution
       before concluding that we are in a stuck state. */
    pulse = 0;
    crashlog_wd_timeout = timeout / CRASHLOG_WD_GRANULARITY;

    crashlog_wd_timer = (timer_t *)malloc(sizeof(timer_t));
    if (!crashlog_wd_timer) {
        LOGE("%s - Could not allocate memory for wd timer\n", __FUNCTION__);
        return -ENOMEM;
    }

    // update signal handler
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = crashlog_wd_handler;

    if (sigaction(CRASHLOG_WD_SIGNAL, &sigact, NULL) == -1) {
        LOGE("%s - Error while updating signal handler\n", __FUNCTION__);
        free(crashlog_wd_timer);
        crashlog_wd_timer = NULL;
        return -1;
    }

    // set up timer
    sevp.sigev_notify = SIGEV_SIGNAL;
    sevp.sigev_signo = CRASHLOG_WD_SIGNAL;
    sevp.sigev_value.sival_ptr = crashlog_wd_timer;

    if (timer_create(CLOCK_REALTIME, &sevp, crashlog_wd_timer) == 0) {
        if (update_crashlog_watchdog_timeout(crashlog_wd_timeout)) {
            LOGI("%s - Crashlog watchdog enabled!\n", __FUNCTION__);
            return 0;
        }
    }

    LOGE("%s - Could not set up a watchdog timer\n", __FUNCTION__);
    free(crashlog_wd_timer);
    crashlog_wd_timer = NULL;
    return -1;
}

void kick_watchdog() {
    pulse = 0;

    if (crashlog_wd_timer)
        update_crashlog_watchdog_timeout(crashlog_wd_timeout);
    // else, the watchdog not started
}
