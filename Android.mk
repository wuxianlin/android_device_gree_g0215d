#
# Copyright (C) 2016 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This contains the module build definitions for the hardware-specific
# components for this device.
#
# As much as possible, those components should be built unconditionally,
# with device-specific names to avoid collisions, to avoid device-specific
# bitrot and build breakages. Building a component unconditionally does
# *not* include it on all devices, so it is safe even with hardware-specific
# components.

LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),g0215d)

include $(call all-makefiles-under,$(LOCAL_PATH))

IMS_LIBS := libimscamera_jni.so libimsmedia_jni.so

IMS_SYMLINKS := $(addprefix $(TARGET_OUT)/app/ims/lib/arm64/,$(notdir $(IMS_LIBS)))
$(IMS_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "IMS lib link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/lib64/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(IMS_SYMLINKS)

$(shell mkdir -p $(TARGET_OUT)/etc/firmware; \
    ln -sf /dev/block/bootdevice/by-name/msadp \
	    $(TARGET_OUT)/etc/firmware/msadp)

# Create links for audcal data files
$(shell mkdir -p $(TARGET_OUT)/etc/firmware/wcd9320; \
	ln -sf /data/misc/audio/wcd9320_anc.bin \
		$(TARGET_OUT)/etc/firmware/wcd9320/wcd9320_anc.bin;\
	ln -sf /data/misc/audio/mbhc.bin \
		$(TARGET_OUT)/etc/firmware/wcd9320/wcd9320_mbhc.bin; \
	ln -sf /data/misc/audio/wcd9320_mad_audio.bin \
		$(TARGET_OUT)/etc/firmware/wcd9320/wcd9320_mad_audio.bin)

# Create a link for the WCNSS config file, which ends up as a writable
# version in /data/misc/wifi
$(shell mkdir -p $(TARGET_OUT)/etc/firmware/wlan/qca_cld; \
    ln -sf /system/etc/wifi/WCNSS_qcom_cfg.ini \
	    $(TARGET_OUT)/etc/firmware/wlan/qca_cld/WCNSS_qcom_cfg.ini; \
    ln -sf /persist/wlan_mac.bin \
	    $(TARGET_OUT_ETC)/firmware/wlan/qca_cld/wlan_mac.bin)

# wlan modules symlink
$(shell mkdir -p $(TARGET_OUT)/lib/modules; \
    ln -sf /system/lib/modules/qca_cld/qca_cld_wlan.ko \
        $(TARGET_OUT)/lib/modules/wlan.ko)

include device/gree/g0215d/tftp.mk

include device/gree/g0215d/kernel/AndroidKernel.mk

endif
