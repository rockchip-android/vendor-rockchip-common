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

#include "utils.h"

#include <dirent.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <string>

bool utils::isDir(std::string dir) {
  struct stat buf;
  if (stat(dir.c_str(), &buf))
    return false;
  return S_ISDIR(buf.st_mode);
}

bool utils::isFile(std::string path) {
  struct stat buf;
  if (stat(path.c_str(), &buf))
    return false;
  return S_ISREG(buf.st_mode);
}

bool utils::copyFile(std::string s, std::string d) {
  int source = open(s.c_str(), O_RDONLY | O_NONBLOCK, 0);
  if (source < 0)
    return false;

  int dest = open(d.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP);
  if (dest < 0) {
    close(source);
    return false;
  }
  char buf[4096];
  ssize_t size;

  while ((size = read(source, buf, 4096)) > 0) {
    write(dest, buf, size);
  }

  close(source);
  close(dest);
  return true;
}

bool utils::rmRec(std::string path, bool rem_root) {
  if (isDir(path)) {
    DIR *d = opendir(path.c_str());

    if (!d)
      return false;
    struct dirent *p = readdir(d);
    while (p) {
      if (strcmp(p->d_name, ".") && strcmp(p->d_name, "..")) {
        if (!rmRec(path + "/" + p->d_name, true)) {
          closedir(d);
          return false;
        }
      }
      p = readdir(d);
    }
    closedir(d);
    if (rem_root)
      rmdir(path.c_str());
  } else {
    if (unlink(path.c_str()))
      return false;
  }
  return true;
}
