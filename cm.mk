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

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from g0215d device
$(call inherit-product, device/gree/g0215d/device.mk)

# Enhanced NFC
$(call inherit-product, vendor/cm/config/nfc_enhanced.mk)

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

PRODUCT_NAME := cm_g0215d
PRODUCT_DEVICE := g0215d
PRODUCT_MANUFACTURER := Gree
PRODUCT_BRAND := Gree
PRODUCT_MODEL := G0215D

PRODUCT_GMS_CLIENTID_BASE := android-gree

TARGET_VENDOR_PRODUCT_NAME := G0215D
TARGET_VENDOR_DEVICE_NAME := G0215D
PRODUCT_BUILD_PROP_OVERRIDES += TARGET_DEVICE=G0215D PRODUCT_NAME=G0215D

PRODUCT_BUILD_PROP_OVERRIDES += \
    BUILD_FINGERPRINT=Android/G0215D/G0215D:6.0.1/MMB29M/fce0db9:user/dev-keys \
    PRIVATE_BUILD_DESC="G0215D-user 6.0.1 MMB29M 6 dev-keys"

TARGET_VENDOR := gree
