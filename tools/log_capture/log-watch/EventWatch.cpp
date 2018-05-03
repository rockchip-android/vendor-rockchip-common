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

#include "EventWatch.h"

#include <sys/stat.h>
#include <climits>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "EventRecord.h"
#include "TimeVal.h"
#include "LogItem.h"
#include "LwConfig.h"
#include "LwLog.h"
#include "utils.h"

#ifdef ANDROID_TARGET
#include <lctclient.h>
#endif

#define MAILBOX_MAX_LIMIT 5000
#define SUSPEND_RECORDS_MAX_LIMIT 50
#define DEFAULT_MAX_COUNT 10
#define DEFAULT_MAX_INTERVAL 60

EventWatch::EventWatch(const char *name)
    : max_event_count(DEFAULT_MAX_COUNT),
      max_event_interval(DEFAULT_MAX_INTERVAL),
      event_suspend_interval(0),
      keep_last(3),
      mailbox_max(MAILBOX_MAX_LIMIT),
      start_pattern(NULL),
      end_pattern(NULL),
      valid_pattern(NULL),
      min_level(0),
      max_level(7),
      max_items(1),
      event_level(0),
      max_records(10),
      max_suspend_records(0),
      flush_timeout(120),
      accept_data(true),
      flush_count(0),
      suspend_records_count(0),
      suspend_until(-1),
      thread_state(READY),
      thread(0) {
  this->name = name ? name : "Unnamed";
  sem_init(&items_available, 0, 0);
  sem_init(&started, 0, 0);
  pthread_mutex_init(&mutex, NULL);
}

EventWatch::~EventWatch() {
  // Make sure the thread is not running
  if (start_pattern)
    delete start_pattern;
  if (end_pattern)
    delete end_pattern;
  if (valid_pattern)
    delete valid_pattern;
  while (!body_patterns.empty()) {
    delete body_patterns.front();
    body_patterns.pop_front();
  }

  sem_destroy(&items_available);
  sem_destroy(&started);
  pthread_mutex_destroy(&mutex);
}

void EventWatch::addBodyPattern(const char* pattern) {
  ItemPattern *pat = new ItemPattern(pattern);
  if (!pat->isValid()) {
    LwLog::error("Cannot add body pattern: %s", pattern);
    LwLog::error("%s", pat->getLastError().c_str());
    delete pat;
    pat = NULL;
  } else {
    body_patterns.push_back(pat);
  }
}

void EventWatch::setEndPattern(const char* pattern) {
  end_pattern = new ItemPattern(pattern);
  if (!end_pattern->isValid()) {
    LwLog::error("Cannot add end pattern: %s", pattern);
    LwLog::error("%s", end_pattern->getLastError().c_str());
    delete end_pattern;
    end_pattern = NULL;
  } else {
    LwLog::debug("End pattern added: %s", pattern);
  }
}

void EventWatch::setStartPattern(const char* pattern) {
  start_pattern = new ItemPattern(pattern);
  if (!start_pattern->isValid()) {
    LwLog::error("Cannot add start pattern: %s", pattern);
    LwLog::error("%s", start_pattern->getLastError().c_str());
    delete start_pattern;
    start_pattern = NULL;
  } else {
    LwLog::debug("Start pattern added: %s", pattern);
  }
}

void EventWatch::setValidPattern(const char* pattern) {
  valid_pattern = new ItemPattern(pattern);
  if (!valid_pattern->isValid()) {
    LwLog::error("Cannot set validation pattern: %s", pattern);
    LwLog::error("%s", valid_pattern->getLastError().c_str());
    delete valid_pattern;
    valid_pattern = NULL;
  } else {
    LwLog::debug("Validation pattern added: %s", pattern);
  }
}

bool EventWatch::isValid() {
  // TODO(tsc): extend this check
  return (start_pattern && start_pattern->isValid());
}

void EventWatch::setFlushTimeout(unsigned int flushTimeout) {
  flush_timeout = flushTimeout;
}

void EventWatch::setMaxItems(size_t maxItems) {
  max_items = maxItems;
}

void EventWatch::addRecord(std::shared_ptr<EventRecord> record) {
  if (!record->isValid())
    return;
  if (event_suspend_interval && record->getTimestamp() <= suspend_until) {
    suspend_records_count++;
    if (max_suspend_records) {
      suspend_records.push_back(record);
      if (suspend_records_count > max_suspend_records)
        suspend_records.pop_front();
    }
  } else {
    records.push_back(record);
  }
}

