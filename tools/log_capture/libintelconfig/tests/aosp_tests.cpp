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

#include <gtest/gtest.h>
#include "tests.h"

TEST(util, escape) {
    ASSERT_EQ(0, test_escape());
}

TEST(util, unescape) {
    ASSERT_EQ(0, test_unescape());
}

TEST(json, load_integer) {
    ASSERT_EQ(0, json_load_integer());
}

TEST(json, load_string) {
    ASSERT_EQ(0, json_load_string());
}

TEST(json, load_nodes) {
    ASSERT_EQ(0, json_load_nodes());
}

TEST(json, load_array) {
    ASSERT_EQ(0, json_load_array());
}

TEST(json, load_error) {
    ASSERT_EQ(0, json_load_error());
}

TEST(json, print_test) {
    ASSERT_EQ(0, json_print_test());
}
