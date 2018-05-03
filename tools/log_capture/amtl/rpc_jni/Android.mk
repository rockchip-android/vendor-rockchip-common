LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := librpc_jni
LOCAL_SRC_FILES := RPCCall.c
LOCAL_CFLAGS := -Werror

LOCAL_SHARED_LIBRARIES := libcutils \
        libutils \
        liblog

ifneq (,$(findstring $(TARGET_BOARD_PLATFORM),sofia_lte sflte_2))
INC_PATH:=$(LOCAL_PATH)/../../../../../../hardware/imc/rpc

LOCAL_C_INCLUDES += $(INC_PATH)/uta_inc/sf_lte/core \
        $(INC_PATH)/uta_inc/sf_lte/cps \
        $(INC_PATH)/uta_inc/sf_lte/audio_ctrl \
        $(INC_PATH)/uta_inc/sf_lte/audio_mm \
        $(INC_PATH)/uta_inc/sf_lte/battery \
        $(INC_PATH)/uta_inc/sf_lte/gps \
        $(INC_PATH)/uta_inc/sf_lte/i2c \
        $(INC_PATH)/uta_inc/sf_lte/inet \
        $(INC_PATH)/uta_inc/sf_lte/lcs \
        $(INC_PATH)/uta_inc/sf_lte/cls/cts \
        $(INC_PATH)/uta_inc/power \
        $(INC_PATH)/uta_inc/sf_lte/rtc \
        $(INC_PATH)/uta_inc/sf_lte/sock \
        $(INC_PATH)/uta_inc/sf_lte/usb \
        $(INC_PATH)/uta_inc/sf_lte/lp \
        $(INC_PATH)/uta_inc/sf_lte/em \
        $(INC_PATH)/uta_inc/sf_lte/cls \
        $(INC_PATH)/uta_inc/sf_lte/cls/lp \
        $(INC_PATH)/uta_inc/sf_lte/lp \
        $(INC_PATH)/uta_inc/sf_lte/flash_plugin \
        $(INC_PATH)/uta_inc/sf_lte/sys \
        $(INC_PATH)/uta_inc/sf_lte/sensor \
        $(INC_PATH)/uta_inc/sf_lte/trace \
        $(INC_PATH)/uta_inc/sf_lte/nvm \
        $(INC_PATH)/uta_inc/sf_lte/metrics \
        $(INC_PATH)/uta_inc/sf_lte/idc \
        $(INC_PATH)/uta_inc/sf_lte/ims \
        $(INC_PATH)/uta_inc/sf_lte/cls/cp

LOCAL_C_INCLUDES += $(INC_PATH)/multi-client/rpc-stubs/trace/sf_lte \
        $(INC_PATH)/multi-client/utils/osa/android \
        $(INC_PATH)/multi-client/utils/osa \
        $(INC_PATH)/multi-client/utils/

LOCAL_WHOLE_STATIC_LIBRARIES += librpc_tracestub

LOCAL_CFLAGS += -DSOFIA_PLATFORM

endif

LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
ifneq (,$(findstring $(TARGET_BOARD_PLATFORM),sofia_lte sflte_2))
    include $(INC_PATH)/multi-client/rpc-stubs/trace/rpc-trace.mk
endif
