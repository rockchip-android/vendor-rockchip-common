/*
 * Copyright (C) Intel 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>

#include "tests.h"

#define ASSERT_EQ(x, y) do {\
	if ((x) != (y)) {\
		printf("test failed " #x " != " #y "\n");\
	} else { \
		printf("test ok " #x " == " #y "\n");\
	} } while (0)

int main(int argc, char **argv)
{
	ASSERT_EQ(0, test_unescape());
	ASSERT_EQ(0, test_escape());
	ASSERT_EQ(0, json_load_integer());
	ASSERT_EQ(0, json_load_string());
	ASSERT_EQ(0, json_load_nodes());
	ASSERT_EQ(0, json_load_array());
	ASSERT_EQ(0, json_load_error());
	ASSERT_EQ(0, json_print_test());
	return 0;
}
