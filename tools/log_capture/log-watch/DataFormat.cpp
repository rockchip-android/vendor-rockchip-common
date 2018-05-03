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

#include "DataFormat.h"

#include <regex.h>
#include <stddef.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>

DataFormat::DataFormat(std::string str, bool repeat) {
  this->source = str;
  this->repeat = repeat;
}

DataFormat::~DataFormat() {
}

void DataFormat::setCaptures(std::vector<std::string> cap) {
  captures = cap;
}

void DataFormat::setMap(char key, std::string str) {
  h_map[key] = str;
}

void DataFormat::cleanMap() {
  h_map.clear();
}

std::string DataFormat::formatMap(std::string str) {
  regex_t rx;
  if (regcomp(&rx, "%([a-zA-Z]+)", REG_EXTENDED)) {
    // just exit
    return str;
  }

  regmatch_t matches[2];
  while (!regexec(&rx, str.c_str(), 2, matches, REG_EXTENDED)) {
    char key = str.substr(matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so)
                .c_str()[0];

    if (!h_map[key].empty()) {
      str.replace(matches[0].rm_so, matches[0].rm_eo - matches[0].rm_so,
                  h_map[key]);
    } else {
      str.replace(matches[0].rm_so, matches[0].rm_eo - matches[0].rm_so,
                     "-?-");
    }
  }
  regfree(&rx);
  return str;
}

std::string DataFormat::formatCap(std::string str) {
    regex_t rx;
    if (regcomp(&rx, "%([0-9]+)", REG_EXTENDED)) {
      // just exit
      return str;
    }

    regmatch_t matches[2];
    while (!regexec(&rx, str.c_str(), 2, matches, REG_EXTENDED)) {
      int id = atoi(
          str.substr(matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so)
              .c_str());
      if (id >= 0 && (size_t) id < captures.size()) {
        str.replace(matches[0].rm_so, matches[0].rm_eo - matches[0].rm_so,
                       captures[id]);
      } else {
        str.replace(matches[0].rm_so, matches[0].rm_eo - matches[0].rm_so,
                       "-?-");
      }
    }
    regfree(&rx);
    return str;
}

bool DataFormat::isRepeat() const {
  return repeat;
}

std::string DataFormat::format(std::vector<std::string> cap,
                               std::map<char, std::string> map) {
  if (!cap.empty())
    captures = cap;

  if (!map.empty())
    h_map = map;

  std::string format = formatMap(source);
  return formatCap(format);
}
