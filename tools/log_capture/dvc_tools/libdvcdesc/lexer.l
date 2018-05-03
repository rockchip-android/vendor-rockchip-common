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
    #include "dvcdesc_internal.h"
    #include "parser.h"
%}

%option noyywrap
/*%option debug*/

DIGIT [0-9a-fA-F]
HEXDIGIT [0-9a-fA-F]
DECNUMBER {DIGIT}|{DIGIT}+
HEXNUMBER 0[xX]{HEXDIGIT}+
CHAR [ -!#-\[\]-~]
CHARS {CHAR}+
DBL_QUOTE ["]

ATTR_DESC attr{CHAR}*
UNIT_DESC unit{CHAR}*
IN_DESC in{CHAR}*
OUT_DESC out{CHAR}*
%%

{DBL_QUOTE}{ATTR_DESC}{DBL_QUOTE} {
	return ATTR_DESC;
};

{DBL_QUOTE}{UNIT_DESC}{DBL_QUOTE} {
	return UNIT_DESC;
};

{DBL_QUOTE}{IN_DESC}{DBL_QUOTE} {
	return IN_DESC;
};

{DBL_QUOTE}{OUT_DESC}{DBL_QUOTE} {
	return OUT_DESC;
};

{DBL_QUOTE}{DBL_QUOTE} |
{DBL_QUOTE}{CHARS}{DBL_QUOTE} {
	yylval.str = malloc(strlen(yytext)-2+1);
	if (strlen(yytext) > 2)
		memcpy(yylval.str, yytext+1, strlen(yytext)-2);
	yylval.str[strlen(yytext)-2] = 0;
	return STRING;
};

{DECNUMBER} {
	yylval.ull = strtoull(yytext, NULL, 10);
	return NUMBER;
};

{HEXNUMBER} {
	yylval.ull = strtoull(yytext, NULL, 16);
	return NUMBER;
};

\{ {
	return O_BEGIN;
};

\} {
	return O_END;
};

\[ {
	return A_BEGIN;
};

\] {
	return A_END;
};

, {
	return COMMA;
};
: {
	return COLON;
};

"#".* /* ignore comments */;
[ \t\n]+ /* ignore whitespace */;

. {
	printf("Unexpected: %c\nExiting...\n",*yytext);
	exit(0);
}
%%
