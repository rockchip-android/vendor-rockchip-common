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

#include "EventRecord.h"

#include <string>
#include <vector>

#include "TimeVal.h"
#include "LogItem.h"

EventRecord::EventRecord()
    : valid(false) {
}

EventRecord::~EventRecord() {
  captures.clear();
}

void EventRecord::addItem(std::shared_ptr<LogItem> li) {
  items.push_back(li);
}

const std::vector<std::string>& EventRecord::getCaptures() const {
  return captures;
}

void EventRecord::setCaptures(const std::vector<std::string>& captures) {
  this->captures = captures;
}

const std::vector<std::shared_ptr<LogItem>>& EventRecord::getItems() const {
  return items;
}

TimeVal EventRecord::getTimestamp() const {
  if (items.empty()) {
    return TimeVal(0);
  }

  return items.front()->getTimestamp();
}

size_t EventRecord::itemCount() {
  return items.size();
}

bool EventRecord::isValid() const {
  return valid;
}

void EventRecord::setValid(bool valid) {
  this->valid = valid;
}
