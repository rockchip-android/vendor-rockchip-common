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
TESTS_OBJ_DIR = obj-tests
APP_TARGET = log_watch
TESTS_TARGET = log_watch_tests
TARGET_OUT_DIR = out
CFLAGS = -c -g -std=gnu++11 -Wall -I../libintelconfig/incl --coverage
LDFLAGS = -g --coverage
STATIC_LIBS = ../libintelconfig/out/libintelconfig.a
SHARED_LIBS = -lpthread

APP_SRCS = 	EventAttachment.cpp \
		EventRecord.cpp \
		EventWatch.cpp \
		ItemPattern.cpp \
		LogReader.cpp \
		KmsgReader.cpp \
		LwConfig.cpp \
		logwatch.cpp \
		LwLog.cpp \
		LogItem.cpp \
		DataFormat.cpp \
		utils.cpp \
		TimeVal.cpp

TESTS_SRCS = 	tests/host_tests.cpp \
		tests/attachments.cpp \
		tests/datafields.cpp \
		tests/patterns.cpp \
		tests/eventwatch.cpp \
		LwLog.cpp \
		EventAttachment.cpp \
		ItemPattern.cpp \
		EventWatch.cpp \
		EventRecord.cpp \
		LogItem.cpp \
		LwConfig.cpp \
		DataFormat.cpp \
		utils.cpp \
		TimeVal.cpp

RM = rm -fr

# should not change
APP_OBJS = $(APP_SRCS:%.cpp=$(OBJ_DIR)/%.o)
TESTS_OBJS = $(TESTS_SRCS:%.cpp=$(TESTS_OBJ_DIR)/%.o)

all: $(TARGET_OUT_DIR)/$(APP_TARGET) $(TARGET_OUT_DIR)/$(TESTS_TARGET)

$(TARGET_OUT_DIR)/$(APP_TARGET): $(TARGET_OUT_DIR) $(APP_OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET_OUT_DIR)/$(APP_TARGET) $(APP_OBJS) $(SHARED_LIBS) $(STATIC_LIBS)

$(TARGET_OUT_DIR)/$(TESTS_TARGET): $(TARGET_OUT_DIR) $(TESTS_OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET_OUT_DIR)/$(TESTS_TARGET) $(TESTS_OBJS) $(SHARED_LIBS) $(STATIC_LIBS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/tests

$(TESTS_OBJ_DIR):
	mkdir -p $(TESTS_OBJ_DIR)/tests

$(TARGET_OUT_DIR):
	mkdir -p $(TARGET_OUT_DIR)

$(APP_OBJS): $(OBJ_DIR)/%.o: %.cpp $(OBJ_DIR)
	$(CXX) $(CFLAGS) $< -o $@

$(TESTS_OBJS): $(TESTS_OBJ_DIR)/%.o: %.cpp $(TESTS_OBJ_DIR)
	$(CXX) $(CFLAGS) $< -o $@

clean:
	$(RM) $(TARGET_OUT_DIR) $(OBJ_DIR) $(TESTS_OBJ_DIR)

