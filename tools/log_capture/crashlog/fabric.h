/* Copyright (C) Intel 2013
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

/**
 * @file fabric.h
 * @brief File containing functions used to handle fabric events.
 *
 * This file contains the functions used to handle fabric events.
 */

#ifndef __FABRIC__H__
#define __FABRIC__H__

#ifdef CRASHLOGD_MODULE_FABRIC
int crashlog_check_fabric(char *reason, int test);
#else
static inline int crashlog_check_fabric(char *reason __attribute__((__unused__)),
                                        int test __attribute__((__unused__))){return 1;}
#endif

#endif /* __FABRIC__H__ */
