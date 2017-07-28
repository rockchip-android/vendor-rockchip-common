ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali400)
ifeq ($(strip $(TARGET_ARCH)), arm)
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcdc_composer=0
PRODUCT_PROPERTY_OVERRIDES += debug.hwui.render_dirty_regions=false
ifeq ($(strip $(GRAPHIC_MEMORY_PROVIDER)), dma_buf)

ifeq ($(strip $(TARGET_BOARD_PLATFORM)), rk322x)
   ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)), box)
   PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/rk322x_box/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/rk322x_box/libGLES_mali.so:obj/lib/libGLES_mali.so
   
   else
   PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/rk322x/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/rk322x/libGLES_mali.so:obj/lib/libGLES_mali.so

   endif
else ifeq ($(strip $(TARGET_BOARD_PLATFORM)), rk3188)
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/rk3188/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/rk3188/libGLES_mali.so:obj/lib/libGLES_mali.so
else
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/libGLES_mali.so:system/lib/egl/libGLES_mali.so \
    vendor/rockchip/common/gpu/Mali400/lib/$(TARGET_ARCH)/libGLES_mali.so:obj/lib/libGLES_mali.so
endif

ifneq ($(filter rk312x rk3036 rk3188, $(strip $(TARGET_BOARD_PLATFORM))), )
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/Mali400/modules/$(TARGET_ARCH)/$(strip $(TARGET_BOARD_PLATFORM))/mali.ko:system/lib/modules/mali.ko
endif

ifneq ($(filter rk3126c rk3128, $(strip $(TARGET_BOARD_PLATFORM))), )
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/Mali400/modules/$(TARGET_ARCH)/rk312x/mali.ko:system/lib/modules/mali.ko
endif

#PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/gpu_performance/etc/performance_info.xml:system/etc/performance_info.xml \
    vendor/rockchip/common/gpu/gpu_performance/etc/packages-compat.xml:system/etc/packages-compat.xml \
    vendor/rockchip/common/gpu/gpu_performance/etc/packages-composer.xml:system/etc/packages-composer.xml \
    vendor/rockchip/common/gpu/gpu_performance/bin/$(TARGET_ARCH)/performance:system/bin/performance \
    vendor/rockchip/common/gpu/gpu_performance/lib/$(TARGET_ARCH)/libperformance_runtime.so:system/lib/libperformance_runtime.so \
    vendor/rockchip/common/gpu/gpu_performance/lib/$(TARGET_ARCH)/gpu.$(TARGET_BOARD_HARDWARE).so:system/lib/hw/gpu.$(TARGET_BOARD_HARDWARE).so

endif
endif
endif
