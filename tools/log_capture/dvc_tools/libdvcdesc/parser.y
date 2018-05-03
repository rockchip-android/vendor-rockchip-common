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
    #include"dvcdesc_internal.h"
    #include"desc_parser.h"
    #include"parser.h"
    #include<string.h>
    #include<stdlib.h>
    #include<stdio.h>

    extern int yylex (void);

    void yyerror(struct dvc_descriptors *descs, char const *s);
    int parse_string(const char* str, struct dvc_descriptors *descs);

    struct yy_buffer_state;
    void yy_switch_to_buffer(struct yy_buffer_state* new_buffer);
    struct yy_buffer_state* yy_scan_string(const char* yystr);
%}
%parse-param { struct dvc_descriptors *descs }

%union {
    unsigned long long ull;
    char *str;
    struct num_array *arr;
    struct pair *pair;
    struct pair_list *pair_list;
}

%token <ull> NUMBER
%token <str> STRING
%token <str> ATTR_DESC
%token <str> UNIT_DESC
%token <str> IN_DESC
%token <str> OUT_DESC
%left O_BEGIN O_END A_BEGIN A_END
%left COMMA
%left COLON

%type <pair> PAIR
%type <pair_list> MEMBERS
%type <pair_list> OBJECT
%type <arr> NUMBERELEMENTS
%type <arr> ARRAY

%%

START	: OBJECT
	| MEMBERS
	;

OBJECT	: O_BEGIN MEMBERS O_END { $$ = $2; }

MEMBERS	: PAIR {
		if ($$)
			$$ = new_pair_list($1);
	}
	| PAIR COMMA {
		if ($$)
			$$ = new_pair_list($1);
	}
	| PAIR COMMA MEMBERS {
		if ($$)
			$$ = pair_list_add($3, $1);
	}
	;

PAIR	: ATTR_DESC COLON OBJECT {
		flush_obj(descs, ATTR_DESC, $3);
		$3 = NULL;
		$$ = NULL;
	}
	| UNIT_DESC COLON OBJECT {
		flush_obj(descs, UNIT_DESC, $3);
		$3 = NULL;
		$$ = NULL;
	}
	| IN_DESC COLON OBJECT {
		flush_obj(descs, IN_DESC, $3);
		$3 = NULL;
		$$ = NULL;
	}
	| OUT_DESC COLON OBJECT {
		flush_obj(descs, OUT_DESC, $3);
		$3 = NULL;
		$$ = NULL;
	}
	| STRING COLON NUMBER {
		$$ = new_num_pair($1, $3);
	}
	| STRING COLON STRING {
		$$ = new_str_pair($1, $3);
	}
	| STRING COLON ARRAY {
		$$ = new_arr_pair($1, $3);
	}
	;

ARRAY	: A_BEGIN A_END {
		$$ = new_arr();
	}
	|A_BEGIN NUMBERELEMENTS A_END {
		$$ = $2;
	}
	;

NUMBERELEMENTS: NUMBER {
		$$ = new_arr();
		$$ = arr_add($$, $1);
	}
	| NUMBER COMMA {
		$$ = new_arr();
		$$ = arr_add($$, $1);
	}
	| NUMBER COMMA NUMBERELEMENTS {
		$$ = arr_add($3, $1);
	}
	;

%%

void yyerror(struct dvc_descriptors *descs, char const *s) {
	if (strlen(s) == 0) {
		s = "syntax error";
	}
	printf("Error %s\n", s);
	dvc_free_content(descs);
}

int parse_string(const char* str, struct dvc_descriptors *descs) {
	yy_switch_to_buffer(yy_scan_string(str));
	return yyparse(descs);
}
