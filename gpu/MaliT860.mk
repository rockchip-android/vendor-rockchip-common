# $(info 'in MaliT860.mk')
# $(info TARGET_BOARD_PLATFORM_GPU:$(TARGET_BOARD_PLATFORM_GPU) )
# $(info TARGET_ARCH:$(TARGET_ARCH) )

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t860)
# Move to Android.mk
endif

PRODUCT_PACKAGES += \
	libGLES_mali

ifeq ($(strip $(ENABLE_STEREO_DEFORM)), true)
PRODUCT_COPY_FILES += \
	vendor/rockchip/common/gpu/libs/libGLES.so:system/lib/egl/libGLES.so
endif
