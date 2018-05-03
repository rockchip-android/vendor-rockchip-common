/* Android Modem Traces and Logs
 *
 * Copyright (C) Intel 2015
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
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <string.h>

#if defined (SOFIA_PLATFORM)
#include <linux/oct2.h>
#endif

#include "Ioctl.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AMTL"

#define UNUSD __attribute__((__unused__))

#if defined (SOFIA_PLATFORM)
JNIEXPORT jint JNICALL Java_com_intel_amtl_modem_communication_IoctlWrapper_GetOctPath(JNIEnv *env,
        UNUSD jobject obj, jstring jchar_device)
{
    int fd;
    enum e_oct_path out_path;
    int result = -1;
    const char *char_device = (*env)->GetStringUTFChars(env, jchar_device, 0);

    fd = open(char_device, O_RDWR);
    if (fd < 0) {
        ALOGE("GetOctPath: %s (%d)", strerror(errno), errno);
        goto out;
    }

    if (ioctl(fd, OCT_IOCTL_GET_PATH, &out_path) < 0) {
        ALOGE("GetOctPath: %s (%d)", strerror(errno), errno);
        goto out;
    }
    result = out_path;

out:
    ALOGI("GetOctPath: response from OCT_IOCTL_GET_PATH: %d", result);
    if (fd >= 0) {
        close(fd);
    }
    return result;
}
#else
JNIEXPORT jint JNICALL Java_com_intel_amtl_modem_communication_IoctlWrapper_GetOctPath(
        UNUSD JNIEnv *env, UNUSD jobject obj, UNUSD jstring jchar_device)
{
    return -1;
}
#endif
