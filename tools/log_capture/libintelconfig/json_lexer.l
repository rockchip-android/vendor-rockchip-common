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
    #include<stdio.h>
    #include<string.h> /*strdup*/
    #include <inttypes.h>
    #include "intelconfig_internal.h"
    #include "json_parser.h"
    #include "util.h"
%}

%option noyywrap noinput nounput
%option prefix="json_"
/*%option debug*/

DEC_DIGIT [0-9]
DEC_NUMBER -?{DEC_DIGIT}+

HEXDIGIT [0-9a-fA-F]
HEX_NUMBER 0[xX]{HEXDIGIT}+

BOOL_Y [Tt][Rr][Uu][Ee]|[Yy][Ee][Ss]
BOOL_N [Ff][Aa][Ll][Ss][Ee]|[Nn][Oo]

ESCAPE_UCODE \\u{HEXDIGIT}{HEXDIGIT}{HEXDIGIT}{HEXDIGIT}
ESCAPE_SEQ \\[nrtfb\/\\\"]|{ESCAPE_UCODE}
SIMPLE_CHAR [ -!#-\[\]-~]
CHAR {SIMPLE_CHAR}|{ESCAPE_SEQ}
CHARS {SIMPLE_CHAR}+
ESCAPE_CHARS {CHAR}+
DBL_QUOTE ["]
%%

{DBL_QUOTE}{DBL_QUOTE} |
{DBL_QUOTE}{CHARS}{DBL_QUOTE} {
	json_lval.string = malloc(strlen(yytext)-2+1);
	if (strlen(yytext) > 2)
		memcpy(json_lval.string, yytext+1, strlen(yytext)-2);
	json_lval.string[strlen(yytext)-2] = 0;
	return STRING;
};

{DBL_QUOTE}{ESCAPE_CHARS}{DBL_QUOTE} {
	json_lval.string = unescape_string(yytext);
	return STRING;
};

{DEC_NUMBER} {
	json_lval.integer = strtol(yytext, NULL, 10);
	return INTEGER;
};

{HEX_NUMBER} {
	json_lval.integer = strtol(yytext, NULL, 16);
	return INTEGER;
};

{BOOL_Y} {
	json_lval.integer = 1;
	return INTEGER;
};

{BOOL_N} {
	json_lval.integer = 0;
	return INTEGER;
};

\{ {
	return NODE_START;
};

\} {
	return NODE_END;
};

\[ {
	return ARRAY_START;
};

\] {
	return ARRAY_END;
};

, {
	return COMMA;
};
: {
	return COLON;
};

"#".* /* ignore comments */;
[ \t]+ /* ignore whitespace */;
\n ;

. {
	return 1;
}
%%
