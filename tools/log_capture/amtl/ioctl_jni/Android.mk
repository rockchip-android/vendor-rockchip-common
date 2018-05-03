LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libioctl_jni

LOCAL_SRC_FILES := Ioctl.c
LOCAL_CFLAGS := -Werror

ifneq (,$(findstring $(TARGET_BOARD_PLATFORM),sofia_lte sflte_2))
    LOCAL_CFLAGS += -DSOFIA_PLATFORM
endif

LOCAL_SHARED_LIBRARIES := libcutils \
        libutils \
        liblog

LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
