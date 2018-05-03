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

#ifndef INTELCONFIG_H
#define INTELCONFIG_H

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup API Intel Config library API
 * @addtogroup API
 * @{
*/

/**
 * @brief The content type of a value
 */
enum iconfig_node_value_type {
	ICV_NODES,	/**< Contains child nodes */
	ICV_STRING,	/**< Contains an ASCII string */
	ICV_INTEGER,	/**< Contains contains an integer or boolean value */
};

/**
 * @brief The content type of a node
 */
enum iconfig_node_type {
	IC_SINGLE,	/**< A single value */
	IC_ARRAY,	/**< An array of 0, 1 or more values */
};

struct iconfig_node;

/**
 * Collection of nodes
 */
struct iconfig_nodes_array {
	size_t count;			/**< Number of entries */
	struct iconfig_node **data;	/**< Entries array */
};

struct iconfig_node_value;

/**
 * Collection of nodes values
 */
struct iconfig_node_values_array {
	size_t count;				/**< Number of entries */
	struct iconfig_node_value **data;	/**< Entries array */
};

/**
 * Node value type
 */
struct iconfig_node_value {
	enum iconfig_node_value_type type;	/**< Type */
	union {
		struct iconfig_nodes_array *nodes;
		char *string;
		long integer;
	} u_vals;				/**< Actual value[s] */
};

/**
 * Node definition
 */
struct iconfig_node {
	enum iconfig_node_type type;	/**< The content type */
	union {
		struct iconfig_node_value val;
		struct iconfig_node_values_array vals;
	} v;				/**< The content union */
	char *name;			/**< The name*/
};

/**
 * @brief Type of input/output formats
 */
enum config_type {
	JSON /**< json */
};

/**
 * @brief Holds an internal parsing context,
 *
 * Could be used to retrieve the last error message.
 */
struct iconfig_parse_ctx;

struct iconfig_parse_ctx *iconfig_alloc_context();
void iconfig_free_context(struct iconfig_parse_ctx *ctx);
const char *iconfig_get_last_error(struct iconfig_parse_ctx *ctx);

void iconfig_free(struct iconfig_node *node);

struct iconfig_node *iconfig_load(struct iconfig_parse_ctx *ctx,
				  const char *src, enum config_type type);
size_t iconfig_store(struct iconfig_node *node, enum config_type type,
		     char *buf, size_t max_size);

struct iconfig_node *get_child(struct iconfig_node_value *value,
			       const char *name);
const char *get_child_string(struct iconfig_node_value *value,
			     const char *name, const char *default_val);
long get_child_integer(struct iconfig_node_value *value, const char *name,
		       long default_val);

struct iconfig_node_value *new_nodes_value(struct iconfig_nodes_array *nodes);
struct iconfig_node_value *new_integer_value(long integer);
struct iconfig_node_value *new_string_value(char *string);

struct iconfig_nodes_array *new_nodes_array();
struct iconfig_nodes_array *nodes_array_add(struct iconfig_nodes_array *arr,
					    struct iconfig_node *node);
struct iconfig_node_values_array *new_values_array();
struct iconfig_node_values_array
*values_array_add(struct iconfig_node_values_array *arr,
		  struct iconfig_node_value *node);

struct iconfig_node *new_array_node(char *name,
				    struct iconfig_node_values_array *nodes);
struct iconfig_node *new_node(char *name, struct iconfig_node_value *nv);

#ifdef __cplusplus
}
#endif

/**
 *     @}
 */
#endif /*INTELCONFIG_H*/
