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

#include <string>
#include <vector>

#include "../ItemPattern.h"

int test_pattern_invalid() {
  ItemPattern ip("(");
  if (ip.isValid())
    return 1;
  std::string  ret = ip.getLastError();
    if (ret.empty())
      return 1;
    return 0;
}

int test_pattern_valid() {
  ItemPattern ip("The quick brown (.+) over the lazy (.+)");
  if (!ip.isValid())
    return 1;
  ip.check("The quick brown fox jumps over the lazy dog");
  std::vector<std::string> mcs = ip.getLastMatches();
  if (mcs.size() != 3)
    return 1;
  if (mcs[0] != "The quick brown fox jumps over the lazy dog")
      return 1;
  if (mcs[1] != "fox jumps")
      return 1;
  if (mcs[2] != "dog")
      return 1;
  return 0;
}
