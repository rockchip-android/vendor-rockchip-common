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

#ifndef EVENTATTACHMENT_H_
#define EVENTATTACHMENT_H_

#include <string>
#include <vector>

class EventAttachment {
  enum AttachmentType {
    ATT_INVALID,
    ATT_PASS_ON,
    ATT_COPY,
    ATT_EXEC
  };

  AttachmentType type;
  unsigned int max_wait;
  std::string src;
  std::string dst;
  bool exec(std::string d, std::string command = "");

 public:
  EventAttachment(std::string s, std::string d = "", bool exec = false,
                  unsigned int wait = 10000);
  virtual ~EventAttachment();
  std::string get(std::string base,
                  std::vector<std::string> cap = std::vector<std::string>());
  std::string getInfo();
};

#endif  // EVENTATTACHMENT_H_
