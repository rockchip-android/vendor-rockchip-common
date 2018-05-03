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

#include "incl/intelconfig.h"

#include <string.h>

#include "intelconfig_internal.h"

/**
 * @addtogroup API
 * @{
*/

/**
 * @param src A NULL terminated string contain the config representation
 * @param type The format use
 * @return NULL if it fails
 */
struct iconfig_node *iconfig_load(struct iconfig_parse_ctx *ctx,
				  const char *src, enum config_type type)
{
	switch (type) {
	case JSON:
		iconfig_reset_context(ctx);
		if (json_parse_string(ctx, src))
			iconfig_cleanup_context(ctx);
		return ctx->root_node;
	}
	return NULL;
}

/**
 * @param node[in] The root node
 * @param type[in] The format used
 * @param buf[out] The output buffer
 * @param max_size[in] Maximum output size
 * @return The data length or 0 if it fails
 */
size_t iconfig_store(struct iconfig_node *node, enum config_type type,
		     char *buf, size_t max_size)
{
	/*keep space for \0*/
	max_size--;
	switch (type) {
	case JSON:
		return json_print(node, buf, max_size);
	}
	return 0;
}

/**
 * Frees the content of a node, including child nodes
 * @param node
 */
void iconfig_free(struct iconfig_node *node)
{
	if (node->type == IC_SINGLE) {
		free_node_value_content(&node->v.val);
	} else {
		size_t i;

		for (i = 0; i < node->v.vals.count; i++) {
			free_node_value_content(node->v.vals.data[i]);
			free(node->v.vals.data[i]);
		}
		free(node->v.vals.data);
	}

	free(node->name);
	free(node);
}

/**
 * @brief Find the first child node named \a name in the given \a value
 * @param value where
 * @param name what
 * @return NULL on failure
 */
struct iconfig_node *get_child(struct iconfig_node_value *value,
			       const char *name)
{
	size_t i;

	if (value->type != ICV_NODES)
		return NULL;
	for (i = 0; i < value->u_vals.nodes->count; i++)
		if (!strcmp(value->u_vals.nodes->data[i]->name, name))
			return value->u_vals.nodes->data[i];
	return NULL;
}

/**
 * @brief Find the first string child node named \a name in the given \a value
 * @param value where
 * @param name what
 * @param default_val Default return value
 * @return \a default_val if it fails
 */
const char *get_child_string(struct iconfig_node_value *value, const char *name,
			     const char *default_val)
{
	struct iconfig_node *node = get_child(value, name);

	if (node && node->type == IC_SINGLE && node->v.val.type == ICV_STRING)
		return node->v.val.u_vals.string;
	return default_val;
}

/**
 * @brief Find the first integer child node named \a name in the given \a value
 * @param value where
 * @param name what
 * @param default_val Default return value
 * @return \a default_val if it fails
 */
long get_child_integer(struct iconfig_node_value *value, const char *name,
		       long default_val)
{
	struct iconfig_node *node = get_child(value, name);

	if (node && node->type == IC_SINGLE && node->v.val.type == ICV_INTEGER)
		return node->v.val.u_vals.integer;
	return default_val;
}

/**
 * @brief Crate an empty nods array (eg. JSON "{}")
 * @return NULL on failure
 */
struct iconfig_nodes_array *new_nodes_array()
{
	struct iconfig_nodes_array *ret;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;

	ret->count = 0;
	ret->data = NULL;
	return ret;
}

/**
 * @brief Add node to array arr.
 * @param arr
 * @param node
 * @return NULL on failure, arr otherwise.
 */
struct iconfig_nodes_array *nodes_array_add(struct iconfig_nodes_array *arr,
					    struct iconfig_node *node)
{
	struct iconfig_node **new_data =
	    malloc(sizeof(*arr->data) * (arr->count + 1));

	if (!new_data) {
		free(arr->data);
		free(arr);
		return NULL;
	}

