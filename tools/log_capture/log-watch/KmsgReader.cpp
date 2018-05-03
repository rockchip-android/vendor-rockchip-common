/*
 * Copyright (C) Intel 2015
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

#include "KmsgReader.h"

#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "LogItem.h"
#include "LwLog.h"

#ifdef KLOG
#define FPATH "/proc/kmsg"
#else
#define FPATH "/dev/kmsg"
#endif

#define USEC_IN_SEC 1000000L
#define NSEC_IN_USEC 1000L
#define NSEC_IN_SEC (NSEC_IN_USEC * USEC_IN_SEC)
#define SEC_FROM_USEC(a) ((a)/(USEC_IN_SEC))
#define EXTRA_NSEC(a)    (((a)%(USEC_IN_SEC))*NSEC_IN_USEC)
#define EXTRA_USEC(a)    ((a)%(USEC_IN_SEC))

KmsgReader::KmsgReader(bool nonblock) :last_prio(0), last_timestamp(0) {
  int mode = O_RDONLY;
  if (nonblock)
    mode |= O_NONBLOCK;
  fd = open(FPATH, mode);
  if (fd < 0)
    LwLog::error("Cannot open " FPATH " (%d)", fd);
}

std::shared_ptr<LogItem> KmsgReader::get() {
  std::shared_ptr<LogItem> ret = std::make_shared<LogItem>();
  if (!ret)
    LwLog::critical("Cannot allocate log item");
  if (fd < 0) {
    ret->setEof(true);
    return ret;
  }

  int len = read(fd, read_buf, LOG_MAX_LEN);
  if (len <= 0) {
    ret->setEof(true);
    return ret;
  }
  /* just make sure we are null terminated */
  if ((size_t)len < LOG_MAX_LEN)
    read_buf[len] = 0;
  else
    read_buf[LOG_MAX_LEN - 1] = 0;

  /* the actual parsing*/
  unsigned char prio;
  uint64_t timestamp;

  const char *msg_start = strchr(read_buf, ';');
  if (msg_start) {
    msg_start++;
    if (sscanf(read_buf, "%hhu,%*u,%" PRIu64 ",", &prio, &timestamp) == 2) {
      last_prio = prio = prio & 7;
      last_timestamp = timestamp;
    } else {
      prio = last_prio;
      timestamp = last_timestamp;
    }
  } else {
    prio = last_prio;
    timestamp = last_timestamp;
  }

  size_t out_len = snprintf(NULL, 0, "<%d> [%5" PRIu64 ".%06" PRIu64 "] %s",
                            prio, SEC_FROM_USEC(timestamp),
                            EXTRA_USEC(timestamp),
                            msg_start ? msg_start : read_buf);

  char *msg = new char[out_len];

  if (msg) {
    snprintf(msg, out_len, "<%d> [%5" PRIu64 ".%06" PRIu64 "] %s", prio,
             SEC_FROM_USEC(timestamp), EXTRA_USEC(timestamp),
             msg_start ? msg_start : read_buf);
  }

  TimeVal ts(SEC_FROM_USEC(timestamp), EXTRA_NSEC(timestamp));
  ret->setTimestamp(ts);
  ret->setPrio(prio);
  ret->setMsg(msg);

  return ret;
}

KmsgReader::~KmsgReader() {
  if (fd > 0)
    close(fd);
}
