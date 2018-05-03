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

#ifndef LWCONFIG_H_
#define LWCONFIG_H_

#include <string>
#include <list>
#include <memory>

class EventWatch;

class LwConfig {
  static LwConfig *inst_;
  std::string loaded_file;
  std::string instance_name;
  std::list<std::shared_ptr<EventWatch>> watchers;
  std::string work_dir;
  std::string source_type;
  std::string source_args;

 public:
  LwConfig();
  virtual ~LwConfig();
  bool load(std::string filename);
  static LwConfig *inst();
  static void release();
  std::list<std::shared_ptr<EventWatch>> getWatchers();
  std::string getWorkDir();
  const std::string& getSourceArgs() const;
  const std::string& getSourceType() const;
  std::string getInstanceName();
};

#endif /* LWCONFIG_H_ */
