LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_JAVA_LIBRARIES := com.intel.internal.telephony.ModemClient

LOCAL_SRC_FILES := $(call all-java-files-under, src)
# Only compile source java files in this apk.

LOCAL_PACKAGE_NAME := Amtl

LOCAL_CERTIFICATE := platform
LOCAL_DEX_PREOPT := nostripping

LOCAL_REQUIRED_MODULES := amtl_cfg

LOCAL_REQUIRED_MODULES += \
    libamtl_jni \
    librpc_jni \
    libioctl_jni

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call first-makefiles-under,$(LOCAL_PATH))