bool EventWatch::process(std::shared_ptr<LogItem> li) {
  bool taken = false;
  bool ret = true;

  if (!li->isEof()) {
    if (start_pattern && start_pattern->check(li->getMsg())) {
      if (record)
        addRecord(record);
      record = std::make_shared<EventRecord>();
      record->addItem(li);
      if (!valid_pattern) {
        record->setCaptures(start_pattern->getLastMatches());
        record->setValid(true);
      }
      taken = true;
    } else if (record) {
      if (end_pattern && end_pattern->check(li->getMsg())) {
        record->addItem(li);
        addRecord(record);
        record.reset();
        taken = true;
      }

      if (!taken && valid_pattern && valid_pattern->check(li->getMsg())) {
        record->addItem(li);
        record->setCaptures(valid_pattern->getLastMatches());
        record->setValid(true);
        taken = true;
      }

      if (!taken) {
        if (!body_patterns.empty()) {
          for (auto pat : body_patterns) {
            if (pat->check(li->getMsg())) {
              record->addItem(li);
              taken = true;
              break;
            }
          }
        } else {
          record->addItem(li);
          taken = true;
        }
      }
    }
    if (taken && record && record->itemCount() >= max_items) {
      addRecord(record);
      record.reset();
    }
  } else {
    ret = false;
    if (record) {
      addRecord(record);
      record.reset();
    }
  }

  // flush the current record if it takes more that the timeout
  if (record &&
      (li->getTimestamp() - record->getTimestamp() >= TimeVal(flush_timeout))) {
    addRecord(record);
    record.reset();
  }

  if (records.size() >= max_records) {
    if (event_suspend_interval) {
      suspend_until = records.front()->getTimestamp()
          + TimeVal(event_suspend_interval);
    }
    flush("Max records");
  } else if (!records.empty() &&
      ((li->getTimestamp() - records.front()->getTimestamp())
          >= TimeVal(flush_timeout))) {
    if (event_suspend_interval) {
      suspend_until = records.front()->getTimestamp()
          + TimeVal(event_suspend_interval);
    }
    flush("Timeout");
  } else if (li->isEof()) {
    if (!records.empty()) {
      flush("Force flush");
    }
    if (!suspend_records.empty()) {
      suspend_records.clear();
    }
  }

  return ret;
}

void EventWatch::setMaxRecords(size_t maxRecords) {
  max_records = maxRecords;
}

bool EventWatch::feed(std::shared_ptr<LogItem> li) {
  if (!li->isEof()
      && (li->getPrio() > max_level || li->getPrio() < min_level))
    return true;

  if (!accept_data)
    return false;

  switch (thread_state) {
    case STOPPED:
      return false;
    case READY:
      threadStart();
    case RUNNING:
      pthread_mutex_lock(&mutex);
      if (mailbox.size() > mailbox_max) {
        LwLog::error("Lazy %s, mailbox size > %d", name.c_str(), mailbox_max);
        mailbox.clear();
        /* Send an eof*/
        li = std::make_shared<LogItem>();
        li->setEof(true);
        accept_data = false;
        mailbox.push_back(li);
        sem_post(&items_available);
      } else if (accept_data) {
        mailbox.push_back(li);
        sem_post(&items_available);
      }
      pthread_mutex_unlock(&mutex);
  }
  return true;
}

void EventWatch::setMaxLevel(unsigned char maxLevel) {
  max_level = maxLevel;
}

bool EventWatch::setupOutputDir(std::string path) {
  std::string root = LwConfig::inst()->getWorkDir();
  root += "/" + name;
  if (!flush_count) {
    if (!utils::isDir(LwConfig::inst()->getWorkDir())) {
      if (mkdir(LwConfig::inst()->getWorkDir().c_str(), S_IRWXU | S_IRWXG))
        return false;
    }

    if (!utils::isDir(root)) {
      if (mkdir(root.c_str(), S_IRWXU | S_IRWXG))
        return false;
    } else if (!utils::rmRec(root, false)) {
      return false;
    }
  }
  if (!utils::isDir(path)) {
    if (mkdir(path.c_str(), S_IRWXU | S_IRWXG))
      return false;
  } else if (!utils::rmRec(path, false)) {
    return false;
  }
  return true;
}

std::string EventWatch::getOutputDirName(unsigned int id) {
  char buf[PATH_MAX];
  snprintf(buf, PATH_MAX, "%s/%s/%03u", LwConfig::inst()->getWorkDir().c_str(),
           name.c_str(), id);
  return buf;
}

void EventWatch::addAttachment(EventAttachment attachment) {
  attachments.push_back(attachment);
}

