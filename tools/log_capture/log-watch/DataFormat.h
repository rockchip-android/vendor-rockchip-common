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

#ifndef DATAFORMAT_H_
#define DATAFORMAT_H_

#include <stddef.h>
#include <string>
#include <vector>
#include <map>

class DataFormat {
  std::string source;
  std::map<char, std::string> h_map;
  std::vector<std::string> captures;
  bool repeat;

  std::string formatMap(std::string str);
  std::string formatCap(std::string str);
 public:
  explicit DataFormat(std::string str, bool repeat = false);
  virtual ~DataFormat();

  std::string format(
      std::vector<std::string> cap, std::map<char, std::string> h_map =
          std::map<char, std::string>());

  void setCaptures(std::vector<std::string> cap);
  void setMap(char key, std::string str);
  void cleanMap();
  bool isRepeat() const;
};

#endif /* DATAFORMAT_H_ */
