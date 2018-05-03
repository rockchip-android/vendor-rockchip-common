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

yacc_flags := -x c -std=gnu89

LOCAL_CFLAGS := $(yacc_flags)
LOCAL_MODULE := libdvcdesc
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := intel

LOCAL_SRC_FILES:= lexer.l \
    parser.y \
    dvcdesc.c \
    desc_parser.c

LOCAL_YACCFLAGS := -v
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/incl
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_STATIC_LIBRARY)

