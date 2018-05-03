LOCAL_PATH := $(call my-dir)

cfg_list := \
    amtl_baytrail_XMM_7160.cfg \
    amtl_cherrytrail_XMM_7260.cfg \
    amtl_gmin_XMM_7260.cfg \
    amtl_cherrytrail_XMM_7360.cfg \
    amtl_gmin_XMM_7360.cfg \
    amtl_clovertrail_XMM_6360.cfg \
    amtl_clovertrail_XMM_7160.cfg \
    amtl_merrifield_XMM_6360.cfg \
    amtl_merrifield_XMM_7160.cfg \
    amtl_merrifield_XMM_7260.cfg \
    amtl_moorefield_XMM_6360.cfg \
    amtl_moorefield_XMM_7160.cfg \
    amtl_moorefield_XMM_7260.cfg \
    amtl_moorefield_XMM_7160_M2.cfg \
    amtl_baytrail_XMM_7160_M2.cfg \
    amtl_moorefield_XMM_7260_DSDA.cfg \
    amtl_cherrytrail_XMM_7160_M2.cfg \
    amtl_cherrytrail_XMM_7260_M2.cfg \
    AMTL.conf

define amtl-boilerplate
 include $$(CLEAR_VARS)
 LOCAL_MODULE := $(1)
 LOCAL_MODULE_TAGS := optional
 LOCAL_MODULE_CLASS := ETC
 LOCAL_MODULE_RELATIVE_PATH := telephony
 LOCAL_SRC_FILES := $(1)
 LOCAL_PROPRIETARY_MODULE := true
 include $$(BUILD_PREBUILT)
endef

$(foreach cfg_file,$(cfg_list),$(eval $(call amtl-boilerplate,$(cfg_file))))

include $(CLEAR_VARS)
LOCAL_MODULE := pti_sigusr1
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := script/$(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := amtl_cfg
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES :=\
    $(cfg_list) \
    pti_sigusr1
include $(BUILD_PHONY_PACKAGE)
