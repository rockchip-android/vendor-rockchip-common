#PRODUCT_COPY_FILES += \
#        vendor/rockchip/common/security/optee/optee.ko:system/lib/modules/optee.ko \
#	vendor/rockchip/common/security/optee/optee_armtz.ko:system/lib/modules/optee_armtz.ko	
#ifneq ($(filter rk312x rk3126c, $(strip $(TARGET_BOARD_PLATFORM))), )
ifneq ($(filter tablet, $(strip $(TARGET_BOARD_PLATFORM_PRODUCT))), )
PRODUCT_COPY_FILES += \
	vendor/rockchip/common/security/optee/ta/258be795-f9ca-40e6-a8699ce6886c5d5d.ta:system/lib/optee_armtz/258be795-f9ca-40e6-a8699ce6886c5d5d.ta	\
	vendor/rockchip/common/security/optee/ta/0b82bae5-0cd0-49a5-9521516dba9c43ba.ta:system/lib/optee_armtz/0b82bae5-0cd0-49a5-9521516dba9c43ba.ta	\
	vendor/rockchip/common/security/optee/lib/arm/libkeymaster2.so:system/lib/libkeymaster2.so	\
	vendor/rockchip/common/security/optee/lib/arm/libRkTeeKeymaster.so:system/lib/libRkTeeKeymaster.so	\
	vendor/rockchip/common/security/optee/lib/arm/libkeymaster_messages2.so:system/lib/libkeymaster_messages2.so	\
	vendor/rockchip/common/security/optee/lib/arm/keystore.rk30board.so:system/lib/hw/keystore.rk30board.so	\
	vendor/rockchip/common/security/optee/lib/arm/libRkTeeGatekeeper.so:system/lib/libRkTeeGatekeeper.so	\
	vendor/rockchip/common/security/optee/lib/arm/librkgatekeeper.so:system/lib/librkgatekeeper.so	\
	vendor/rockchip/common/security/optee/lib/arm/gatekeeper.rk30board.so:system/lib/hw/gatekeeper.rk30board.so	
#endif

ifeq ($(strip $(TARGET_ARCH)), arm64)
PRODUCT_COPY_FILES += \
	vendor/rockchip/common/security/optee/lib/arm64/libkeymaster2.so:system/lib64/libkeymaster2.so	\
	vendor/rockchip/common/security/optee/lib/arm64/libRkTeeKeymaster.so:system/lib64/libRkTeeKeymaster.so	\
	vendor/rockchip/common/security/optee/lib/arm64/libkeymaster_messages2.so:system/lib64/libkeymaster_messages2.so	\
	vendor/rockchip/common/security/optee/lib/arm64/keystore.rk30board.so:system/lib64/hw/keystore.rk30board.so	\
	vendor/rockchip/common/security/optee/lib/arm64/libRkTeeGatekeeper.so:system/lib64/libRkTeeGatekeeper.so	\
	vendor/rockchip/common/security/optee/lib/arm64/librkgatekeeper.so:system/lib64/librkgatekeeper.so	\
	vendor/rockchip/common/security/optee/lib/arm64/gatekeeper.rk30board.so:system/lib64/hw/gatekeeper.rk30board.so
endif
endif

ifeq ($(strip $(TARGET_ARCH)), arm64)
PRODUCT_COPY_FILES += \
	vendor/rockchip/common/security/optee/lib/arm64/tee-supplicant:system/bin/tee-supplicant        \
        vendor/rockchip/common/security/optee/lib/arm64/libteec.so:system/lib64/libteec.so
else
PRODUCT_COPY_FILES += \
        vendor/rockchip/common/security/optee/lib/arm/tee-supplicant:system/bin/tee-supplicant	\
	vendor/rockchip/common/security/optee/lib/arm/libteec.so:system/lib/libteec.so
endif

ifeq ($(ENABLE_KEYBOX_PROVISION),true)

PRODUCT_COPY_FILES += \
	vendor/rockchip/common/security/optee/optee_user/rk_widevine/c11fe8ac-b997-48cf-a28de2a55e5240ef.ta:system/lib/optee_armtz/c11fe8ac-b997-48cf-a28de2a55e5240ef.ta	\
	vendor/rockchip/common/security/optee/optee_user/rk_widevine/rk_store_keybox:system/bin/rk_store_keybox
endif
#LOCAL_PATH := $(call my-dir)
#OPTEE_KO_FILES := $(shell ls $(LOCAL_PATH)/*.ko)
#PRODUCT_COPY_FILES += \
#    $(foreach file, $(OPTEE_KO_FILES), $(LOCAL_PATH)/$(file):system/lib/modules/$(file))
