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

#define LOG_TAG "liblctclient-stub"
#include <log/log.h>

int lct_log_all(unsigned int __attribute((unused)) type,
                const char *submitter, const char *event,
                unsigned int __attribute((unused)) flags,
                const char __attribute((unused)) * d0,
                const char __attribute((unused)) * d1,
                const char __attribute((unused)) * d2,
                const char __attribute((unused)) * d3,
                const char __attribute((unused)) * d4,
                const char __attribute((unused)) * d5,
                const char __attribute((unused)) * flist,
                unsigned int __attribute((unused)) add_steps)
{
    ALOGI("Skip %s event from %s, LCT is not enabled", submitter, event);
    return -1;
}
