ifneq ($(strip $(TARGET_2ND_ARCH)), )
# etc
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_64/powervr.ini:system/etc/powervr.ini

# lib
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_64/lib/egl/egl.cfg:system/lib/egl/egl.cfg \
    vendor/rockchip/common/gpu/libG6110/G6110_64/lib/modules/pvrsrvkm.ko:system/lib/modules/pvrsrvkm.ko

# vendor/firmware
PRODUCT_COPY_FILES += \
   vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/firmware/rgx.fw.signed.5.9.1.46:system/vendor/firmware/rgx.fw.signed.5.9.1.46

# vendor/bin
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/bin/pvrsrvctl:system/vendor/bin/pvrsrvctl \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/bin/pvrtld:system/vendor/bin/pvrtld

# vendor/lib
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/egl/libEGL_POWERVR_ROGUE.so:system/vendor/lib/egl/libEGL_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/egl/libGLESv1_CM_POWERVR_ROGUE.so:system/vendor/lib/egl/libGLESv1_CM_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/egl/libGLESv2_POWERVR_ROGUE.so:system/vendor/lib/egl/libGLESv2_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/hw/gralloc.rk3368.so:system/vendor/lib/hw/gralloc.rk3368.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/hw/vulkan.rk3368.so:system/vendor/lib/hw/vulkan.rk3368.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/hw/memtrack.rk3368.so:system/vendor/lib/hw/memtrack.rk3368.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libcreatesurface.so:system/vendor/lib/libcreatesurface.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libglslcompiler.so:system/vendor/lib/libglslcompiler.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libIMGegl.so:system/vendor/lib/libIMGegl.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libpvrANDROID_WSEGL.so:system/vendor/lib/libpvrANDROID_WSEGL.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libPVRScopeServices.so:system/vendor/lib/libPVRScopeServices.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libsrv_um.so:system/vendor/lib/libsrv_um.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libusc.so:system/vendor/lib/libusc.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libPVROCL.so:system/vendor/lib/libPVROCL.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/liboclcompiler.so:system/vendor/lib/liboclcompiler.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libufwriter.so:system/vendor/lib/libufwriter.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libLLVMIMG.so:system/vendor/lib/libLLVMIMG.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib/libclangIMG.so:system/vendor/lib/libclangIMG.so

# vendor/lib64
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/egl/libEGL_POWERVR_ROGUE.so:system/vendor/lib64/egl/libEGL_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/egl/libGLESv1_CM_POWERVR_ROGUE.so:system/vendor/lib64/egl/libGLESv1_CM_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/egl/libGLESv2_POWERVR_ROGUE.so:system/vendor/lib64/egl/libGLESv2_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/hw/gralloc.rk3368.so:system/vendor/lib64/hw/gralloc.rk3368.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/hw/vulkan.rk3368.so:system/vendor/lib64/hw/vulkan.rk3368.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libcreatesurface.so:system/vendor/lib64/libcreatesurface.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libglslcompiler.so:system/vendor/lib64/libglslcompiler.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libIMGegl.so:system/vendor/lib64/libIMGegl.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libpvrANDROID_WSEGL.so:system/vendor/lib64/libpvrANDROID_WSEGL.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libPVRScopeServices.so:system/vendor/lib64/libPVRScopeServices.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libsrv_init.so:system/vendor/lib64/libsrv_init.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libsrv_um.so:system/vendor/lib64/libsrv_um.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libusc.so:system/vendor/lib64/libusc.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libPVROCL.so:system/vendor/lib64/libPVROCL.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/liboclcompiler.so:system/vendor/lib64/liboclcompiler.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libufwriter.so:system/vendor/lib64/libufwriter.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libsutu_display.so:system/vendor/lib64/libsutu_display.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libLLVMIMG.so:system/vendor/lib64/libLLVMIMG.so \
    vendor/rockchip/common/gpu/libG6110/G6110_64/vendor/lib64/libclangIMG.so:system/vendor/lib64/libclangIMG.so

else
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_32/lib/egl/egl.cfg:system/lib/egl/egl.cfg \
    vendor/rockchip/common/gpu/libG6110/G6110_32/lib/modules/pvrsrvkm.ko:system/lib/modules/pvrsrvkm.ko

# vendor/bin
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/bin/pvrsrvctl:system/vendor/bin/pvrsrvctl \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/bin/pvrtld:system/vendor/bin/pvrtld

# vendor/lib
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/egl/libEGL_POWERVR_ROGUE.so:system/vendor/lib/egl/libEGL_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/egl/libGLESv1_CM_POWERVR_ROGUE.so:system/vendor/lib/egl/libGLESv1_CM_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/egl/libGLESv2_POWERVR_ROGUE.so:system/vendor/lib/egl/libGLESv2_POWERVR_ROGUE.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/hw/gralloc.rk3368.so:system/vendor/lib/hw/gralloc.rk3368.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libcreatesurface.so:system/vendor/lib/libcreatesurface.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libglslcompiler.so:system/vendor/lib/libglslcompiler.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libIMGegl.so:system/vendor/lib/libIMGegl.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libpvrANDROID_WSEGL.so:system/vendor/lib/libpvrANDROID_WSEGL.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libsrv_init.so:system/vendor/lib/libsrv_init.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libsrv_um.so:system/vendor/lib/libsrv_um.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libusc.so:system/vendor/lib/libusc.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libPVROCL.so:system/vendor/lib/libPVROCL.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/liboclcompiler.so:system/vendor/lib/liboclcompiler.so \
    vendor/rockchip/common/gpu/libG6110/G6110_32/vendor/lib/libufwriter.so:system/vendor/lib/libufwriter.so
endif
