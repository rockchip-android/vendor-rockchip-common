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

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := log-watch
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := intel

LOCAL_SRC_FILES := \
    DataFormat.cpp \
    EventAttachment.cpp \
    EventRecord.cpp \
    EventWatch.cpp \
    ItemPattern.cpp \
    KmsgReader.cpp \
    LwConfig.cpp \
    logwatch.cpp \
    LwLog.cpp \
    LogItem.cpp \
    LogReader.cpp \
    TimeVal.cpp \
    utils.cpp

LOCAL_CPPFLAGS := \
    -std=gnu++11 \
    -W -Wall -Wextra \
    -Wunused \
    -Werror \
    -DANDROID_TARGET

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3188)
LOCAL_CFLAGS += -DKLOG
endif

LOCAL_SHARED_LIBRARIES := liblct liblog
LOCAL_STATIC_LIBRARIES := libintelconfig
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
