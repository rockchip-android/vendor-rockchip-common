LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libparse_stack
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
    backtrace.c \
    symbols.c \
    symbols_64.c \
    generate_tomb_file.c
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
