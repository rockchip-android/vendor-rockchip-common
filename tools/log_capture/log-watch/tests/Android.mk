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
LOCAL_MODULE := log-watch_tests
LOCAL_MODULE_TAGS := optional tests
LOCAL_MODULE_OWNER := intel
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES = \
    aosp_tests.cpp \
    attachments.cpp \
    datafields.cpp \
    eventwatch.cpp \
    patterns.cpp \
    ../LwLog.cpp \
    ../EventAttachment.cpp \
    ../ItemPattern.cpp \
    ../EventWatch.cpp \
    ../EventRecord.cpp \
    ../LogItem.cpp \
    ../LwConfig.cpp \
    ../DataFormat.cpp \
    ../TimeVal.cpp \
    ../utils.cpp

LOCAL_CPPFLAGS := \
    -std=gnu++11 \
    -W -Wall -Wextra \
    -Wunused \
    -Werror

LOCAL_STATIC_LIBRARIES := libintelconfig

include $(BUILD_NATIVE_TEST)

include $(CLEAR_VARS)
LOCAL_MODULE := log-watch_host_tests
LOCAL_MODULE_TAGS := optional tests
LOCAL_MODULE_OWNER := intel
LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES = \
    aosp_tests.cpp \
    attachments.cpp \
    datafields.cpp \
    eventwatch.cpp \
    patterns.cpp \
    ../LwLog.cpp \
    ../EventAttachment.cpp \
    ../ItemPattern.cpp \
    ../EventWatch.cpp \
    ../EventRecord.cpp \
    ../LogItem.cpp \
    ../LwConfig.cpp \
    ../DataFormat.cpp \
    ../TimeVal.cpp \
    ../utils.cpp

LOCAL_CPPFLAGS := \
    -std=gnu++11 \
    -W -Wall -Wextra \
    -Wunused \
    -Werror

LOCAL_STATIC_LIBRARIES := libintelconfig

include $(BUILD_HOST_NATIVE_TEST)
