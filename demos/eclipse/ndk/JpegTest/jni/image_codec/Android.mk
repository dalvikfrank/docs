LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libjpeg

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    LOCAL_SRC_FILES := libjpeg/lib/64/libjpeg.so
endif
ifeq ($(TARGET_ARCH_ABI), armeabi)
    LOCAL_SRC_FILES := libjpeg/lib/32/libjpeg.so
endif

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libimage_codec
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libjpeg/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../filters/common

LOCAL_SRC_FILES :=  \
    inputstream_wrapper.cpp \
    outputstream_wrapper.cpp \
    jpeg_utils.cpp \
    jpeg_codec.cpp \
    test_main.cpp

LOCAL_SHARED_LIBRARIES := libjpeg
LOCAL_LDLIBS    := -lm -llog -ljnigraphics
include $(BUILD_SHARED_LIBRARY)
