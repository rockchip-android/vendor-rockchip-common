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
#include <stdio.h>
#include <stdlib.h>

#include "incl/intelconfig.h"
#include "util.h"

#define INDENT_STEP 1
#define INDENT_CHAR '\t'

static size_t jp_node(struct iconfig_node *node, size_t level,
		      char *buf, size_t max_size);

static inline size_t jp_indent(size_t level, char *buf, size_t max_size)
{
	size_t indent = level * INDENT_STEP;
	size_t tmp = indent;

	if (level > max_size)
		return 0;
	while (tmp--)
		*(buf++) = INDENT_CHAR;
	return indent;
}

static size_t jp_nodes_array(struct iconfig_nodes_array *arr, size_t level,
			     char *buf, size_t max_size)
{
	size_t i, rem, ret;

	rem = max_size;
	for (i = 0; i < arr->count; i++) {
		if (i)
			ret = snprintf(buf, rem, ",\n");
		else
			ret = snprintf(buf, rem, "\n");

		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;
		ret = jp_node(arr->data[i], level + 1, buf, rem);
		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;
	}
	return max_size - rem;
}

static size_t jp_node_value(struct iconfig_node_value *value, size_t level,
			    char *buf, size_t max_size)
{
	size_t ret, rem;
	char *escaped;

	rem = max_size;
	switch (value->type) {
	case ICV_INTEGER:
		ret = snprintf(buf, rem, "%ld", value->u_vals.integer);
		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;
		break;
	case ICV_STRING:
		escaped = escape_string(value->u_vals.string);
		ret = snprintf(buf, rem, "\"%s\"", escaped ? escaped : "ERROR");
		free(escaped);
		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;
		break;
	case ICV_NODES:
		ret = snprintf(buf, rem, "{");
		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;
		if (value->u_vals.nodes->count) {
			ret = jp_nodes_array(value->u_vals.nodes, level, buf,
					     max_size);
			if (!ret || ret > rem)
				return 0;
			buf += ret;
			rem -= ret;
		}

		ret = snprintf(buf, rem, "\n");
		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;

		if (level) {
			if (!jp_indent(level, buf, rem))
				return 0;
			buf += level;
			rem -= level;
		}

		ret = snprintf(buf, rem, "}");
		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;
		break;
	}
	return max_size - rem;
}

static size_t jp_node(struct iconfig_node *node, size_t level,
		      char *buf, size_t max_size)
{
	size_t ret, rem;

	rem = max_size;

	if (node->type == IC_SINGLE) {
		if (level) {
			ret = jp_indent(level, buf, rem);
			if (!ret)
				return 0;
			buf += ret;
			rem -= ret;

			ret = snprintf(buf, rem, "\"%s\": ", node->name);
			if (!ret || ret > rem)
				return 0;
			buf += ret;
			rem -= ret;
		}

		ret = jp_node_value(&node->v.val, level, buf, rem);
		if (!ret || ret > rem)
			return 0;
		buf += ret;
		rem -= ret;
	} else {
		if (node->v.vals.count) {
			size_t i;

			if (level) {
				ret = jp_indent(level, buf, rem);
				if (!ret)
					return 0;
				buf += ret;
				rem -= ret;
			}

			ret = snprintf(buf, rem, "\"%s\": [",
				       node->name);
			if (!ret || ret > rem)
				return 0;
			buf += ret;
			rem -= ret;

			for (i = 0; i < node->v.vals.count; i++) {
				if (i)
					ret = snprintf(buf, rem, ",\n");
				else
					ret = snprintf(buf, rem, "\n");

				if (!ret || ret > rem)
					return 0;
				buf += ret;
				rem -= ret;

				ret = jp_indent(level + 1, buf, rem);
				if (!ret)
					return 0;
				buf += ret;
				rem -= ret;

				ret = jp_node_value(node->v.vals.data[i],
						    level + 1, buf, rem);
				if (!ret || ret > rem)
					return 0;
				buf += ret;
				rem -= ret;
			}

			ret = snprintf(buf, rem, "\n");
			if (!ret || ret > rem)
				return 0;
			buf += ret;
			rem -= ret;

			if (level) {
				ret = jp_indent(level, buf, rem);
				if (!ret)
					return 0;
				buf += ret;
				rem -= ret;
			}

			ret = snprintf(buf, rem, "]");
			if (!ret || ret > rem)
				return 0;
			buf += ret;
			rem -= ret;
		} else {
			if (level) {
				ret = jp_indent(level, buf, rem);
				if (!ret)
					return 0;
				buf += ret;
				rem -= ret;
			}

			ret = snprintf(buf, rem, "\"%s\": []",
				       node->name);
			if (!ret || ret > rem)
				return 0;
			buf += ret;
			rem -= ret;
		}
	}
	return max_size - rem;
}

size_t json_print(struct iconfig_node *node, char *buf, size_t max_size)
{
	return jp_node(node, 0, buf, max_size);
}
