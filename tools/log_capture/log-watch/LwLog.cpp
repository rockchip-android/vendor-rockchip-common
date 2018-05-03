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

#include "LwLog.h"

#ifdef ANDROID_TARGET
#define LOG_TAG "log-watch"
#include <cutils/log.h>
#endif

#include <unistd.h>
#include <cstdio>
#include <cstdlib>

bool LwLog::enable_android = false;

LwLog::LwLog() {
}

LwLog::~LwLog() {
}

void LwLog::debug(const char* fmt, ...) {
  va_list al;

  va_start(al, fmt);
  log(DEBUG, fmt, al);
  va_end(al);
}

void LwLog::info(const char* fmt, ...) {
  va_list al;

  va_start(al, fmt);
  log(INFO, fmt, al);
  va_end(al);
}

void LwLog::warn(const char* fmt, ...) {
  va_list al;

  va_start(al, fmt);
  log(WARN, fmt, al);
  va_end(al);
}

void LwLog::error(const char* fmt, ...) {
  va_list al;

  va_start(al, fmt);
  log(ERROR, fmt, al);
  va_end(al);
}

void LwLog::critical(const char* fmt, ...) {
  va_list al;

  va_start(al, fmt);
  log(ABORT, fmt, al);
  va_end(al);
  abort();
}

#ifdef ANDROID_TARGET
#define LW_LOGD(m) \
  do {\
    if (enable_android) {\
      ALOGD("%s", m);\
    } else {\
      printf("(%d) D: %s\n", getpid(), m);\
    }\
  } while (0)

#define LW_LOGI(m) \
  do {\
    if (enable_android) {\
      ALOGI("%s", m);\
    } else {\
      printf("(%d) I: %s\n", getpid(), m);\
    }\
  } while (0)

#define LW_LOGW(m) \
  do {\
    if (enable_android) {\
      ALOGW("%s", m);\
    } else {\
      printf("(%d) W: %s\n", getpid(), m);\
    }\
  } while (0)

#define LW_LOGE(m) \
  do {\
    if (enable_android) {\
      ALOGE("%s", m);\
    } else {\
      printf("(%d) E: %s\n", getpid(), m);\
    }\
  } while (0)

#else
#define LW_LOGD(m) printf("(%d) D: %s\n", getpid(), m)
#define LW_LOGI(m) printf("(%d) I: %s\n", getpid(), m)
#define LW_LOGW(m) printf("(%d) W: %s\n", getpid(), m)
#define LW_LOGE(m) printf("(%d) E: %s\n", getpid(), m)
#endif

void LwLog::log(LogLevel level, const char* fmt, va_list al) {
  char tmp[1024];
  vsnprintf(tmp, sizeof(tmp), fmt, al);
  switch (level) {
    case DEBUG:
      LW_LOGD(tmp);
      break;
    case INFO:
      LW_LOGI(tmp);
      break;
    case WARN:
      LW_LOGW(tmp);
      break;
    case ERROR:
      LW_LOGE(tmp);
      break;
    case ABORT:
      LW_LOGE(tmp);
      LW_LOGE("ABORT");
      break;
  }
}

void LwLog::setEnableAndroid(bool enableAndroid) {
  enable_android = enableAndroid;
}
