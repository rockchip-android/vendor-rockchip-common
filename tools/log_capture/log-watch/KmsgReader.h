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
#ifndef KMSGREADER_H_
#define KMSGREADER_H_

#include <stddef.h>
#include <memory>

#include "LogReader.h"
#define LOG_MAX_LEN 1024

class KmsgReader : public LogReader {
  int fd;
  unsigned char last_prio;
  uint64_t last_timestamp;
  char read_buf[LOG_MAX_LEN];

 public:
  explicit KmsgReader(bool nonblock = false);
  virtual std::shared_ptr<LogItem> get();
  virtual ~KmsgReader();
};

#endif  // KMSGREADER_H_
