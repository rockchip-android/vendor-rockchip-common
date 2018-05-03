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

#ifndef LCT_H_
#define LCT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*Should be kept in sync with the internal ones*/
#define    LCT_EV_FLAGS_PRIORITY_LOW    (1<<0)

enum lct_ev_type {
    LCT_EV_STAT,
    LCT_EV_INFO,
    LCT_EV_ERROR,
    LCT_EV_LAST
};

#define LCT_ADDITIONAL_NONE         0
#define LCT_ADDITIONAL_APLOG        (1<<0)
#define LCT_ADDITIONAL_LAST_KMSG    (1<<1)
#define LCT_ADDITIONAL_FWMSG        (1<<2)

/* API */
#define MKFN(fn, ...) MKFN_N(fn, ##__VA_ARGS__, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)(__VA_ARGS__)
#define MKFN_N(fn, n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n, ...) fn##n
#define lct_log(...) MKFN(__lct_log_, ##__VA_ARGS__)

/*Use lct_log
 * lct_log(Type, Submitter_name, Ev_name, flags, [[[[[[[[Data0], Data1], Data2], Data3], Data4], Data5], filelist], add_steps])
 * */

int __lct_log_all(unsigned int type, const char *submitter,
                  const char *event, unsigned int flags,
                  const char *d0, const char *d1,
                  const char *d2, const char *d3,
                  const char *d4, const char *d5,
                  const char *flist, unsigned int add_steps);

#define __lct_log_4(Type, Submitter_name, Ev_name, flags) \
        __lct_log_all(Type, Submitter_name, Ev_name, flags, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0)

#define __lct_log_5(Type, Submitter_name, Ev_name, flags, Data0)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, NULL, NULL, NULL, NULL, NULL, NULL, 0)

#define __lct_log_6(Type, Submitter_name, Ev_name, flags, Data0, Data1)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, Data1, NULL, NULL, NULL, NULL, NULL, 0)

#define __lct_log_7(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, NULL, NULL, NULL, NULL, 0)

#define __lct_log_8(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, NULL, NULL, NULL, 0)

#define __lct_log_9(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4, NULL, NULL, 0)

#define __lct_log_10(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4, Data5)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4, Data5, NULL, 0)

#define __lct_log_11(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4, Data5, filelist)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4, Data5, filelist, 0)

#define __lct_log_12(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4, Data5, filelist, add_steps)\
        __lct_log_all(Type, Submitter_name, Ev_name, flags, Data0, Data1, Data2, Data3, Data4, Data5, filelist, add_steps)

#ifdef __cplusplus
}
#endif
#endif                          /* !LCT_H_ */
