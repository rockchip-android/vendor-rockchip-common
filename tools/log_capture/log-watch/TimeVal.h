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

#ifndef TIMEVAL_H_
#define TIMEVAL_H_


class TimeVal {
 public:
  explicit TimeVal(long s = 0, long us = 0);
  virtual ~TimeVal();
  bool operator < (const TimeVal &p);
  bool operator <= (const TimeVal &p);
  bool operator == (const TimeVal &p);
  bool operator >= (const TimeVal &p);
  bool operator > (const TimeVal &p);

  TimeVal operator + (const TimeVal &p);
  TimeVal operator - (const TimeVal &p);

  TimeVal& operator = (const TimeVal &p);
  TimeVal add(long s, long us = 0);
 private:
  long sec;
  long usec;
  void redist();
};

#endif /* TIMEVAL_H_ */
