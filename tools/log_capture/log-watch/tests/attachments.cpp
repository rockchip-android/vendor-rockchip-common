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
#include <fstream>
#include <string>

#include "../EventAttachment.h"
#include "../LwLog.h"

static void create_file(std::string name) {
  std::ofstream myfile;
  myfile.open(name);
  myfile << name << " content";
  myfile.close();
}

int test_attachments_get_info() {
  EventAttachment att("in", "out");
  std::string ret = att.getInfo();
  if (ret != "in : out")
    return 1;
  return 0;
}

int test_attachments_pass_existing() {
  create_file("some_data_file");
  EventAttachment att("some_data_file");
  std::string ret = att.get("");
  std::remove("some_data_file");
  if (ret != "some_data_file")
    return 1;
  return 0;
}

int test_attachments_pass_non_existing() {
  EventAttachment att("some_invalid_path");
  std::string ret = att.get("");
  if (!ret.empty())
    return 1;
  return 0;
}

int test_attachments_copy_existing() {
  create_file("some_data_file");
  EventAttachment att("some_data_file", "some_destination");
  std::string ret = att.get(".");
  std::remove("some_data_file");
  std::remove("./some_destination");
  if (ret != "./some_destination")
    return 1;
  return 0;
}

int test_attachments_copy_non_existing() {
  EventAttachment att("some_invalid_path", "some_destination");
  std::string ret = att.get("");
  if (!ret.empty())
    return 1;
  return 0;
}

int test_attachments_copy_invalid_dest() {
  create_file("some_data_file");
  EventAttachment att("some_data_file", "/");
  std::string ret = att.get("");
  std::remove("some_data_file");
  if (!ret.empty())
    return 1;
  return 0;
}

int test_attachments_exec_success() {
  EventAttachment att("echo some_command_output", "some_destination", true);
  std::string ret = att.get(".");
  if (ret != "./some_destination")
    return 1;
  std::ifstream myfile;
  myfile.open("./some_destination");
  myfile >> ret;
  std::remove("./some_destination");
  if (ret != "some_command_output")
    return 1;
  return 0;
}

int test_attachments_exec_bad_command() {
  EventAttachment att("some_command_bad", "some_destination", true);
  std::string ret = att.get(".");
  if (ret != "./some_destination")
    return 1;
  std::ifstream myfile;
  myfile.open("./some_destination");
  std::getline(myfile, ret);
  std::remove("./some_destination");

  if (ret != "Execvp failed for, some_command_bad")
    return 1;
  return 0;
}

int test_attachments_exec_bad_output() {
  EventAttachment att("some_command_bad", "/", true);
  std::string ret = att.get("");
  if (!ret.empty())
    return 1;
  return 0;
}

int test_attachments_exec_long_command() {
  std::ofstream shfile;
  shfile.open("test_command_file");
  shfile << "echo in test command file\n";
  shfile << "sleep 5\n";
  shfile.close();
  EventAttachment att("sh test_command_file", "some_destination", true, 500);
  std::string ret = att.get(".");
  if (ret != "./some_destination")
    return 1;
  std::ifstream myfile;
  myfile.open("./some_destination");
  while (std::getline(myfile, ret)) { }
  std::remove("./some_destination");
  std::remove("test_command_file");

  if (ret != "Exec of \"sh test_command_file\" took more that 500ms, process killed")
    return 1;
  return 0;
}

int test_attachments_replace_cap() {
  std::vector<std::string> captures(101);

  captures[0] = "cap0";
  captures[1] = "cap1";
  captures[10] = "cap10";
  captures[100] = "cap100";

  EventAttachment att("echo %100_%10", "%0_%1", true);
  std::string ret = att.get(".", captures);
  if (ret != "./cap0_cap1")
    return 1;
  std::ifstream myfile;
  myfile.open("./cap0_cap1");
  myfile >> ret;
  std::remove("./cap0_cap1");
  if (ret != "cap100_cap10")
    return 1;
  return 0;
}
