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

#include "ItemPattern.h"

#include <cstring>
#include <string>
#include <vector>

ItemPattern::ItemPattern(const char *pattern) {
  match_count = 0;
  matches = NULL;

  this->pattern = pattern;

  last_error = regcomp(&rx, pattern, REG_EXTENDED);
  if (last_error)
    return;

  match_count = countCaptures();
  matches = new regmatch_t[match_count];
}

ItemPattern::~ItemPattern() {
  if (isValid()) {
    regfree(&rx);
  }
  if (matches)
    delete[] matches;
}

bool ItemPattern::isValid() {
  return (match_count > 0);
}

int ItemPattern::countCaptures() {
  const char *str = pattern.c_str();
  int count = (str[0] == '(') ? 2 : 1;
  int i, len = strlen(str);

  for (i = 1; i < len; i++)
    if (str[i] == '(' && str[i - 1] != '\\')
      count++;
  return count;
}

std::string ItemPattern::getLastError() {
  char tmp[256];
  if (regerror(last_error, &rx, tmp, 256))
    return tmp;
  return "";
}

bool ItemPattern::check(const char* str) {
  last_error = regexec(&rx, str, match_count, matches, REG_EXTENDED);
  if (!last_error)
    last_string = str;
  return (last_error == 0);
}

std::vector<std::string> ItemPattern::getLastMatches() const {
  std::vector<std::string> ret;
  if (last_error || last_string.empty())
    return ret;
  for (size_t i = 0; i < match_count; i++)
    ret.push_back(last_string.substr(matches[i].rm_so,
                                     matches[i].rm_eo - matches[i].rm_so));

  return ret;
}

