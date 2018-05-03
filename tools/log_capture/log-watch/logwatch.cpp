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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <memory>

#include "EventWatch.h"
#include "LogItem.h"
#include "LogReader.h"
#include "LwConfig.h"
#include "LwLog.h"

void usage(const char *app) {
  printf("usage:\n");
  printf("%s [-a] config-file\n", app);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (argc > 2 && !(strncmp(argv[1], "-a", 2)))
    LwLog::setEnableAndroid(true);

  if (!LwConfig::inst()->load(argv[argc-1])) {
    LwLog::error("Cannot load configuration file %s", argv[argc-1]);
    return EXIT_FAILURE;
  }

  std::list<std::shared_ptr<EventWatch>> watchers = LwConfig::inst()->getWatchers();
  if (watchers.empty()) {
    LwLog::error("No valid watchers in %s", argv[argc-1]);
    LwConfig::release();
    return EXIT_FAILURE;
  }

  LogReader *reader = LogReader::getReader(LwConfig::inst()->getSourceType(),
                                           LwConfig::inst()->getSourceArgs());
  if (!reader) {
    LwLog::error("Invalid source in %s", argv[argc-1]);
    LwConfig::release();
    return EXIT_FAILURE;
  }

  std::shared_ptr<LogItem> item = reader->get();
  do {
    for (auto &watch : watchers) {
      watch->feed(item);
    }
    item = reader->get();
  } while (!item->isEof());

  for (auto &watch : watchers) {
    watch->feed(item);
    watch->waitThreadStop();
  }

  delete reader;
  LwConfig::release();
  return EXIT_SUCCESS;
}