void EventWatch::setMinLevel(unsigned char minLevel) {
  min_level = minLevel;
}

void EventWatch::flush(const char* reason) {
  LwLog::info("%s flush %d, reason: %s", name.c_str(), flush_count, reason);

  if (flush_count >= keep_last)
    utils::rmRec(getOutputDirName(flush_count - keep_last), true);

  std::string root = getOutputDirName(flush_count);

  if (!setupOutputDir(root))
    LwLog::error("Cannot setup output dir: %s", root.c_str());

  std::ofstream of;
  of.open(root + "/" + "summary.txt");

  int i = 0;

  if (of.is_open()) {
    for (auto &rec : records) {
      of << "Record " << i << ":\n";
      for (auto &li : rec->getItems()) {
        of << li->getMsg() << "\n";
      }
      of << "\n";
      i++;
    }
    of.close();
  }

  std::string files =  root + "/" + "summary.txt";

  if (suspend_records_count && max_suspend_records) {
    of.open(root + "/" + "suspend.txt");

    i = 0;
    if (of.is_open()) {
      if (suspend_records_count > max_suspend_records) {
        of << std::to_string(suspend_records_count - max_suspend_records);
        of << " records dropped \n";
        of << "---------------------\n\n";
      }
      for (auto &rec : suspend_records) {
        of << "Record " << i << ":\n";
        for (auto &li : rec->getItems()) {
          of << li->getMsg() << "\n";
        }
        of << "\n";
        i++;
      }
      of.close();
    }

    suspend_records.clear();
    files += std::string(";") +  root + "/" + "suspend.txt";
  }

  i = 0;
  std::vector<std::string> data_fields(6);

  for (auto &rec : records) {
    int j = 0;
    for (auto &format : data_formats) {
      if (!i || format.isRepeat()) {
        format.setMap('r', std::to_string(i));
        format.setMap('R', std::to_string(records.size()));
        format.setMap('S', std::to_string(suspend_records_count));
        std::string tmp = format.format(rec->getCaptures());
        if (!tmp.empty()) {
          if (i)
            data_fields[j] += ", " + tmp;
          else
            data_fields[j] += tmp;
        }
      }
      j++;
    }
    i++;
  }

  suspend_records_count = 0;

  /* attachments */
  for (auto &ea : attachments) {
    std::string ret = ea.get(root, records.front()->getCaptures());
    if (ret.empty())
      LwLog::error("Cannot get attachment from %s", ea.getInfo().c_str());
    else
      files += std::string(";") + ret.c_str();
  }

#ifdef ANDROID_TARGET
  int lret = lct_log(event_level, LwConfig::inst()->getInstanceName().c_str(),
      name.c_str(), 0,
      data_fields[0].empty()?NULL:data_fields[0].c_str(),
      data_fields[1].empty()?NULL:data_fields[1].c_str(),
      data_fields[2].empty()?NULL:data_fields[2].c_str(),
      data_fields[3].empty()?NULL:data_fields[3].c_str(),
      data_fields[4].empty()?NULL:data_fields[4].c_str(),
      data_fields[5].empty()?NULL:data_fields[5].c_str(),
      files.c_str());
  if (lret < 0)
    LwLog::error("lct_log returned %d", lret);
#else
  of.open(root + "/" + "event_submission.txt");
  i = 0;

  if (of.is_open()) {
    of << "lct_level: " << (int)event_level << "\n";
    of << "lct_submitter: " << LwConfig::inst()->getInstanceName() << "\n";
    of << "lct_event: " << name.c_str() << "\n";
    of << " 0,\n";
    for (int df = 0; df < 6; df++)
      if (!data_fields[df].empty())
        of << "Data[" << df << "]: " << data_fields[df].c_str() << "\n";
    of << "Att: " << files.c_str() << "\n";
    of.close();
  }
#endif

  flush_count++;

  pthread_mutex_lock(&mutex);
  logEvent(records.front()->getItems().front()->getTimestamp());

  if (accept_data && !logContinue()) {
    LwLog::error("Noisy %s, mec %d, mei %d", name.c_str(),
                 max_event_count, max_event_interval);
    std::shared_ptr<LogItem> li = std::make_shared<LogItem>();
    if (!li)
      LwLog::critical("Cannot allocate log item");
    li->setEof(true);

    accept_data = false;
    mailbox.clear();
    mailbox.push_back(li);
    sem_post(&items_available);
  }

  pthread_mutex_unlock(&mutex);
  records.clear();
}

void EventWatch::threadStart() {
  LwLog::info("Starting %s thread", name.c_str());
  pthread_create(&thread, NULL, threadEntry, this);
  sem_wait(&started);
}

