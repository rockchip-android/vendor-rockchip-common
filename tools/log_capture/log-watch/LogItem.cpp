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

#include "LogItem.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TimeVal.h"

LogItem::~LogItem() {
  if (msg)
    delete[] msg;
}

unsigned char LogItem::getPrio() const {
  return prio;
}

TimeVal LogItem::getTimestamp() const {
  return timestamp;
}

bool LogItem::isEof() const {
  return eof;
}

LogItem::LogItem()
    : prio(0),
      timestamp(0),
      msg(NULL),
      eof(false) {
}

void LogItem::setMsg(char* msg) {
  this->msg = msg;
}

void LogItem::setPrio(unsigned char prio) {
  this->prio = prio;
}

void LogItem::setTimestamp(TimeVal timestamp) {
  this->timestamp = timestamp;
}

void LogItem::setEof(bool forceFlush) {
  eof = forceFlush;
}

const char* LogItem::getMsg() const {
  return msg;
}
