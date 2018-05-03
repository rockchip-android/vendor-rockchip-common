/*
 * Copyright (C) Intel 2016
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

#include "TimeVal.h"


#define USEC_IN_SEC 1000000L

TimeVal::TimeVal(long s, long us)
    : sec(s),
      usec(us) {
  redist();
}

TimeVal::~TimeVal() {
}

TimeVal& TimeVal::operator =(const TimeVal &p) {
  sec = p.sec;
  usec = p.usec;
  return *this;
}

TimeVal TimeVal::add(long s, long us) {
  sec += s;
  usec += us;
  redist();
  return *this;
}

bool TimeVal::operator <=(const TimeVal& p) {
  return (operator <(p)) || (operator ==(p));
}

bool TimeVal::operator ==(const TimeVal& p) {
  return sec == p.sec && usec == p.usec;
}

bool TimeVal::operator >=(const TimeVal& p) {
  return !operator <(p);
}

bool TimeVal::operator >(const TimeVal& p) {
  return !((operator <(p)) || (operator ==(p)));
}

TimeVal TimeVal::operator +(const TimeVal& p) {
  TimeVal tmp(sec, usec);
  tmp.add(p.sec, p.usec);
  tmp.redist();
  return tmp;
}

TimeVal TimeVal::operator -(const TimeVal& p) {
  TimeVal tmp(sec, usec);
  tmp.add(-p.sec, -p.usec);
  tmp.redist();
  return tmp;
}

bool TimeVal::operator <(const TimeVal& p) {
  if (sec < p.sec) {
    return true;
  } else if (sec == p.sec) {
    return usec < p.usec;
  }
  return false;
}

void TimeVal::redist() {
  while (usec >= USEC_IN_SEC) {
    usec -= USEC_IN_SEC;
    sec++;
  }

  while (usec < 0) {
    usec += USEC_IN_SEC;
    sec--;
  }
}