	memcpy(&new_data[1], arr->data, sizeof(*arr->data) * arr->count);
	new_data[0] = node;
	arr->count++;
	free(arr->data);
	arr->data = new_data;
	return arr;
}

/**
 * @brief Crate a new empty values array (eg. JSON "[]")
 * @return  NULL on failure
 */
struct iconfig_node_values_array *new_values_array()
{
	struct iconfig_node_values_array *ret;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;

	ret->count = 0;
	ret->data = NULL;
	return ret;
}

/**
 * @brief Add value to array arr.
 * @param arr
 * @param value
 * @return NULL on failure, arr otherwise.
 */
struct iconfig_node_values_array
*values_array_add(struct iconfig_node_values_array *arr,
		  struct iconfig_node_value *value)
{
	struct iconfig_node_value **new_data =
	    malloc(sizeof(*arr->data) * (arr->count + 1));

	if (!new_data) {
		free(arr->data);
		free(arr);
		return NULL;
	}

	memcpy(&new_data[1], arr->data, sizeof(*arr->data) * arr->count);
	new_data[0] = value;
	arr->count++;
	free(arr->data);
	arr->data = new_data;
	return arr;
}

/**
 * @brief Create a new value holding nodes.
 * @param nodes
 * @return NULL on failure
 */
struct iconfig_node_value *new_nodes_value(struct iconfig_nodes_array *nodes)
{
	struct iconfig_node_value *ret = malloc(sizeof(*ret));

	if (!ret)
		return NULL;

	ret->type = ICV_NODES;
	ret->u_vals.nodes = nodes;

	return ret;
}

/**
 * @brief Create a new value holding integer.
 * @param nodes
 * @return NULL on failure
 */
struct iconfig_node_value *new_integer_value(long integer)
{
	struct iconfig_node_value *ret = malloc(sizeof(*ret));

	if (!ret)
		return NULL;

	ret->type = ICV_INTEGER;
	ret->u_vals.integer = integer;

	return ret;
}

/**
 * @brief Create a new value holding string.
 *
 * NOTE: string will be freed when the value is freed.
 * @param string
 * @return NULL on failure.
 */
struct iconfig_node_value *new_string_value(char *string)
{
	struct iconfig_node_value *ret = malloc(sizeof(*ret));

	if (!ret)
		return NULL;

	ret->type = ICV_STRING;
	ret->u_vals.string = string;
	return ret;
}

/**
 * @brief Crates a new patent node for nv
 * @param name Should be dynamically allocated, it will be freed by iconfig_free
 * @param nv
 * @return NULL on failure.
 */
struct iconfig_node *new_node(char *name, struct iconfig_node_value *nv)
{
	struct iconfig_node *ret;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;

	ret->type = IC_SINGLE;
	ret->v.val.type = nv->type;
	ret->v.val.u_vals = nv->u_vals;
	ret->name = name;
	free(nv);

	return ret;
}

/**
 * @brief Create a new node holding values.
 * @param name
 * @param values
 * @return NULL on failure.
 */
struct iconfig_node *new_array_node(char *name,
				    struct iconfig_node_values_array *values)
{
	struct iconfig_node *ret;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;

	ret->type = IC_ARRAY;
	ret->v.vals.count = values->count;
	ret->v.vals.data = values->data;
	ret->name = name;

	free(values);

	return ret;
}

/**
 * @brief Allocates a new parsing context
 * @return
 */
struct iconfig_parse_ctx *iconfig_alloc_context()
{
	struct iconfig_parse_ctx *ret;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;
	memset(ret, 0, sizeof(*ret));
	return ret;
}

/**
 * @brief Destroys a parsing context
 * @param ctx
 */
void iconfig_free_context(struct iconfig_parse_ctx *ctx)
{
	free(ctx->last_error);
	free(ctx);
}

/**
 * @brief Get the last parsing error message
 * @param ctx
 * @return
 */
const char *iconfig_get_last_error(struct iconfig_parse_ctx *ctx)
{
	return ctx->last_error;
}

/**
 *     @}
 */
