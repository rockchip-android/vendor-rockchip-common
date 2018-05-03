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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *unescape_string(const char *str)
{
	char *tmp, *out;	/*will not be longer that the input */
	size_t in_len, out_len = 0, i;
	int escape = 0;

	in_len = strlen(str);
	tmp = malloc(in_len + 1);
	if (!tmp)
		return NULL;

	for (i = 0; i < in_len; i++) {
		if (!escape && str[i] == '\\') {
			escape = 1;
			continue;
		}
		if (escape) {
			switch (str[i]) {
			case '\"':
				tmp[out_len] = '\"';
				out_len++;
				break;
			case '\\':
				tmp[out_len] = '\\';
				out_len++;
				break;
			case '/':
				tmp[out_len] = '/';
				out_len++;
				break;
			case 'b':
				tmp[out_len] = '\b';
				out_len++;
				break;
			case 'f':
				tmp[out_len] = '\f';
				out_len++;
				break;
			case 'n':
				tmp[out_len] = '\n';
				out_len++;
				break;
			case 'r':
				tmp[out_len] = '\r';
				out_len++;
				break;
			case 't':
				tmp[out_len] = '\t';
				out_len++;
				break;
			default:
				tmp[out_len] = '\\';
				tmp[out_len + 1] = str[i];
				out_len += 2;
				break;
			}
			escape = 0;
			continue;
		}
		if (str[i] != '\"') {
			tmp[out_len] = str[i];
			out_len++;
		}
	}
	tmp[out_len] = 0;

	/*alloc what we actually need */
	out = malloc(out_len + 1);
	if (!out)
		return tmp;

	memcpy(out, tmp, out_len);
	out[out_len] = 0;
	free(tmp);
	return out;
}

char *escape_string(const char *str)
{
	char *tmp, *out;	/*will not be longer that the 2*input */
	size_t in_len, out_len = 0, i;

	in_len = strlen(str);
	tmp = malloc(2 * in_len + 1);
	if (!tmp)
		return NULL;

	for (i = 0; i < in_len; i++) {
		switch (str[i]) {
		case '\"':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = '\"';
			out_len += 2;
			break;
		case '\\':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = '\\';
			out_len += 2;
			break;
		case '/':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = '/';
			out_len += 2;
			break;
		case '\b':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = 'b';
			out_len += 2;
			break;
		case '\f':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = 'f';
			out_len += 2;
			break;
		case '\n':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = 'n';
			out_len += 2;
			break;
		case '\r':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = 'r';
			out_len += 2;
			break;
		case '\t':
			tmp[out_len] = '\\';
			tmp[out_len + 1] = 't';
			out_len += 2;
			break;
		case 'u':
			if (out_len > 2 && tmp[out_len - 1] == '\\' &&
			    tmp[out_len - 2] == '\\') {
				tmp[out_len - 1] = 'u';
			} else {
				tmp[out_len] = str[i];
				out_len++;
			}
			break;
		default:
			tmp[out_len] = str[i];
			out_len++;
			break;
		}
	}
	tmp[out_len] = 0;

	/* alloc what we actually need */
	out = malloc(out_len + 1);
	if (!out)
		return tmp;

	memcpy(out, tmp, out_len);
	out[out_len] = 0;
	free(tmp);
	return out;
}
