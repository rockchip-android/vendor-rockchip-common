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

#include "EventAttachment.h"

#include <stddef.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "LwLog.h"
#include "DataFormat.h"
#include "utils.h"

EventAttachment::EventAttachment(std::string s, std::string d, bool exec,
                                 unsigned int wait) : src(s), dst(d) {
  if (src.empty()) {
    type = ATT_INVALID;
    return;
  }

  if (exec) {
    if (dst.empty())
      type = ATT_INVALID;
    else
      type = ATT_EXEC;
    max_wait = wait;
    return;
  }

  if (dst.empty())
    type = ATT_PASS_ON;
  else
    type = ATT_COPY;
}

EventAttachment::~EventAttachment() {
}

std::string EventAttachment::get(std::string base,
                                 std::vector<std::string> cap) {
  std::string f_dst = this->dst;
  std::string f_src = this->src;

  if (!cap.empty()) {
    DataFormat df_src(f_src);
    f_src = df_src.format(cap);
    DataFormat df_dst(f_dst);
    f_dst = df_dst.format(cap);
  }

  std::string full_dest = base + "/" + f_dst;

  switch (type) {
    case ATT_INVALID:
      return "";
    case ATT_PASS_ON:
      if (!utils::isFile(f_src))
        return "";
      else
        return f_src;
    case ATT_COPY:
      if (!utils::isFile(f_src)) {
        return "";
      } else {
        if (utils::copyFile(f_src, full_dest))
          return full_dest;
        else
          return "";
      }
    case ATT_EXEC: {
      if (exec(full_dest, f_src))
        return full_dest;
      else
        return "";
    }
  }
  return "";
}

bool EventAttachment::exec(std::string d, std::string command) {
  int status;
  pid_t childID;

  if (command.empty())
    command = src;
  // check if destination is valid
  std::ofstream destination;
  destination.open(d);
  if (destination.is_open()) {
    destination.close();
  } else {
    LwLog::error("Cannot access destination, %s", d.c_str());
    return false;
  }

  if ((childID = fork()) == -1) {
    LwLog::error("Cannot fork for %s", command.c_str());
    return false;
  } else if (childID == 0) {
    freopen(d.c_str(), "w", stdout);
    freopen(d.c_str(), "w", stderr);

    std::vector<std::string> args;
    std::string scpy = command;
    while (!scpy.empty()) {
      size_t pos = scpy.find(" ");
      if (pos != std::string::npos) {
        args.push_back(scpy.substr(0, pos));
        scpy.erase(0, pos + 1);
      } else {
        args.push_back(scpy);
        break;
      }
    }

    const char **argsv = new const char*[args.size() + 1];

    for (size_t i = 0; i < args.size(); i++)
      argsv[i] = args[i].c_str();
    argsv[args.size()] = reinterpret_cast<char *>(0);

    execvp(argsv[0], (char* const *) argsv);
    /*should not come to this*/
    printf("Execvp failed for, %s", command.c_str());
    delete[] argsv;
    exit(EXIT_SUCCESS);
  } else {
    for (unsigned int i = 0; i < (max_wait * 4); i++) {
      pid_t ret_pid = waitpid(childID, &status, WNOHANG | WUNTRACED);
      if (ret_pid < 0) {
        LwLog::error("Cannot wait for child, %s", command.c_str());
        return false;
      } else if (ret_pid == childID) {
        if (WIFSIGNALED(status))
          LwLog::info("Child ended because of an uncaught signal.");
        else if (WIFSTOPPED(status))
          LwLog::info("Child process has stopped.");
        return true;
      } else {
        usleep(250);
      }
    }

    LwLog::error("Child process %d (%s) takes more than %dms kill it.", childID,
                 command.c_str(), max_wait);
    kill(childID, SIGKILL);
    while (waitpid(childID, NULL, WNOHANG) >= 0) {
    }

    destination.open(d, std::ofstream::out | std::ofstream::app);
    if (destination.is_open()) {
      destination << "\n===================================================\n";
      destination << "Exec of \"" << command << "\" took more that ";
      destination << max_wait;
      destination << "ms, process killed";
      destination.close();
    }
  }
  return true;
}

std::string EventAttachment::getInfo() {
  return src + " : " + dst;
}
