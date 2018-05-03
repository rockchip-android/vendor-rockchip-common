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

#ifndef INTELCONFIG_INTERNAL_H
#define INTELCONFIG_INTERNAL_H

#include "incl/intelconfig.h"

struct free_list_element {
	void *data;
	struct free_list_element *next;
};

struct iconfig_parse_ctx {
	char *last_error;
	struct iconfig_node *root_node;
	struct free_list_element *strings;
	struct free_list_element *nodes;
	struct free_list_element *values;
	struct free_list_element *nodes_arrays;
	struct free_list_element *values_arrays;
};

int free_list_add(struct free_list_element **first, void *ptr);
int free_list_rem(struct free_list_element **first, void *ptr);

void iconfig_reset_context(struct iconfig_parse_ctx *ctx);
void iconfig_cleanup_context(struct iconfig_parse_ctx *ctx);

static inline int
free_list_add_string(struct iconfig_parse_ctx *ctx, char *str)
{
	if (!ctx)
		return 1;
	return free_list_add(&ctx->strings, str);
}

static inline int
free_list_rem_string(struct iconfig_parse_ctx *ctx, char *str)
{
	if (!ctx)
		return 1;
	return free_list_rem(&ctx->strings, str);
}

static inline int
free_list_add_node(struct iconfig_parse_ctx *ctx,
		   struct iconfig_node *node)
{
	if (!ctx)
		return 1;
	return free_list_add(&ctx->nodes, node);
}

static inline int
free_list_rem_node(struct iconfig_parse_ctx *ctx,
		   struct iconfig_node *node)
{
	if (!ctx)
		return 1;
	return free_list_rem(&ctx->nodes, node);
}

static inline int
free_list_add_value(struct iconfig_parse_ctx *ctx,
		    struct iconfig_node_value *node_value)
{
	if (!ctx)
		return 1;
	return free_list_add(&ctx->values, node_value);
}

static inline int
free_list_rem_value(struct iconfig_parse_ctx *ctx,
		    struct iconfig_node_value *node_value)
{
	if (!ctx)
		return 1;
	return free_list_rem(&ctx->values, node_value);
}

static inline int
free_list_add_nodes_array(struct iconfig_parse_ctx *ctx,
			  struct iconfig_nodes_array *nodes_array)
{
	if (!ctx)
		return 1;
	return free_list_add(&ctx->nodes_arrays, nodes_array);
}

static inline int
free_list_rem_nodes_array(struct iconfig_parse_ctx *ctx,
			  struct iconfig_nodes_array *nodes_array)
{
	if (!ctx)
		return 1;
	return free_list_rem(&ctx->nodes_arrays, nodes_array);
}

static inline int
free_list_add_values_array(struct iconfig_parse_ctx *ctx,
			   struct iconfig_node_values_array *node_values_array)
{
	if (!ctx)
		return 1;
	return free_list_add(&ctx->values_arrays, node_values_array);
}

static inline int
free_list_rem_values_array(struct iconfig_parse_ctx *ctx,
			   struct iconfig_node_values_array *node_values_array)
{
	if (!ctx)
		return 1;
	return free_list_rem(&ctx->values_arrays, node_values_array);
}

void parser_error(struct iconfig_parse_ctx *ctx, const char *str);

int json_parse_string(struct iconfig_parse_ctx *ctx, const char *str);
size_t json_print(struct iconfig_node *node, char *buf, size_t max_size);

void free_nodes(struct iconfig_nodes_array *nodes);
void free_node_value_content(struct iconfig_node_value *value);

#endif /*INTELCONFIG_INTERNAL_H */
