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

#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

#include "../EventWatch.h"
#include "../LogItem.h"
#include "../LwConfig.h"
#include "../utils.h"

int test_eventwatch_invalid() {
  EventWatch ew("test_invalid");
  if (ew.isValid())
    return 1;
  return 0;
}

int test_eventwatch_invalid_bad_start() {
  EventWatch ew("test_invalid");
  ew.setStartPattern("(.+");
  if (ew.isValid())
    return 1;
  return 0;
}

int test_eventwatch_accept_one() {
  std::string test_ew_name = "test_watch";
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern(".+");
  ew.setMaxItems(1);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();

  char *msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);

  ew.waitThreadStop();

  // Check the watch root exists
  if (!utils::isDir(LwConfig::inst()->getWorkDir() + "/" + test_ew_name))
    ret = 1;

  // Check event 000
  if (!utils::isDir(
      LwConfig::inst()->getWorkDir() + "/" + test_ew_name + "/000"))
    ret = 1;

  // Check event 000 content
  if (!utils::isFile(
      LwConfig::inst()->getWorkDir() + "/" + test_ew_name + "/000/summary.txt"))
    ret = 1;

  ret = 1;
  std::string line;
  std::ifstream myfile;
  myfile.open(
      LwConfig::inst()->getWorkDir() + "/" + test_ew_name + "/000/summary.txt");
  while (std::getline(myfile, line)) {
    if (line == "some text")
      ret = 0;
  }

  utils::rmRec(LwConfig::inst()->getWorkDir() + "/" + test_ew_name, true);
  return ret;
}

