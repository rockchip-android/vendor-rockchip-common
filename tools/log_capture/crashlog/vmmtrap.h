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
 * @file vmmtrap.h
 * @brief File containing functions to detect vmm crashes.
 */

#ifndef __VMMTRAP_H__
#define __VMMTRAP_H__

#ifdef CONFIG_SOFIA
void crashlog_check_vmmtrap(bool ipanic_generated);
#else
static inline void crashlog_check_vmmtrap(bool ipanic_generated __unused) {}
#endif
#endif /* __VMMTRAP_H__ */
