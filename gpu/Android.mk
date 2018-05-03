LOCAL_PATH := $(call my-dir)
# $(info 'in MaliT860.mk')
# $(info TARGET_BOARD_PLATFORM_GPU:$(TARGET_BOARD_PLATFORM_GPU) )
# $(info TARGET_ARCH:$(TARGET_ARCH) )

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t860)
include $(CLEAR_VARS)
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE := libGLES_mali
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MULTILIB := both
LOCAL_SRC_FILES_$(TARGET_ARCH) := MaliT860/lib/$(TARGET_ARCH)/libGLES_mali.so
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := MaliT860/lib/$(TARGET_2ND_ARCH)/libGLES_mali.so
LOCAL_MODULE_PATH_32 := $(TARGET_OUT_VENDOR)/lib/egl
LOCAL_MODULE_PATH_64 := $(TARGET_OUT_VENDOR)/lib64/egl

# Create symlinks.
LOCAL_POST_INSTALL_CMD := \
	if [ -f $(LOCAL_MODULE_PATH_32)/libGLES_mali.so ];then cd $(TARGET_OUT_VENDOR)/lib; ln -sf egl/libGLES_mali.so libGLES_mali.so; cd -; fi; \
	if [ -f $(LOCAL_MODULE_PATH_64)/libGLES_mali.so ];then cd $(TARGET_OUT_VENDOR)/lib64; ln -sf egl/libGLES_mali.so libGLES_mali.so; cd -; fi;

include $(BUILD_PREBUILT)
endif

include $(LOCAL_PATH)/gpu_performance/Android.mk
