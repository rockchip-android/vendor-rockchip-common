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

#ifndef EVENTWATCH_H_
#define EVENTWATCH_H_

#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <ctime>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "DataFormat.h"
#include "EventAttachment.h"
#include "ItemPattern.h"
#include "TimeVal.h"

class LogItem;
class EventRecord;
class TimeVal;

enum TState {
  READY,  // Could be started
  RUNNING,
  STOPPED,  // Not working anymore (was disabled)
};

class EventWatch {
  // Configuration
  std::string name;
  unsigned int max_event_count;
  unsigned int max_event_interval;
  unsigned int event_suspend_interval;
  unsigned int keep_last;
  unsigned int mailbox_max;

  ItemPattern *start_pattern, *end_pattern, *valid_pattern;
  std::list<ItemPattern *> body_patterns;
  unsigned char min_level, max_level;
  size_t max_items;
  unsigned char event_level;

  std::vector<DataFormat> data_formats;
  std::list<EventAttachment> attachments;
  size_t max_records;
  size_t max_suspend_records;
  unsigned int flush_timeout;

  // Execution
  std::list<std::shared_ptr<EventRecord>> records;
  std::list<std::shared_ptr<EventRecord>> suspend_records;
  std::shared_ptr<EventRecord> record;
  std::list<std::shared_ptr<LogItem>> mailbox;

  std::list<TimeVal> event_log;

  bool accept_data;
  unsigned int flush_count;
  size_t suspend_records_count;
  TimeVal suspend_until;

  sem_t items_available;
  sem_t started;
  pthread_mutex_t mutex;

  TState thread_state;
  pthread_t thread;

  static void *threadEntry(void *self);
  void threadLoop();
  void threadStart();
  bool setupOutputDir(std::string path);
  std::string getOutputDirName(unsigned int id);
  void flush(const char *reason);
  bool process(std::shared_ptr<LogItem> li);
  void addRecord(std::shared_ptr<EventRecord> record);
  void logEvent(TimeVal ts);
  bool logContinue();

  EventWatch(const EventWatch&) { /* do not copy */ }
  EventWatch& operator=(const EventWatch&) { return *this;}

 public:
  explicit EventWatch(const char *name);
  virtual ~EventWatch();
  void addBodyPattern(const char* pattern);
  void setEndPattern(const char* pattern);
  void setStartPattern(const char* pattern);
  void setValidPattern(const char* pattern);
  bool isValid();
  bool isEnabled();
  bool feed(std::shared_ptr<LogItem> li);
  void setFlushTimeout(unsigned int flushTimeout);
  void setMaxItems(size_t max);
  void setMaxRecords(size_t maxRecords);
  void setMaxSuspendRecords(size_t max);
  void setMaxLevel(unsigned char maxLevel);
  void setMinLevel(unsigned char minLevel);
  void addAttachment(EventAttachment attachment);
  void waitThreadStop();
  void setDataFormats(unsigned int id, const char *pattern, bool repeat);
  const std::string& getName() const;
  void setMailboxMax(unsigned int max);
  void setKeepLast(unsigned int keepLast);
  void setMaxEvents(unsigned int maxEventCount, unsigned int maxEventInterval);
  void setEventSuspendInterval(unsigned int EventSuspendInterval);
  void setEventLevel(unsigned char eventLevel);
};

#endif  // EVENTWATCH_H_
