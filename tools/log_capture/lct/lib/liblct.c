/*
 * Copyright (C) Intel 2014 - 2015
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
 */

#include <dlfcn.h>
#include <cutils/properties.h>
#define LOG_TAG "liblct"
#include <log/log.h>

int (*__lct_log_fn) (unsigned int, const char *, const char *,
                     unsigned int, const char *, const char *,
                     const char *, const char *, const char *,
                     const char *, const char *, unsigned int) = NULL;

void *libhdl = NULL;

int __lct_log_all(unsigned int type, const char *submitter,
                  const char *event, unsigned int flags, const char *d0,
                  const char *d1, const char *d2, const char *d3,
                  const char *d4, const char *d5, const char *flist,
                  unsigned int add_steps)
{
    if (__lct_log_fn)
        return __lct_log_fn(type, submitter, event, flags, d0, d1, d2, d3, d4,
                            d5, flist, add_steps);
    ALOGI("Skip %s event from %s, no logging lib", submitter, event);
    return -1;
}

__attribute__((constructor))
static void lib_init(void)
{
    char buf[PROPERTY_VALUE_MAX];
    property_get("ro.debuggable", buf, "0");
    if (buf[0] == '0')
        return;

    /*the lib */
    libhdl = dlopen("liblctsock.so", RTLD_LAZY);
    if (!libhdl)
        return;

    __lct_log_fn = dlsym(libhdl, "lct_log_all");

    if (!__lct_log_fn) {
        dlclose(libhdl);
        libhdl = NULL;
    }
}


__attribute__((destructor))
static void lib_destruct(void)
{
    __lct_log_fn = NULL;
    if (libhdl)
        dlclose(libhdl);
    libhdl = NULL;
}
