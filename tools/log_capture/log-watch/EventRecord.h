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

#ifndef EVENTRECORD_H_
#define EVENTRECORD_H_

#include <cstddef>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

class LogItem;
class TimeVal;

class EventRecord {
  std::vector<std::shared_ptr<LogItem>> items;
  std::vector<std::string> captures;  // From the start log item
  bool valid;
 public:
  EventRecord();
  virtual ~EventRecord();
  void addItem(std::shared_ptr<LogItem> li);
  size_t itemCount();
  TimeVal getTimestamp() const;
  const std::vector<std::string>& getCaptures() const;
  const std::vector<std::shared_ptr<LogItem>>& getItems() const;
  void setCaptures(const std::vector<std::string>& captures);
  bool isValid() const;
  void setValid(bool valid);
};

#endif  // EVENTRECORD_H_
