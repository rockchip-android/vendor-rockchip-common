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

#include "utils.h"

#include "fsutils.h"
#include "log.h"
#include "privconfig.h"

#include <fcntl.h>
#include <time.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>

/* PACKAGES */
#define CRASHREPORT_PACKAGE "com.intel.crashreport"

/* to store if the crashreport package is present */
static int crashreport_available = -1;

static int64_t current_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

int run_command_array(const char **command, unsigned int timeout) {
    pid_t pid;
    int status;
    int64_t start = current_time_ns();
    unsigned int elapsed = timeout;
    timeout *= 10;

    if ((pid = fork()) < 0) {
        LOGE("%s: Error while forking child\n", __FUNCTION__);
        return -1;
    } else if (!pid) {
        // child process
        // make sure the child dies when crashlogd dies
        prctl(PR_SET_PDEATHSIG, SIGKILL);

        // just ignore SIGPIPE, will go down with parent's
        struct sigaction sigact;
        memset(&sigact, 0, sizeof(sigact));
        sigact.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &sigact, NULL);

        execvp(command[0], (char **)command);
        LOGE("%s: Failed to launch (%s): %s\n", __FUNCTION__, command[0], strerror(errno));
        _exit(-1);
    }

    // parent process
    while(timeout--) {
        pid_t p = waitpid(pid, &status, WNOHANG);

        if (p == -1) {
            LOGE("%s: Error encountered while waiting for pid: %d (%s)\n",
                 __FUNCTION__, pid, command[0]);
            return -1;
        }

        if (p == pid)
            return status;

        usleep(100000);
    }

    timeout = elapsed;
    elapsed = (unsigned int)((current_time_ns() - start) / 100000000);
    LOGD("%s: Command (%s) timed out: %d seconds (elapsed time: %d.%d seconds)\n",
         __FUNCTION__, command[0], timeout, elapsed / 10, elapsed % 10);
    kill(pid, SIGKILL);

    // clean child to avoid zombie processes
    while(waitpid(pid, NULL, WNOHANG) >= 0){};

    return -1;
}

int run_command(const char *command, unsigned int timeout) {
    char *args[64];
    size_t max_args = sizeof(args)/sizeof(args[0]) - 1;
    char *input, *start;
    unsigned int index = 0;
    int ret;

    start = input = strdup(command);
    if (!input) {
        return -1;
    }

    while (*input != '\0' && index < max_args) {
         while (*input == ' ' || *input == '\t' || *input == '\n')
              *input++ = '\0';

         args[index++] = input;
         while (*input != '\0' && *input != ' ' &&
                *input != '\t' && *input != '\n')
              input++;
    }
    args[index] = NULL;

    ret = run_command_array((const char **)args, timeout);
    free(start);

    return ret;
}

int dump_system_information(const char *filepath) {
    FILE *fd;
    char *command;

    if (!filepath)
        return -EINVAL;

    fd = fopen(filepath, "w+");

    if (!fd)
        return -errno;

    dup2(fileno(fd), STDOUT_FILENO);
    dup2(fileno(fd), STDERR_FILENO);

    printf("\n\n*** /system/bin/top -n 1 -d 1 ***\n");
    fflush(stdout);
    run_command("/system/bin/top -n 1 -d 1", 15);
    printf("\n\n*** /system/xbin/procrank ***\n");
    fflush(stdout);
    run_command("/system/xbin/procrank", 15);
    printf("\n\n*** cat /proc/meminfo ***\n");
    fflush(stdout);
    run_command("cat /proc/meminfo", 15);

    if (file_exists(LOGCATEXT_PATH) != 0)
        command = LOGCATEXT_PATH " -b all -d";
    else
        command = "/system/bin/logcat -b all -d";

    printf("\n\n*** %s ***\n", command);
    fflush(stdout);
    run_command(command, 45);
    fflush(stdout);

    fclose(fd);
    return 0;
}

/**
 * Checks wether or not a package is present
 *
 * @param package indicating the name of the package
 * @return returns 1 if package exists, 0 if it does not, negative number if an error occured.
 *            If the package exists, pm is expected to write one line containing the package name.
 *            If the package does not exist, pm would return nothing.
 *            If however pm returns some input, we will assume an error(stderr) occured
 *                as in: pm missing or PackageManager service not started, and return -1.
 */
static int check_package_presence(const char *package) {
    char buffer[MAXLINESIZE + 1] = {0};
    char command[PATHMAX];
    int out_pipe[2];
    int prev_stdout, prev_stderr;
    int status = -1, len;

    if (!package || package[0] == '\0' || pipe(out_pipe) != 0)
        return status;

    prev_stdout = dup(STDOUT_FILENO);
    prev_stderr = dup(STDERR_FILENO);
    dup2(out_pipe[1], STDOUT_FILENO);
    dup2(out_pipe[1], STDERR_FILENO);
    close(out_pipe[1]);

    //need to add O_NONBLOCK, otherwise it will hang if nothing is output on stdout/stderr
    fcntl(out_pipe[0], F_SETFL, fcntl(out_pipe[0], F_GETFL) | O_NONBLOCK);

    snprintf(command, PATHMAX, "pm list packages -f %s", package);
    run_command(command, 15);
    fflush(stdout);

    len = read(out_pipe[0], buffer, MAXLINESIZE);
    if (len > 0) buffer[len] = 0;
    dup2(prev_stdout, STDOUT_FILENO);
    dup2(prev_stderr, STDERR_FILENO);

    if (strstr(buffer, package))
        return 1;
    else if (len > 0)
        return -1;

    return 0;
}

int is_crashreport_available() {
#ifdef __TEST__
    return 1;
#else
    static bool boot_completed = false;

    //crashreport presence will be verified until a successful check is performed
    //and boot completed has been set, in order to ensure that applications
    //installed on first boot will be reported by package manager.
    if (!boot_completed) {
        char prop[PROPERTY_VALUE_MAX];
        property_get(PROP_BOOT_STATUS, prop, "");
        boot_completed = prop[0] == '1';
    }

    if (crashreport_available < 0 && boot_completed) {
        crashreport_available = check_package_presence(CRASHREPORT_PACKAGE);
    }

    return (crashreport_available > 0);
#endif
}
