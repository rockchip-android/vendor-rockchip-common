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

#include "intelconfig_internal.h"

#include <stdlib.h>
#include <string.h>

#include "incl/intelconfig.h"

int free_list_add(struct free_list_element **first, void *ptr)
{
	struct free_list_element *current;

	current = malloc(sizeof(*current));
	if (!current)
		return 1;
	current->data = ptr;
	if (*first) {
		current->next = (*first);
		*first = current;
	} else {
		current->next = NULL;
		*first = current;
	}
	return 0;
}

int free_list_rem(struct free_list_element **first, void *ptr)
{
	struct free_list_element *current, *prev;

	if (!*first)
		return 1;
	if ((*first)->data == ptr) {
		current = (*first);
		(*first) = current->next;
		free(current);
		return 0;
	}

	for (prev = (*first); prev; prev = prev->next) {
		current = prev->next;
		if (!current)
			break;
		if (current->data == ptr) {
			prev->next = current->next;
			free(current);
			return 0;
		}
	}
	return 0;
}

void parser_error(struct iconfig_parse_ctx *ctx, const char *str)
{
	free(ctx->last_error);
	ctx->last_error = strdup(str);
}

void iconfig_reset_context(struct iconfig_parse_ctx *ctx)
{
	free(ctx->last_error);
	memset(ctx, 0, sizeof(*ctx));
}

void free_nodes(struct iconfig_nodes_array *nodes)
{
	size_t i;

	for (i = 0; i < nodes->count; i++)
		iconfig_free(nodes->data[i]);
	free(nodes->data);
	free(nodes);
}

void free_node_value_content(struct iconfig_node_value *value)
{
	switch (value->type) {
	case ICV_STRING:
		free(value->u_vals.string);
		break;
	case ICV_NODES:
		free_nodes(value->u_vals.nodes);
		break;
	default:
		break;
	}
}

void iconfig_cleanup_context(struct iconfig_parse_ctx *ctx)
{
	struct free_list_element *current;

	while (ctx->strings) {
		current = ctx->strings;
		ctx->strings = current->next;
		free(current->data);
		free(current);
	}

	while (ctx->nodes) {
		current = ctx->nodes;
		ctx->nodes = current->next;
		iconfig_free(current->data);
		free(current);
	}

	while (ctx->values) {
		current = ctx->values;
		ctx->values = current->next;
		free_node_value_content(current->data);
		free(current->data);
		free(current);
	}

	while (ctx->nodes_arrays) {
		current = ctx->nodes_arrays;
		ctx->nodes_arrays = current->next;
		free_nodes(current->data);
		free(current);
	}

	while (ctx->values_arrays) {
		size_t i;
		struct iconfig_node_values_array *arr;

		current = ctx->values_arrays;
		ctx->values_arrays = current->next;
		arr = current->data;
		for (i = 0; i < arr->count; i++)
			free_node_value_content(arr->data[i]);
		free(arr->data);
		free(current->data);
		free(current);
	}
}
