ifeq ($(strip $(TARGET_BOARD_PLATFORM)), rk3399)
PRODUCT_PACKAGES += \
	RFTestTool

$(call inherit-product-if-exists, $(LOCAL_PATH)/broadcom/broadcom.mk)
endif