int test_eventwatch_flush_eof() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern(".+");
  ew.setMaxItems(2);
  ew.setMaxRecords(5);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();

  char *msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  li = std::make_shared<LogItem>();
  msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  li = std::make_shared<LogItem>();
  msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);

  ew.waitThreadStop();

  // Check the watch root exists
  if (!utils::isDir(test_ew_path))
    ret = 1;

  // Check event 000
  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  // Check event 001
  if (utils::isDir(test_ew_path + "/001"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_history() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern(".+");
  ew.setMaxItems(1);
  ew.setMaxRecords(1);
  ew.setKeepLast(2);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();

  char *msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  li = std::make_shared<LogItem>();
  msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  li = std::make_shared<LogItem>();
  msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);

  ew.waitThreadStop();

  // Check the watch root exists
  if (!utils::isDir(test_ew_path))
    ret = 1;

  // Check event 000
  if (utils::isDir(test_ew_path + "/000"))
    ret = 1;

  // Check event 001
  if (!utils::isDir(test_ew_path + "/001"))
    ret = 1;

  // Check event 001
  if (!utils::isDir(test_ew_path + "/002"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_kick_noisy() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern(".+");
  ew.setMaxItems(1);
  ew.setMaxRecords(1);
  ew.setMaxEvents(1, 6);

  if (!ew.isValid())
    return 1;

  std::shared_ptr<LogItem> li;
  char *msg;

  li = std::make_shared<LogItem>();
  msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  li->setTimestamp(TimeVal(0));
  ew.feed(li);

  li = std::make_shared<LogItem>();
  msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  li->setTimestamp(TimeVal(1));
  ew.feed(li);

  li = std::make_shared<LogItem>();
  msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  li->setTimestamp(TimeVal(1));
  ew.feed(li);

  sleep(1);
  // by this time it should be dead
  if (ew.isEnabled()) {
    std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
    li_stop->setEof(true);
    ew.feed(li_stop);
    ret = 1;
  }
  ew.waitThreadStop();

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_gen_1() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern(".+");
  ew.setMaxItems(1);
  ew.setMaxRecords(1);
  ew.setMaxEvents(1, 0);

  if (!ew.isValid())
    return 1;

  std::shared_ptr<LogItem> li;
  char *msg;
  int count = 4;

  while (count--) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "some text %d", count);
    li->setMsg(msg);
    ew.feed(li);
  }

  sleep(1);
  // by this time it should be dead
  if (ew.isEnabled()) {
    std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
    li_stop->setEof(true);
    ew.feed(li_stop);
    ret = 1;
  }
  ew.waitThreadStop();

  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  if (utils::isDir(test_ew_path + "/001"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_gen_2() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern(".+");
  ew.setMaxItems(1);
  ew.setMaxRecords(1);
  ew.setMaxEvents(2, 0);

  if (!ew.isValid())
    return 1;

  std::shared_ptr<LogItem> li;
  char *msg;
  int count = 4;

  while (count--) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "some text %d", count);
    li->setMsg(msg);
    ew.feed(li);
  }
  sleep(1);
  // by this time it should be dead
  if (ew.isEnabled()) {
    std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
    li_stop->setEof(true);
    ew.feed(li_stop);
    ret = 1;
  }
  ew.waitThreadStop();

  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  if (!utils::isDir(test_ew_path + "/001"))
    ret = 1;

  if (utils::isDir(test_ew_path + "/002"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_kick_lazy() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern(".+");
  ew.setMaxItems(1);
  ew.setMaxRecords(1);
  ew.setMailboxMax(2);

  EventAttachment attachment("sleep 5", "sleep_res", true, 5000);
  ew.addAttachment(attachment);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();

  char *msg = new char[100];
  snprintf(msg, 100, "some text");
  li->setMsg(msg);
  ew.feed(li);

  int count = 5;
  while (count--) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "some text");
    li->setMsg(msg);
    ew.feed(li);
  }

  sleep(6);
  // by this time it should be dead
  if (ew.isEnabled()) {
    std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
    li_stop->setEof(true);
    ew.feed(li_stop);
    ret = 1;
  }
  ew.waitThreadStop();

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_suspend_interval() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setDataFormats(0,"%S",false);
  ew.setStartPattern(".+");
  ew.setMaxItems(1);
  ew.setMaxRecords(1);
  ew.setEventSuspendInterval(4);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();
  char *msg;

  for (int i = 0; i < 6; i++) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "some text %d", i);
    li->setMsg(msg);
    li->setTimestamp(TimeVal(i));
    ew.feed(li);
  }

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);
  ew.waitThreadStop();

  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  if (utils::isFile(test_ew_path + "/000/suspend.txt"))
    ret = 1;

  if (!utils::isDir(test_ew_path + "/001"))
    ret = 1;

  if (utils::isFile(test_ew_path + "/001/suspend.txt"))
    ret = 1;

  if (utils::isDir(test_ew_path + "/002"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_suspend_interval_keep() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;
  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setDataFormats(0,"%S",false);
  ew.setStartPattern(".+");
  ew.setMaxItems(1);
  ew.setMaxRecords(1);
  ew.setEventSuspendInterval(4);
  ew.setMaxSuspendRecords(2);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();
  char *msg;

  for (int i = 0; i < 6; i++) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "some text %d", i);
    li->setMsg(msg);
    li->setTimestamp(TimeVal(i));
    ew.feed(li);
  }

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);
  ew.waitThreadStop();

  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  if (utils::isFile(test_ew_path + "/000/suspend.txt"))
    ret = 1;

  if (!utils::isDir(test_ew_path + "/001"))
    ret = 1;

  if (!utils::isFile(test_ew_path + "/001/suspend.txt"))
    ret = 1;

  if (utils::isDir(test_ew_path + "/002"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}
int test_eventwatch_vlidation_pass() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;

  const char *lines[] = { "start of record", "body of record", "body of record",
      "validation of record", "ignore ", "body of record", "end of record", };

  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern("start of (r.+)");
  ew.setValidPattern("validation of (r.+)");
  ew.addBodyPattern("body of (r.+)");
  ew.setEndPattern("end of r.+");
  ew.setDataFormats(0, "%0", false);
  ew.setDataFormats(1, "%1", false);
  ew.setMaxItems(100);
  ew.setMaxRecords(1);
  ew.setEventSuspendInterval(3);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();
  char *msg;

  for (size_t i = 0; i < sizeof(lines) / sizeof(*lines); i++) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "%s", lines[i]);
    li->setMsg(msg);
    li->setTimestamp(TimeVal(i));
    ew.feed(li);
  }

  sleep(1);

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);
  ew.waitThreadStop();

  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_vlidation_pass_twice() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;

  const char *lines[] = {
      "start of record",
      "body of record",
      "body of record",
      "validation of record",
      "validation of record2",
      "ignore ",
      "body of record",
      "end of record",
  };

  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern("start of (r.+)");
  ew.setValidPattern("validation of (r.+)");
  ew.addBodyPattern("body of (r.+)");
  ew.setEndPattern("end of r.+");
  ew.setDataFormats(0, "%0", false);
  ew.setDataFormats(1, "%1", false);
  ew.setMaxItems(100);
  ew.setMaxRecords(1);
  ew.setEventSuspendInterval(3);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();
  char *msg;

  for (size_t i = 0; i < sizeof(lines) / sizeof(*lines); i++) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "%s", lines[i]);
    li->setMsg(msg);
    li->setTimestamp(TimeVal(i));
    ew.feed(li);
  }

  sleep(1);

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);
  ew.waitThreadStop();

  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_vlidation_fail() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;

  const char *lines[] = {
      "start of record",
      "body of record",
      "body of record",
      "ignore ",
      "body of record",
      "end of record",
  };

  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern("start of (r.+)");
  ew.setValidPattern("validation of (r.+)");
  ew.addBodyPattern("body of (r.+)");
  ew.setEndPattern("end of r.+");
  ew.setDataFormats(0, "%0", false);
  ew.setDataFormats(1, "%1", false);
  ew.setMaxItems(100);
  ew.setMaxRecords(1);
  ew.setEventSuspendInterval(3);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();
  char *msg;

  for (size_t i = 0; i < sizeof(lines) / sizeof(*lines); i++) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "%s", lines[i]);
    li->setMsg(msg);
    li->setTimestamp(TimeVal(i));
    ew.feed(li);
  }

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);
  ew.waitThreadStop();

  if (utils::isDir(test_ew_path + "/000"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}

int test_eventwatch_no_vlidation() {
  std::string test_ew_name = "test_watch";
  std::string test_ew_path = LwConfig::inst()->getWorkDir() + "/"
      + test_ew_name;

  const char *lines[] = {
      "start of record",
      "body of record",
      "body of record",
      "ignore ",
      "body of record",
      "end of record",
  };

  int ret = 0;
  EventWatch ew(test_ew_name.c_str());
  ew.setStartPattern("start of (r.+)");
  ew.addBodyPattern("body of (r.+)");
  ew.setEndPattern("end of r.+");
  ew.setDataFormats(0, "%0", false);
  ew.setDataFormats(1, "%1", false);
  ew.setMaxItems(100);
  ew.setMaxRecords(1);
  ew.setEventSuspendInterval(3);

  if (!ew.isValid())
    return 1;
  std::shared_ptr<LogItem> li = std::make_shared<LogItem>();
  char *msg;

  for (size_t i = 0; i < sizeof(lines) / sizeof(*lines); i++) {
    li = std::make_shared<LogItem>();
    msg = new char[100];
    snprintf(msg, 100, "%s", lines[i]);
    li->setMsg(msg);
    li->setTimestamp(TimeVal(i));
    ew.feed(li);
  }

  std::shared_ptr<LogItem> li_stop = std::make_shared<LogItem>();
  li_stop->setEof(true);
  ew.feed(li_stop);
  ew.waitThreadStop();

  if (!utils::isDir(test_ew_path + "/000"))
    ret = 1;

  utils::rmRec(test_ew_path, true);
  return ret;
}
// TODO(tsc): to be extended (end + body patters), level filtering, full content analysis
