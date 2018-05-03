LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= intel_prop.c

LOCAL_MODULE:= intel_prop
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES := \
	libcutils \
	libc \
	liblog

ifeq ($(INTEL_PROP_LIBDMI),true)
LOCAL_CFLAGS += -DENABLE_DMI
LOCAL_STATIC_LIBRARIES += libdmi
endif

LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
