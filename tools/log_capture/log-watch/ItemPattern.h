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

#ifndef ITEMPATTERN_H_
#define ITEMPATTERN_H_

#include <regex.h>
#include <stddef.h>
#include <string>
#include <vector>

class ItemPattern {
  std::string pattern;
  regex_t rx;
  regmatch_t *matches;
  size_t match_count;
  int last_error;
  std::string last_string;

  int countCaptures();

  ItemPattern(const ItemPattern&) { /* do not copy */ }
  ItemPattern& operator=(const ItemPattern&) { return *this;}

 public:
  explicit ItemPattern(const char *pattern);
  virtual ~ItemPattern();
  bool isValid();
  std::string getLastError();
  bool check(const char *str);
  std::vector<std::string> getLastMatches() const;
};

#endif  // ITEMPATTERN_H_
