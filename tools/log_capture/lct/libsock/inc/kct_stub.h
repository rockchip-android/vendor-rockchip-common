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

#ifndef KCT_STUB_H
#define KCT_STUB_H

#define    EV_FLAGS_PRIORITY_LOW    (1<<0)

#ifndef MAX_SB_N
#define MAX_SB_N 32
#endif

#ifndef MAX_EV_N
#define MAX_EV_N 32
#endif

#define NETLINK_CRASHTOOL 27
#define ATTCHMT_ALIGN 4U

/* Type of events supported by crashtool */
enum ct_ev_type {
    CT_EV_STAT,
    CT_EV_INFO,
    CT_EV_ERROR,
    CT_EV_LAST
};

enum ct_attchmt_type {
    CT_ATTCHMT_DATA0,
    CT_ATTCHMT_DATA1,
    CT_ATTCHMT_DATA2,
    CT_ATTCHMT_DATA3,
    CT_ATTCHMT_DATA4,
    CT_ATTCHMT_DATA5,
    /* Always add new types after DATA5 */
    CT_ATTCHMT_BINARY,
    CT_ATTCHMT_FILELIST,
    CT_ATTCHMT_ADDITIONAL
};

#define CT_ADDITIONAL_NONE         0
#define CT_ADDITIONAL_APLOG        (1<<0)
#define CT_ADDITIONAL_LAST_KMSG    (1<<1)
#define CT_ADDITIONAL_FWMSG        (1<<2)

struct ct_attchmt {
    __u32 size;                 /* sizeof(data) */
    enum ct_attchmt_type type;
    char data[];
} __aligned(4);

struct ct_event {
    __u64 timestamp;
    char submitter_name[MAX_SB_N];
    char ev_name[MAX_EV_N];
    enum ct_ev_type type;
    __u32 attchmt_size;         /* sizeof(all_attachments inc. padding) */
    __u32 flags;
    struct ct_attchmt attachments[];
} __aligned(4);

#define ATTCHMT_ALIGNMENT    4

#ifndef KCT_ALIGN
#define __KCT_ALIGN_MASK(x, mask)    (((x) + (mask)) & ~(mask))
#define __KCT_ALIGN(x, a)            __KCT_ALIGN_MASK(x, (typeof(x))(a) - 1)
#define KCT_ALIGN(x, a)         __KCT_ALIGN((x), (a))
#endif /* !KCT_ALIGN */

#define foreach_attchmt(Event, Attchmt)                                        \
    if ((Event)->attchmt_size)                                                 \
        for ((Attchmt) = (Event)->attachments;                                 \
             (Attchmt) < (typeof(Attchmt))(((char *) (Event)->attachments) +   \
                                           (Event)->attchmt_size);             \
             (Attchmt) = (typeof(Attchmt))KCT_ALIGN(((size_t)(Attchmt))        \
                                                    + sizeof(*(Attchmt))       \
                                                    + (Attchmt)->size,         \
                                                    ATTCHMT_ALIGNMENT))

#endif
