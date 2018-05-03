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

/**
 * @file utils.h
 * @brief File containing generic useful functions that could be reused.
 *
 */

#ifndef __UTILS_H__
#define __UTILS_H__

/**
 * Runs a specified command and waits for it to finish for
 * a given amount of time.
 *
 * @param command indicates a null terminated array of parameters,
 *                first parameter being the command to be run while
 *                the rest represent the arguments used.
 * @param timeout value in seconds before stopping the process on
 *                which the command is running
 * @return -1 on failure, termination status otherwise
 */
int run_command_array(const char **command, unsigned int timeout);

/**
 * Runs a specified command and waits for it to finish for
 * a given amount of time.
 *
 * @param command indicates a string that contains the command
 *                and arguments that are to be executed.
 * @param timeout value in seconds before stopping the process on
 *                which the command is running
 * @return -1 on failure, termination status otherwise
 */
int run_command(const char *command, unsigned int timeout);

/**
 * Outputs to a specified file various system information
*
 * @param filepath indicating the output directory
 * @return 0 on success, -errno code if error
 */
int dump_system_information(const char *filepath);

/**
 * Checks wether or not a crashreport is available.
 * Actual check is performed only until a successful verification is performed
 *     since crashlog started. Afterwards, the presence is memorized.
 */
int is_crashreport_available();
#endif
