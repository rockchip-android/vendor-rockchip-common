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

#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifndef CRASHLOGD_MODULE_KCT
#include "kct_stub.h"
#else
#include <linux/kct.h>
#endif

#include "lctpriv.h"
#define LOG_TAG "liblctclient"
#include <log/log.h>

static struct ct_event *lct_alloc_event(const char *submitter_name,
                                        const char *ev_name,
                                        enum ct_ev_type ev_type,
                                        unsigned int flags)
{
    struct ct_event *ev = NULL;

    if (submitter_name && ev_name) {
        ev = calloc(1, sizeof(*ev));
        if (ev) {
            strncpy(ev->submitter_name, submitter_name, MAX_SB_N);
            ev->submitter_name[MAX_SB_N - 1] = '\0';
            strncpy(ev->ev_name, ev_name, MAX_EV_N);
            ev->ev_name[MAX_EV_N - 1] = '\0';

            ev->timestamp = time(NULL);
            ev->flags = flags;
            ev->type = ev_type;
        } else {
            ALOGW("Cannot allocate event");
        }
    }

    return ev;
}

static int lct_attach(struct ct_event **ev,
                      enum ct_attchmt_type at_type,
                      unsigned int size, const void *data)
{
    __u32 new_size = sizeof(struct ct_event) + (*ev)->attchmt_size +
        KCT_ALIGN(size + sizeof(struct ct_attchmt), ATTCHMT_ALIGNMENT);
    struct ct_event *new_ev = realloc(*ev, new_size);
    if (new_ev) {
        struct ct_attchmt *new_attchmt = (struct ct_attchmt *)
            (((char *)new_ev->attachments) + new_ev->attchmt_size);

        new_attchmt->size = size;
        new_attchmt->type = at_type;
        memcpy(new_attchmt->data, data, size);

        new_ev->attchmt_size = new_size - sizeof(struct ct_event);

        *ev = new_ev;
    } else {
        ALOGW("Cannot add attachment");
    }
    return new_ev ? 0 : -ENOMEM;
}

static int lct_log_event(struct ct_event *ev)
{
    int ret = -1;
    int sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd > 0) {
        const struct sockaddr_un addr = {
            .sun_family = AF_UNIX,
            .sun_path = SK_NAME,
        };
        ret = sendto(sock_fd, ev, PKT_SIZE(ev),
                     0x0, (const struct sockaddr *)&addr, sizeof(addr));
        close(sock_fd);
    } else {
        ALOGE("Cannot open LCT socket event %s from %s skipped",
              ev->ev_name, ev->submitter_name);
    }
    free(ev);
    return ret;
}

int lct_log_all(unsigned int type, const char *submitter,
                const char *event, unsigned int flags, const char *d0,
                const char *d1, const char *d2, const char *d3,
                const char *d4, const char *d5, const char *flist,
                unsigned int add_steps)
{
    struct ct_event *ev = lct_alloc_event(submitter, event, type, flags);
    if (ev) {
        if (d0)
            lct_attach(&ev, CT_ATTCHMT_DATA0, strlen(d0) + 1, d0);
        if (d1)
            lct_attach(&ev, CT_ATTCHMT_DATA1, strlen(d1) + 1, d1);
        if (d2)
            lct_attach(&ev, CT_ATTCHMT_DATA2, strlen(d2) + 1, d2);
        if (d3)
            lct_attach(&ev, CT_ATTCHMT_DATA3, strlen(d3) + 1, d3);
        if (d4)
            lct_attach(&ev, CT_ATTCHMT_DATA4, strlen(d4) + 1, d4);
        if (d5)
            lct_attach(&ev, CT_ATTCHMT_DATA5, strlen(d5) + 1, d5);
        if (flist)
            lct_attach(&ev, CT_ATTCHMT_FILELIST, strlen(flist) + 1, flist);
        if (add_steps)
            lct_attach(&ev, CT_ATTCHMT_ADDITIONAL, sizeof(add_steps),
                       &add_steps);
        return lct_log_event(ev);
    }
    return -ENOMEM;
}
