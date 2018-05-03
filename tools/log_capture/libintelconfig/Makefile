#
# Copyright (C) Intel 2015
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

OBJ_DIR = obj
GEN_DIR = gen
LIB_TARGET = libintelconfig.a
APP_TARGET = intelconfig_test
TARGET_OUT_DIR = out
CFLAGS = -c -g -Wall -I$(GEN_DIR) -I. --coverage
ARFLAGS = rcs
LDFLAGS = -g --coverage
YACC = bison
LEX = flex

LIB_SRCS = intelconfig.c intelconfig_internal.c util.c json_print.c
APP_SRCS = tests/host_test.c tests/util.c tests/json.c
GEN_LIB_SRCS = $(GEN_DIR)/json_lexer.c $(GEN_DIR)/json_parser.c

RM = rm -fr

# + should not change
LIB_OBJS = $(LIB_SRCS:%.c=$(OBJ_DIR)/%.o)
APP_OBJS = $(APP_SRCS:%.c=$(OBJ_DIR)/%.o)
GEN_LIB_OBJS = $(GEN_LIB_SRCS:$(GEN_DIR)/%.c=$(OBJ_DIR)/%.o)

all: lib $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET_OUT_DIR)/$(APP_TARGET) $(APP_OBJS) $(TARGET_OUT_DIR)/$(LIB_TARGET)

lib: $(OBJ_DIR) $(GEN_LIB_OBJS) $(LIB_OBJS) $(TARGET_OUT_DIR)
	$(AR) $(ARFLAGS) $(TARGET_OUT_DIR)/$(LIB_TARGET) $(GEN_LIB_OBJS) $(LIB_OBJS)

$(GEN_DIR)/json_lexer.c: $(GEN_DIR)/json_parser.c json_lexer.l
	$(LEX) -o $(GEN_DIR)/json_lexer.c json_lexer.l

$(GEN_DIR)/json_parser.c: $(GEN_DIR) json_parser.y
	$(YACC) -d -v -o $(GEN_DIR)/json_parser.c json_parser.y

$(GEN_DIR):
	mkdir -p $(GEN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/tests

$(TARGET_OUT_DIR):
	mkdir -p $(TARGET_OUT_DIR)

# could be much better
$(LIB_OBJS): $(LIB_SRCS)
	$(foreach src,$(LIB_SRCS),$(CC) $(CFLAGS) $(src) -o $(src:%.c=$(OBJ_DIR)/%.o);)

$(APP_OBJS): $(APP_SRCS)
	$(foreach src,$(APP_SRCS),$(CC) $(CFLAGS) $(src) -o $(src:%.c=$(OBJ_DIR)/%.o);)

$(GEN_LIB_OBJS): $(GEN_LIB_SRCS)
	$(foreach src,$(GEN_LIB_SRCS),$(CC) $(CFLAGS) $(src) -o $(src:$(GEN_DIR)/%.c=$(OBJ_DIR)/%.o);)


clean:
	$(RM) $(TARGET_OUT_DIR)/$(LIB_TARGET) $(OBJ_DIR) *.tab.* *.lex.*
