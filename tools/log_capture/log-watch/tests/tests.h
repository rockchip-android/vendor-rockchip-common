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

#ifndef TESTS_TESTS_H
#define TESTS_TESTS_H

int test_attachments_get_info();
int test_attachments_pass_existing();
int test_attachments_pass_non_existing();
int test_attachments_copy_existing();
int test_attachments_copy_non_existing();
int test_attachments_copy_invalid_dest();
int test_attachments_exec_success();
int test_attachments_exec_bad_command();
int test_attachments_exec_bad_output();
int test_attachments_exec_long_command();
int test_attachments_replace_cap();

int test_data_replace_complete();
int test_data_replace_incomplete();

int test_pattern_invalid();
int test_pattern_valid();

int test_eventwatch_invalid();
int test_eventwatch_invalid_bad_start();
int test_eventwatch_accept_one();
int test_eventwatch_flush_eof();
int test_eventwatch_history();
int test_eventwatch_kick_noisy();
int test_eventwatch_gen_1();
int test_eventwatch_gen_2();
int test_eventwatch_kick_lazy();
int test_eventwatch_suspend_interval();
int test_eventwatch_suspend_interval_keep();
int test_eventwatch_vlidation_pass();
int test_eventwatch_vlidation_pass_twice();
int test_eventwatch_vlidation_fail();
int test_eventwatch_no_vlidation();

#endif  // TESTS_TESTS_H
