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

#ifndef LWLOG_H_
#define LWLOG_H_

#include <stdarg.h>

enum LogLevel {
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ABORT
};

class LwLog {
  static bool enable_android;
  static void log(LogLevel level, const char *fmt, va_list al);

 public:
  LwLog();
  virtual ~LwLog();

  static void debug(const char *fmt, ...);
  static void info(const char *fmt, ...);
  static void warn(const char *fmt, ...);
  static void error(const char *fmt, ...);
  static void critical(const char *fmt, ...);
  static void setEnableAndroid(bool enableAndroid);
};

#endif  // LWLOG_H_
