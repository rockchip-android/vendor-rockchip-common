/* Copyright (C) Intel 2015
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

/*
* Name          : enable_watchdog
* Description   : This function enables a software watchdog.
* Parameters    :
*   unsigned int timeout     -> timeout in seconds until a watchdog should
*                               occur after the last kick
*/
int enable_watchdog(unsigned int timeout);

/*
* Name          : kick_watchdog
* Description   : This function resets the watchdog timeout.
*/
void kick_watchdog();
