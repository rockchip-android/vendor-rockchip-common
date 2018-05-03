/* Android Modem Traces and Logs
 *
 * Copyright (C) Intel 2012
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <strings.h>
#include <cutils/log.h>
#include <string.h>

#include "ModemInterface.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AMTL"

#define INTERFACE_CLOSED -1
#define UNUSED __attribute__((__unused__))

JNIEXPORT jint JNICALL Java_com_intel_amtl_modem_communication_ModemInterface_OpenSerialTty(
        JNIEnv *env, UNUSED jobject obj, jstring jtty_name, jint baudrate)
{
    int fd = INTERFACE_CLOSED;
    const char *tty_name = (*env)->GetStringUTFChars(env, jtty_name, 0);

    struct termios tio;
    ALOGI("OpenSerial: opening %s", tty_name);

    fd = open(tty_name, O_RDWR | CLOCAL | O_NOCTTY);
    if (fd < 0) {
        ALOGE("OpenSerial: %s (%d)", strerror(errno), errno);
        goto open_serial_failure;
    }

    struct termios terminalParameters;
    if (tcgetattr(fd, &terminalParameters)) {
        ALOGE("OpenSerial: %s (%d)", strerror(errno), errno);
        goto open_serial_failure;
    }

    cfmakeraw(&terminalParameters);
    if (tcsetattr(fd, TCSANOW, &terminalParameters)) {
        ALOGE("OpenSerial: %s (%d)", strerror(errno), errno);
        goto open_serial_failure;
    }

    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0 ) {
        ALOGE("OpenSerial: %s (%d)", strerror(errno), errno);
        goto open_serial_failure;
    }

    memset(&tio, 0, sizeof(tio));
    tio.c_cflag = B115200;
    tio.c_cflag |= CS8 | CLOCAL | CREAD;
    tio.c_iflag &= ~(INPCK | IGNPAR | PARMRK | ISTRIP | IXANY | ICRNL);
    tio.c_oflag &= ~OPOST;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 10;

    tcflush(fd, TCIFLUSH);
    cfsetispeed(&tio, baudrate);
    tcsetattr(fd, TCSANOW, &tio);

    goto open_serial_success;

open_serial_failure:
    if (fd >= 0) {
        close(fd);
        fd = INTERFACE_CLOSED;
    }

open_serial_success:
    if (fd != INTERFACE_CLOSED)
        ALOGI("OpenSerial: %s opened (%d)", tty_name, fd);
    (*env)->ReleaseStringUTFChars(env, jtty_name, tty_name);
    return fd;
}

JNIEXPORT jint JNICALL Java_com_intel_amtl_modem_communication_ModemInterface_CloseSerial(
        UNUSED JNIEnv *env, UNUSED jobject obj, jint fd)
{
    ALOGI("CloseSerial: closing file descriptor (%d)", fd);
    if (fd >= 0) {
        close(fd);
        fd = INTERFACE_CLOSED;
        ALOGD("CloseSerial: closed");
    }
    else {
        ALOGD("CloseSerial: already closed");
    }
    return 0;
}
