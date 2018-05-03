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

#include <string.h>

#include "../incl/intelconfig.h"

#define INT_DEC "\"integer-dec\":-100"
#define INT_HEX "\"integer-hex\":0x01aB"
#define INT_BOOL_Y "\"bool-y\":true"
#define INT_BOOL_N "\"bool-n\":false"
#define STRING_ESCAPED "\"str-esc\": \"\\\" \\\\ \\/ \\b \\f\\n \\r \\t \\u0123\""
#define STRING_UNESCAPED "\"str-unesc\":\"some text\""
#define EMPTY_NODE "\"empty-node\":{}"
#define EMPTY_ARRAY "\"empty-array\":[]"

int json_load_integer(void)
{
	struct iconfig_node *root_node;
	struct iconfig_node *my_node;
	struct iconfig_parse_ctx *ctx;

	ctx = iconfig_alloc_context();
		if (!ctx)
			return 1;
	root_node = iconfig_load(ctx, "{" INT_DEC "}", JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "integer-dec");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_INTEGER)
		goto err;
	if (my_node->v.val.u_vals.integer != -100)
		goto err;
	if (get_child_integer(&root_node->v.val, "integer-dec", 0) != -100)
		goto err;

	iconfig_free(root_node);

	root_node = iconfig_load(ctx, "{" INT_HEX "}", JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "integer-hex");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_INTEGER)
		goto err;
	if (my_node->v.val.u_vals.integer != 0x01aB)
		goto err;
	if (get_child_integer(&root_node->v.val, "integer-hex", 0) != 0x01aB)
		goto err;

	iconfig_free(root_node);

	root_node = iconfig_load(ctx, "{" INT_BOOL_Y "}", JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "bool-y");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_INTEGER)
		goto err;
	if (my_node->v.val.u_vals.integer != 1)
		goto err;
	if (get_child_integer(&root_node->v.val, "bool-y", 0) != 1)
		goto err;

	iconfig_free(root_node);

	root_node = iconfig_load(ctx, "{" INT_BOOL_N "}", JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "bool-n");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_INTEGER)
		goto err;
	if (my_node->v.val.u_vals.integer != 0)
		goto err;
	if (get_child_integer(&root_node->v.val, "bool-n", 1) != 0)
		goto err;

	iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 0;
err:
	iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 1;
}

int json_load_string(void)
{
	struct iconfig_node *root_node;
	struct iconfig_node *my_node;
	struct iconfig_parse_ctx *ctx;
	const char *val;

	ctx = iconfig_alloc_context();
	if (!ctx)
		return 1;
	root_node = iconfig_load(ctx, "{" STRING_ESCAPED "}", JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "str-esc");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_STRING)
		goto err;
	val = get_child_string(&root_node->v.val, "str-esc", "");
	if (strcmp(val, "\" \\ / \b \f\n \r \t \\u0123"))
		goto err;

	iconfig_free(root_node);

	root_node = iconfig_load(ctx, "{" STRING_UNESCAPED "}", JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "str-unesc");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_STRING)
		goto err;
	val = get_child_string(&root_node->v.val, "str-unesc", "");
	if (strcmp(val, "some text"))
		goto err;

	iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 0;
err:
	iconfig_free_context(ctx);
	iconfig_free(root_node);
	return 1;
}

int json_load_nodes(void)
{
	struct iconfig_node *root_node;
	struct iconfig_node *my_node;
	struct iconfig_parse_ctx *ctx;

	ctx = iconfig_alloc_context();
	if (!ctx)
		return 1;
	root_node = iconfig_load(ctx, "{" EMPTY_NODE ", \"other\":{" EMPTY_NODE
				      "," EMPTY_ARRAY "}}", JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "empty-node");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_NODES)
		goto err;
	if (my_node->v.val.u_vals.nodes->count)
		goto err;

	my_node = get_child(&root_node->v.val, "other");
	if (!my_node)
		goto err;
	if (my_node->type != IC_SINGLE)
		goto err;
	if (my_node->v.val.type != ICV_NODES)
		goto err;
	if (my_node->v.val.u_vals.nodes->count != 2)
		goto err;

	iconfig_free(root_node);

	root_node = iconfig_load(ctx, "{}", JSON);

	if (!root_node)
		goto err;
	if (root_node->type != IC_SINGLE)
		goto err;
	if (root_node->v.val.type != ICV_NODES)
		goto err;
	if (root_node->v.val.u_vals.nodes->count)
		goto err;

	iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 0;
