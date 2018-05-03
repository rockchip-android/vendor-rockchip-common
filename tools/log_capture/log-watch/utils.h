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

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <ctime>

namespace utils {
  bool isDir(std::string dir);
  bool isFile(std::string path);
  bool copyFile(std::string s, std::string d);
  bool rmRec(std::string path, bool rem_root);
};

#endif /* UTILS_H_ */
