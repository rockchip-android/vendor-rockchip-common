###############################################################################
# rkmcapp-armeabi-v7a-debug
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MULTILIB := 32
LOCAL_MODULE := rkmcapp-armeabi-v7a-debug
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_TAGS := optional
LOCAL_BUILT_MODULE_STEM := package.apk
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
#LOCAL_PRIVILEGED_MODULE :=
LOCAL_CERTIFICATE := platform
#LOCAL_OVERRIDES_PACKAGES := 
LOCAL_SRC_FILES := $(LOCAL_MODULE).apk
#LOCAL_REQUIRED_MODULES :=
LOCAL_PREBUILT_JNI_LIBS := \
    lib/arm/gdbserver \
	lib/arm/gdb.setup \
	lib/arm/libass.so \
	lib/arm/libbluray.so \
	lib/arm/libcpluff-arm.so \
	lib/arm/libcurl.so \
	lib/arm/libdvdcss-arm.so \
	lib/arm/libdvdnav-arm.so \
	lib/arm/libexif-arm.so \
	lib/arm/libgif.so \
	lib/arm/libImageLib-arm.so \
	lib/arm/lib_imagingft.so \
	lib/arm/lib_imagingmath.so \
	lib/arm/lib_imaging.so \
	lib/arm/libKODI_adsp-arm.so \
	lib/arm/libKODI_audioengine-arm.so \
	lib/arm/libKODI_guilib-arm.so \
	lib/arm/librkmc.so \
	lib/arm/libnfs.so \
	lib/arm/libplist.so \
	lib/arm/libRKCodec-arm.so \
	lib/arm/libRKvcodec_mpi-arm.so \
	lib/arm/librtmp.so \
	lib/arm/libshairplay.so \
	lib/arm/libsmbclient.so \
	lib/arm/libXBMC_addon-arm.so \
	lib/arm/libXBMC_codec-arm.so \
	lib/arm/libXBMC_pvr-arm.so \
	lib/arm/libxbmpeg2convert.so \
	lib/arm/libxbmpeg2.so \
	lib/arm/libxbogg.so \
	lib/arm/libxbvorbisfile.so \
	lib/arm/libxbvorbis.so


include $(BUILD_PREBUILT)
