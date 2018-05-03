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

#ifndef LOGITEM_H_
#define LOGITEM_H_

#include <ctime>

#include "TimeVal.h"

class LogItem {
  unsigned char prio;
  TimeVal timestamp;
  char *msg;
  bool eof;

  LogItem(const LogItem&) { /* do not copy */ }
  LogItem& operator=(const LogItem&) { return *this;}

 public:
  LogItem();
  virtual ~LogItem();
  unsigned char getPrio() const;
  TimeVal getTimestamp() const;
  const char* getMsg() const;
  bool isEof() const;
  void setEof(bool forceFlush);
  void setMsg(char* msg);
  void setPrio(unsigned char prio);
  void setTimestamp(TimeVal timestamp);
};

#endif  // LOGITEM_H_
