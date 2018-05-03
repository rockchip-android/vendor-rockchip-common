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

#include "../util.h"

#include <stdlib.h>
#include <string.h>

static const char *escaped = "\\\" \\\\ \\/ \\b \\f\\n \\r \\t \\u0123";
static const char *unescaped = "\" \\ / \b \f\n \r \t \\u0123";

int test_unescape(void)
{
	int ret;
	char *res = unescape_string(escaped);

	ret = strcmp(res, unescaped);
	free(res);
	return ret;
}

int test_escape(void)
{
	int ret;
	char *res = escape_string(unescaped);

	ret = strcmp(res, escaped);
	free(res);
	return ret;
}