err:
	iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 1;
}

int json_load_array(void)
{
	struct iconfig_node *root_node;
	struct iconfig_node *my_node;
	struct iconfig_parse_ctx *ctx;

	ctx = iconfig_alloc_context();
	if (!ctx)
		return 1;
	root_node = iconfig_load(ctx, "{" EMPTY_ARRAY ", \"other\":[0,1]}",
				 JSON);

	if (!root_node)
		goto err;
	my_node = get_child(&root_node->v.val, "empty-array");
	if (!my_node)
		goto err;
	if (my_node->type != IC_ARRAY)
		goto err;
	if (my_node->v.vals.count)
		goto err;

	my_node = get_child(&root_node->v.val, "other");
	if (!my_node)
		goto err;
	if (my_node->type != IC_ARRAY)
		goto err;
	if (my_node->v.vals.count != 2)
		goto err;

	iconfig_free(root_node);

	root_node = iconfig_load(ctx, "[]", JSON);

	if (!root_node)
		goto err;
	if (root_node->type != IC_ARRAY)
		goto err;
	if (root_node->v.vals.count)
		goto err;
	iconfig_free(root_node);

	root_node = iconfig_load(ctx, "[0,1]", JSON);

	if (!root_node)
		goto err;
	if (root_node->type != IC_ARRAY)
		goto err;
	if (root_node->v.vals.count != 2)
		goto err;
	iconfig_free(root_node);
	iconfig_free_context(ctx);

	return 0;
err:
	iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 1;
}

int json_load_error(void)
{
	struct iconfig_node *root_node;
	struct iconfig_parse_ctx *ctx;

	ctx = iconfig_alloc_context();
	if (!ctx)
		return 1;
	root_node = iconfig_load(ctx, "{" EMPTY_NODE ",\n\"other\":{"
				      EMPTY_NODE "\n," EMPTY_ARRAY ","
				      STRING_ESCAPED "}", JSON);

	if (root_node)
		goto err;

	root_node = iconfig_load(ctx, "{ \"name\":4 [2,3,4]}", JSON);

	if (root_node)
		goto err;

	root_node = iconfig_load(ctx, "{ \"name\":4, \"some:[2,3,4] }", JSON);

	if (root_node)
		goto err;

	root_node = iconfig_load(ctx, "{ [2,3,4", JSON);

	if (root_node)
		goto err;
	iconfig_free_context(ctx);
	return 0;
err:
	if (root_node)
		iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 1;
}

int json_print_test(void)
{
	struct iconfig_node *root_node;
	struct iconfig_parse_ctx *ctx;
	char buf[1024];
	int ret;

	ctx = iconfig_alloc_context();
	if (!ctx)
		return 1;
	root_node = iconfig_load(ctx, "{" EMPTY_NODE "," EMPTY_ARRAY ","
				      INT_HEX ",\"intarray\":[1,2,3,4],"
				      STRING_ESCAPED "}", JSON);

	if (!root_node)
		goto err;

	ret = iconfig_store(root_node, JSON, buf, 1024);

	if (!ret)
		goto err;

	/* limit the output buffer*/
	buf[10] = -1;

	ret = iconfig_store(root_node, JSON, buf, 10);
	if (ret)
		goto err;

	if (buf[10] != -1)
		goto err;

	iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 0;
err:
	if (root_node)
		iconfig_free(root_node);
	iconfig_free_context(ctx);
	return 1;
}
