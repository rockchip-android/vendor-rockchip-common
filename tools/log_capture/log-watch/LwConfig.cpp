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

#include "LwConfig.h"

#include <intelconfig.h>
#include <stdlib.h>
#include <errno.h>

#include "EventWatch.h"
#include "EventAttachment.h"

#include <string>
#include <fstream>
#include <list>
#include "LwLog.h"

LwConfig *LwConfig::inst_ = NULL;

LwConfig::LwConfig() : loaded_file("") {
}

LwConfig::~LwConfig() {
}

bool LwConfig::load(std::string filename) {
  if (!loaded_file.empty()) {
    LwLog::error("Configuration already loaded %s", loaded_file.c_str());
    return false;
  }
  LwLog::info("Loading %s", filename.c_str());

  std::string buffer;
  std::ifstream fin(filename.c_str());
  getline(fin, buffer, static_cast<char>(-1));
  fin.close();

  if (buffer.empty()) {
    LwLog::error("Empty config %s", loaded_file.c_str());
    return false;
  }
  struct iconfig_parse_ctx *ctx = iconfig_alloc_context();
  if (!ctx) {
    LwLog::error("Cannot get parse context");
    return false;
  }

  iconfig_node *root = iconfig_load(ctx, buffer.c_str(), JSON);

  if (!root || root->type != IC_SINGLE || root->v.val.type != ICV_NODES) {
    LwLog::error("Invalid config %s, %s", loaded_file.c_str(),
                 iconfig_get_last_error(ctx));
    iconfig_free_context(ctx);
    return false;
  }

  iconfig_free_context(ctx);

  iconfig_node *tmp_node = get_child(&root->v.val, "root-workdir");

  if (!tmp_node || tmp_node->type != IC_SINGLE ||
      tmp_node->v.val.type != ICV_STRING) {
    LwLog::error("Cannot get workdir_root");
    iconfig_free(root);
    return false;
  }

  work_dir = tmp_node->v.val.u_vals.string;

  tmp_node = get_child(&root->v.val, "instance-name");

  if (!tmp_node || tmp_node->type != IC_SINGLE ||
      tmp_node->v.val.type != ICV_STRING) {
    LwLog::error("Cannot get instance name");
    iconfig_free(root);
    return false;
  }

  instance_name = tmp_node->v.val.u_vals.string;
  work_dir += std::string("/") + instance_name;

  tmp_node = get_child(&root->v.val, "source");
  if (!tmp_node || tmp_node->type != IC_SINGLE
      || tmp_node->v.val.type != ICV_NODES) {
    LwLog::error("Cannot get source definition");
    iconfig_free(root);
    return false;
  } else {
    const char *str = get_child_string(&tmp_node->v.val, "type", NULL);
    source_type = str ? str : "";
    str = get_child_string(&tmp_node->v.val, "arguments", NULL);
    source_args = str ? str : "";
  }

  iconfig_node *watchers_node = get_child(&root->v.val, "watchers");

  if (!watchers_node || watchers_node->type != IC_ARRAY) {
    LwLog::error("Cannot get watchers");
    iconfig_free(root);
    return false;
  }

  for (size_t w_id =0; w_id < watchers_node->v.vals.count; w_id++) {
    iconfig_node_value *watch_value = watchers_node->v.vals.data[w_id];

    if (watch_value->type != ICV_NODES)
      continue;

    // The name
    const char *tmp_c_string = get_child_string(watch_value, "name", NULL);

    std::shared_ptr<EventWatch> tmp;
    tmp = std::make_shared<EventWatch>(tmp_c_string ? tmp_c_string :
                                       std::to_string((long)w_id).c_str());
    if (!tmp)
      LwLog::critical("Cannot allocate watcher");

    tmp_c_string = get_child_string(watch_value, "start_pattern", NULL);
    if (tmp_c_string)
      tmp->setStartPattern(tmp_c_string);

    tmp_c_string = get_child_string(watch_value, "validation_pattern", NULL);
    if (tmp_c_string)
      tmp->setValidPattern(tmp_c_string);

    tmp_node = get_child(watch_value, "body_pats");
    if (tmp_node && tmp_node->type == IC_ARRAY) {
      for (size_t i = 0; i < tmp_node->v.vals.count; i++) {
        if (tmp_node->v.vals.data[i]->type == ICV_STRING)
          tmp->addBodyPattern(tmp_node->v.vals.data[i]->u_vals.string);
      }
    }

    tmp_c_string = get_child_string(watch_value, "end_pattern", NULL);
    if (tmp_c_string)
      tmp->setEndPattern(tmp_c_string);

    tmp_node = get_child(watch_value, "attachments");
    if (tmp_node && tmp_node->type == IC_ARRAY) {
      for (size_t i = 0; i < tmp_node->v.vals.count; i++) {
        const char *src = get_child_string(tmp_node->v.vals.data[i],
                                           "src", NULL);
        if (src) {
          const char *dst = get_child_string(tmp_node->v.vals.data[i],
                                             "dst", NULL);
          bool exec = get_child_integer(tmp_node->v.vals.data[i],
                                        "exec", 0) > 0;
          int wait = get_child_integer(tmp_node->v.vals.data[i],
                                       "max_run", 10) * 1000;
          if (dst)
            tmp->addAttachment(EventAttachment(src, dst, exec, wait));
          else
            tmp->addAttachment(EventAttachment(src, "", exec, wait));
        }
      }
    }

    tmp_node = get_child(watch_value, "data");
    if (tmp_node && tmp_node->type == IC_ARRAY) {
      for (size_t i = 0; i < tmp_node->v.vals.count; i++) {
        if (tmp_node->v.vals.data[i]->type == ICV_NODES) {
          int id = get_child_integer(tmp_node->v.vals.data[i], "id", i);
          bool repeat = get_child_integer(tmp_node->v.vals.data[i],
                                          "repeat", 0) > 0;
          const char *fmt = get_child_string(tmp_node->v.vals.data[i],
                                             "format", NULL);
          tmp->setDataFormats(id, fmt, repeat);
        } else {
          LwLog::error("Invalid data format[%d] for %s", i,
                       tmp->getName().c_str());
        }
      }
    }

    long tmp_val = get_child_integer(watch_value, "min_level", -1);
    if (tmp_val > 0)
      tmp->setMinLevel(tmp_val);

    tmp_val = get_child_integer(watch_value, "max_level", -1);
    if (tmp_val > 0)
      tmp->setMaxLevel(tmp_val);

    tmp_val = get_child_integer(watch_value, "event_level", 0);
    tmp->setEventLevel(tmp_val & 0xff);

    tmp_val = get_child_integer(watch_value, "max_items", -1);
    if (tmp_val > 0)
      tmp->setMaxItems(tmp_val);

    tmp_val = get_child_integer(watch_value, "max_records", -1);
    if (tmp_val > 0)
      tmp->setMaxRecords(tmp_val);

    tmp_val = get_child_integer(watch_value, "max_suspend_records", -1);
    if (tmp_val > 0)
      tmp->setMaxSuspendRecords(tmp_val);

    tmp_val = get_child_integer(watch_value, "flush_timeout", -1);
    if (tmp_val > 0)
      tmp->setFlushTimeout(tmp_val);

    tmp_val = get_child_integer(watch_value, "mailbox_max", -1);
        if (tmp_val > 0)
          tmp->setMailboxMax(tmp_val);

    tmp_val = get_child_integer(watch_value, "max_event_count", 10);
    unsigned int ec = (tmp_val > 0)?tmp_val:10;

    tmp_val = get_child_integer(watch_value, "max_event_interval", 60);
    unsigned int ei = (tmp_val >= 0)?tmp_val:60;
    tmp->setMaxEvents(ec, ei);

    tmp_val = get_child_integer(watch_value, "event_suspend_interval", -1);
    if (tmp_val >= 0)
      tmp->setEventSuspendInterval(tmp_val);

    tmp_val = get_child_integer(watch_value, "keep_last", -1);
    if (tmp_val > 0)
      tmp->setKeepLast(tmp_val);

    if (tmp->isValid()) {
      watchers.push_back(tmp);
      LwLog::info("Watch[%d]: %", w_id, tmp->getName().c_str());
    } else {
      LwLog::error("Drop invalid watch[%d]: %s", w_id, tmp->getName().c_str());
    }
  }
  iconfig_free(root);

  return !watchers.empty();
}

std::list<std::shared_ptr<EventWatch>> LwConfig::getWatchers() {
  return watchers;
}

LwConfig* LwConfig::inst() {
  if (!inst_)
    inst_ = new LwConfig();
  return inst_;
}

std::string LwConfig::getWorkDir() {
  if (work_dir.empty())
    return ".";
  return work_dir;
}

void LwConfig::release() {
  if (inst_) {
    delete inst_;
    inst_ = NULL;
  }
}

const std::string& LwConfig::getSourceArgs() const {
  return source_args;
}

const std::string& LwConfig::getSourceType() const {
  return source_type;
}

std::string LwConfig::getInstanceName() {
  if (instance_name.empty())
    return "LW";
  return instance_name;
}
