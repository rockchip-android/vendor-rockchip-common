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
 * @file tcs_wrapper.h
 * @brief File containing all functions redirecting to calls to libtcs.
 * This file is required to avoid conflicts on typedef made in libtcs.
 *
 */

#ifndef __TCS_WRAPPER_H__
#define __TCS_WRAPPER_H__

#ifdef CRASHLOGD_MODULE_MODEM
int get_modem_name(char *modem_name, unsigned int modem_id);
int get_modem_count();
#else
static inline int get_modem_name(char __attribute((unused)) * modem_name,
                                 unsigned int __attribute((unused)) modem_id) {
    return 0;
}
static inline int get_modem_count() {
    return 0;
}
#endif

#endif /* __TCS_WRAPPER_H__ */
