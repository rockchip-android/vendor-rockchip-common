LOCAL_PATH := $(call my-dir)

PRODUCT_PACKAGES += \
    libOMX_Core \
    libomxvpu_dec \
    libomxvpu_enc \
    librk_demux \
    librk_hevcdec \
    libRkOMX_Resourcemanager \
    librk_vpuapi

ifeq ($(filter sofia%, $(TARGET_BOARD_PLATFORM)), )
PRODUCT_PACKAGES += \
    libhevcdec \
    librkwmapro \
    librkswscale \
    libjesancache \
    libapedec\
    librk_audio \
    wfd
else
ifeq ($(strip $(USE_INTEL_MDP)), true)
PRODUCT_PACKAGES += \
    libmdp_omx_core \
    libstagefright_soft_aacdec_mdp \
    libstagefright_soft_aacenc_mdp \
    libstagefright_soft_amrdec_mdp \
    libstagefright_soft_amrenc_mdp \
    libstagefright_soft_mp3dec_mdp \
    libstagefright_soft_vorbisdec_mdp
endif
endif

ifeq ($(filter rk2928, $(TARGET_BOARD_PLATFORM)), )
PRODUCT_PACKAGES += \
    libjpeghwdec \
    libjpeghwenc
endif

ifneq ($(filter rk3228 rk3229 rk322x rk3328, $(strip $(TARGET_BOARD_PLATFORM))), )
PRODUCT_COPY_FILES += \
	vendor/rockchip/common/vpu/lib/arm/rk322x/libmpp.so:system/lib/libmpp.so \
	vendor/rockchip/common/vpu/lib/arm/rk322x/libvpu.so:system/lib/libvpu.so \
	vendor/rockchip/common/vpu/lib/arm/rk322x/modules/vcodec_service.ko:system/lib/modules/vcodec_service.ko
PRODUCT_PACKAGES += \
	libiep
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)), rk3399)
PRODUCT_COPY_FILES += \
	vendor/rockchip/common/vpu/lib/arm/rk3399/libjpeghwenc.so:system/lib/libjpeghwenc.so \
	vendor/rockchip/common/vpu/lib/arm/rk3399/libjpeghwdec.so:system/lib/libjpeghwdec.so \
	vendor/rockchip/common/vpu/lib/arm64/rk3399/libjpeghwenc.so:system/lib64/libjpeghwenc.so \
	vendor/rockchip/common/vpu/lib/arm64/rk3399/libjpeghwdec.so:system/lib64/libjpeghwdec.so \
        vendor/rockchip/common/vpu/lib/arm/rk3399/libstagefright_hdcp.so:system/lib/libstagefright_hdcp.so \
        vendor/rockchip/common/vpu/lib/arm64/rk3399/libstagefright_hdcp.so:system/lib64/libstagefright_hdcp.so
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)), rk3366)
PRODUCT_COPY_FILES += \
	vendor/rockchip/common/vpu/lib/arm/rk3366/libjpeghwenc.so:system/lib/libjpeghwenc.so
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)), rk3288)
PRODUCT_COPY_FILES += \
        vendor/rockchip/common/vpu/lib/arm/rk3288/libmpp.so:system/lib/libmpp.so \
        vendor/rockchip/common/vpu/lib/arm/rk3288/libvpu.so:system/lib/libvpu.so \
        vendor/rockchip/common/vpu/lib/arm/rk3288/librk_vpuapi.so:system/lib/librk_vpuapi.so \
	vendor/rockchip/common/vpu/lib/arm/rk3288/libjpeghwenc.so:system/lib/libjpeghwenc.so
endif




ifneq ($(filter rk%, $(TARGET_BOARD_PLATFORM)), )
PRODUCT_PACKAGES += \
    librkffplayer \
    libffmpeg \
    libcsiconv \
    iso
endif

ifneq ($(filter rk3228 rk3288 rk3328 rk3366 rk3399, $(strip $(TARGET_BOARD_PLATFORM))), )
PRODUCT_PACKAGES += \
    libmpp \
    libvpu
endif

ifneq ($(filter rk3328, $(TARGET_BOARD_PLATFORM)), )
PRODUCT_COPY_FILES += \
    vendor/rockchip/common/vpu/firmware/monet.bin:system/etc/firmware/monet.bin \
    vendor/rockchip/common/vpu/lib/arm/libihton.so:system/lib/libihton.so
endif

PRODUCT_COPY_FILES += \
    vendor/rockchip/common/vpu/etc/media_codecs_rk_vpu.xml:system/etc/media_codecs_rk_vpu.xml 
ifneq ($(filter rk312x rk3126c rk3188, $(TARGET_BOARD_PLATFORM)), )
    PRODUCT_COPY_FILES += \
        vendor/rockchip/common/vpu/etc/media_codecs_google_audio_rk312x.xml:system/etc/media_codecs_google_audio.xml \
        vendor/rockchip/common/vpu/etc/media_codecs_google_video_rk312x.xml:system/etc/media_codecs_google_video.xml
else
    PRODUCT_COPY_FILES += \
        vendor/rockchip/common/vpu/etc/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
        vendor/rockchip/common/vpu/etc/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml
endif
ifneq ($(filter rk312x rk3126c rk3188, $(TARGET_BOARD_PLATFORM)), )
    PRODUCT_COPY_FILES += \
        vendor/rockchip/common/vpu/etc/media_codecs_performance_rk312x.xml:system/etc/media_codecs_performance.xml
else
    ifneq ($(filter rk3228 rk3288 rk3366 rk3399, $(TARGET_BOARD_PLATFORM)), )
        PRODUCT_COPY_FILES += \
            vendor/rockchip/common/vpu/etc/media_codecs_performance_mpp.xml:system/etc/media_codecs_performance.xml
    else
        ifneq ($(filter rk3328, $(TARGET_BOARD_PLATFORM)), )
            PRODUCT_COPY_FILES += \
                vendor/rockchip/common/vpu/etc/media_codecs_performance_rk3328.xml:system/etc/media_codecs_performance.xml
        else
            PRODUCT_COPY_FILES += \
    	        vendor/rockchip/common/vpu/etc/media_codecs_performance.xml:system/etc/media_codecs_performance.xml
        endif
    endif
endif
ifneq ($(filter rk312x rk3126c rk3188, $(TARGET_BOARD_PLATFORM)), )
    PRODUCT_COPY_FILES += \
        vendor/rockchip/common/vpu/etc/media_codecs_rk312x.xml:system/etc/media_codecs.xml
else
    ifneq ($(filter rk3328, $(TARGET_BOARD_PLATFORM)), )
        PRODUCT_COPY_FILES += \
            vendor/rockchip/common/vpu/etc/media_codecs_rk3328.xml:system/etc/media_codecs.xml
    else
        ifneq ($(filter rk%, $(TARGET_BOARD_PLATFORM)), )
            PRODUCT_COPY_FILES += \
                vendor/rockchip/common/vpu/etc/media_codecs.xml:system/etc/media_codecs.xml
        else
            PRODUCT_COPY_FILES += \
                vendor/rockchip/common/vpu/etc/media_codecs_sofia.xml:system/etc/media_codecs.xml
        endif
    endif
endif

ifneq ($(filter px5%, $(PRODUCT_BUILD_MODULE)), )
PRODUCT_PACKAGES += \
     libdrivingrecoder
endif