void* EventWatch::threadEntry(void* self) {
  reinterpret_cast<EventWatch *>(self)->threadLoop();
  return NULL;
}

void EventWatch::threadLoop() {
  pthread_mutex_lock(&mutex);
  thread_state = RUNNING;
  sem_post(&started);
  pthread_mutex_unlock(&mutex);

  std::shared_ptr<LogItem> li;
  while (thread_state == RUNNING) {
    sem_wait(&items_available);
    pthread_mutex_lock(&mutex);
    if (mailbox.empty()) {
      pthread_mutex_unlock(&mutex);
      LwLog::error("Wake up with no data %s", name.c_str());
    } else {
      li = mailbox.front();
      mailbox.pop_front();
      pthread_mutex_unlock(&mutex);
      if (!process(li))
        break;
    }
  }
  pthread_mutex_lock(&mutex);
  thread_state = STOPPED;
  pthread_mutex_unlock(&mutex);
}

void EventWatch::waitThreadStop() {
  pthread_mutex_lock(&mutex);
  if (thread_state == RUNNING) {
    LwLog::info("Wait for %s thread", name.c_str());
  }
  pthread_mutex_unlock(&mutex);
  pthread_join(thread, NULL);
  LwLog::info("%s is now stopped", name.c_str());
}

const std::string& EventWatch::getName() const {
  return name;
}

void EventWatch::setDataFormats(unsigned int id, const char *pattern,
                                bool repeat) {
  if (id > 5)
    return;
  while (data_formats.size() <= id)
    data_formats.push_back(DataFormat(""));
  data_formats[id] = DataFormat(pattern, repeat);
}

void EventWatch::setKeepLast(unsigned int keepLast) {
  keep_last = keepLast;
}

void EventWatch::setMaxEvents(unsigned int maxEventCount, unsigned int maxEventInterval) {
  max_event_count = maxEventCount;
  max_event_interval = maxEventInterval;
  /* Check range*/
  if (max_event_interval
      && ((max_event_count * 100) / max_event_interval
          > ((DEFAULT_MAX_COUNT * 100) / DEFAULT_MAX_INTERVAL))) {
    LwLog::warn("%s Max events out of bound (%d/%d), change to 10/60",
                name.c_str(), max_event_count, max_event_interval);
    max_event_count = DEFAULT_MAX_COUNT;
    max_event_interval = DEFAULT_MAX_INTERVAL;
  }
}

bool EventWatch::isEnabled() {
  bool ret = true;
  pthread_mutex_lock(&mutex);
  if (thread_state == STOPPED)
    ret = false;
  pthread_mutex_unlock(&mutex);
  return ret;
}

void EventWatch::logEvent(TimeVal ts) {
  if (!max_event_interval) {
    return;
  }
  event_log.push_front(ts);
  if (event_log.size() > max_event_count + 1) {
    event_log.pop_back();
  }
}

bool EventWatch::logContinue() {
  if (!max_event_interval && max_event_count) {
    return (flush_count < max_event_count);
  }

  if (flush_count <= max_event_count) {
    /* We are not yet at a point when we can compare timestamps. */
    return true;
  }

  /* Check the time stamp */
  return (event_log.front() - event_log.back() >= TimeVal(max_event_interval));
}

void EventWatch::setMaxSuspendRecords(size_t max) {
  if (max > SUSPEND_RECORDS_MAX_LIMIT) {
    LwLog::error("%s: mailbox_max cannot exceed  %d", name.c_str(),
                 SUSPEND_RECORDS_MAX_LIMIT);
    mailbox_max = SUSPEND_RECORDS_MAX_LIMIT;
  } else {
    max_suspend_records = max;
  }
}

void EventWatch::setEventLevel(unsigned char eventLevel) {
  event_level = eventLevel;
#ifdef  ANDROID_TARGET
  /*Keep it in lct limits*/
  if (event_level > LCT_EV_ERROR)
    event_level = LCT_EV_ERROR;
#endif
}

void EventWatch::setEventSuspendInterval(unsigned int EventSuspendInterval) {
  event_suspend_interval = EventSuspendInterval;
}

void EventWatch::setMailboxMax(unsigned int max) {
  if (max > MAILBOX_MAX_LIMIT)
    LwLog::error("%s: mailbox_max cannot exceed  %d", name.c_str(),
                 MAILBOX_MAX_LIMIT);
  mailbox_max = max > MAILBOX_MAX_LIMIT ? MAILBOX_MAX_LIMIT : max;
}
