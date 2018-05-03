/* Copyright (C) Intel 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

%{
    #include "intelconfig_internal.h"
    #include "json_parser.h"
    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>

    extern int json_lex ();

    void json_error(struct iconfig_parse_ctx *ctx, char const *s);
    int json_parse_string(struct iconfig_parse_ctx *ctx, const char* str);

    struct json__buffer_state;
    void json__switch_to_buffer(struct json__buffer_state* new_buffer);
    void json__delete_buffer (struct json__buffer_state* b  );
    struct json__buffer_state* json__scan_string(const char* yystr);
%}
%parse-param { struct iconfig_parse_ctx *ctx }
%name-prefix "json_"

%union {
    long integer;
    char *string;
    struct iconfig_node *node;
    struct iconfig_node_value *node_value;
    struct iconfig_nodes_array *nodes_array;
    struct iconfig_node_values_array *node_values_array;
}

%token <integer> INTEGER
%token <string> STRING
%left NODE_START NODE_END ARRAY_START ARRAY_END
%left COMMA
%left COLON

%type <node> 			NODE
%type <nodes_array>		NODES
%type <node_value>		VALUE
%type <node_values_array>	VALUES
%type <node_values_array>	VALUES_ARRAY
%type <string>	STRING_ALLOC
%%

START	: NODE {
		if (ctx->root_node)
			iconfig_free(ctx->root_node);

		ctx->root_node = $1;

		free_list_rem_node(ctx, $1);
	}
	| NODES {
		struct iconfig_node_value *nv = new_nodes_value($1);
		char *name = strdup("iconfig-root");
		if (ctx->root_node)
			iconfig_free(ctx->root_node);

		ctx->root_node = new_node(name, nv);

		free_list_rem_nodes_array(ctx, $1);
	}
	| VALUE {
		char *name = strdup("iconfig-root");
		if (ctx->root_node)
			iconfig_free(ctx->root_node);

		ctx->root_node = new_node(name,$1);

		free_list_rem_value(ctx, $1);
	}
	| VALUES {
		char *name = strdup("iconfig-root");
		if (ctx->root_node)
			iconfig_free(ctx->root_node);

		ctx->root_node = new_array_node(name, $1);

		free_list_rem_values_array(ctx, $1);
	}
	| VALUES_ARRAY {
		char *name = strdup("iconfig-root");
		if (ctx->root_node)
			iconfig_free(ctx->root_node);

		ctx->root_node = new_array_node(name, $1);

		free_list_rem_values_array(ctx, $1);
	}
	;

STRING_ALLOC: STRING {
		$$ = $1;
		free_list_add_string(ctx, $1);
	}
	;

VALUE	: NODE_START NODES NODE_END {
		$$ = new_nodes_value($2);

		free_list_add_value(ctx, $$);
		free_list_rem_nodes_array(ctx, $2);
	}
	| NODE_START NODE NODE_END {
		struct iconfig_nodes_array *nva = new_nodes_array();
		if (!nva)
			abort();
		nva = nodes_array_add(nva, $2);
		$$ = new_nodes_value(nva);

		free_list_rem_node(ctx, $2);
		free_list_add_value(ctx, $$);
	}
	| NODE_START NODE_END {
		struct iconfig_nodes_array *nva = new_nodes_array();
		$$ = new_nodes_value(nva);

		free_list_add_value(ctx, $$);
	}
	| STRING_ALLOC {
		$$ = new_string_value($1);
		free_list_rem_string(ctx, $1);
		free_list_add_value(ctx, $$);
	}
	| INTEGER {
		$$ = new_integer_value($1);
		free_list_add_value(ctx, $$);
	}
	| VALUE COMMA {
		$$ = $1;
	}
	;

VALUES : VALUE COMMA VALUE{
		$$ = new_values_array();
		$$ = values_array_add($$, $3);
		$$ = values_array_add($$, $1);

		free_list_rem_value(ctx, $1);
		free_list_rem_value(ctx, $3);
		free_list_add_values_array(ctx, $$);
	}
	| VALUE COMMA VALUES {
		$$ = values_array_add($3, $1);

		free_list_rem_value(ctx, $1);
	}
	;

VALUES_ARRAY : ARRAY_START VALUES ARRAY_END {
		$$ = $2;
	}
	| ARRAY_START VALUE ARRAY_END
	{
		$$ = new_values_array();
		$$ = values_array_add($$, $2);

		free_list_rem_value(ctx, $2);
		free_list_add_values_array(ctx, $$);
	}
	| ARRAY_START ARRAY_END
	{
		$$ = new_values_array();
		free_list_add_values_array(ctx, $$);
	}
	;

NODE	: STRING_ALLOC COLON VALUE {
		$$ = new_node($1,$3);
		free_list_rem_string(ctx, $1);
		free_list_rem_value(ctx, $3);

		free_list_add_node(ctx, $$);
	}
	| STRING_ALLOC COLON VALUES_ARRAY {
		$$ = new_array_node($1,$3);

		free_list_rem_string(ctx, $1);
		free_list_rem_values_array(ctx, $3);
		free_list_add_node(ctx, $$);
	}
	| NODE COMMA {
		$$ = $1;
	}
	;

NODES	: NODE COMMA NODE{
		$$ = new_nodes_array();
		$$ = nodes_array_add($$, $3);
		$$ = nodes_array_add($$, $1);

		free_list_rem_node(ctx, $1);
		free_list_rem_node(ctx, $3);
		free_list_add_nodes_array(ctx, $$);
	}
	| NODE COMMA NODES {
		$$ = nodes_array_add($3, $1);
		free_list_rem_node(ctx, $1);
	}
	;
%%

void json_error(struct iconfig_parse_ctx *ctx, char const *s)
{
	if (strlen(s) == 0) {
		s = "Syntax error";
	}
	parser_error(ctx, s);
}

int json_parse_string(struct iconfig_parse_ctx *ctx, const char* str)
{
	int ret;
	struct json__buffer_state *b = json__scan_string(str);
	json__switch_to_buffer(b);
	ctx->root_node = NULL;
	ret = json_parse(ctx);
	json__delete_buffer(b);
	return ret;
}
