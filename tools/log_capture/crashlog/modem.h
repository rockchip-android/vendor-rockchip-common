/* * Copyright (C) Intel 2010
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
 * @file modem.h
 * @brief File containing functions to handle the processing of modem events.
 *
 * This file contains the functions to handle the processing of modem events and
 * modem shutdown events.
 */

#ifndef __MODEM_H__
#define __MODEM_H__

#include "privconfig.h"
#include "inotify_handler.h"

#include <sys/types.h>

#ifdef CRASHLOGD_MODULE_MODEM
int crashlog_check_mpanic_abort();
#else
static inline int crashlog_check_mpanic_abort() { return 0; }
#endif

#endif /* __MODEM_H__ */
