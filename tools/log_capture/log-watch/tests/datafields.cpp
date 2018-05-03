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

#include "../DataFormat.h"

int test_data_replace_complete() {
  DataFormat df("%Z %a %0 %1 %3");
  std::vector<std::string> captures;
  captures.push_back("c0");
  captures.push_back("c1");
  captures.push_back("c2");
  captures.push_back("c3");
  df.setMap('a',"0");
  df.setMap('Z',"2");
  std::string ret = df.format(captures);
  if (ret != "2 0 c0 c1 c3")
    return 1;
  return 0;
}

int test_data_replace_incomplete() {
  DataFormat df("%Z %a %0 %1 %3");
  std::vector<std::string> captures;
  captures.push_back("c0");
  captures.push_back("c1");
  captures.push_back("c2");
  df.setMap('Z',"2");
  std::string ret = df.format(captures);
  if (ret != "2 -?- c0 c1 -?-")
    return 1;
  return 0;
}
