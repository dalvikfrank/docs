LOCAL_PATH:= $(call my-dir)

ifdef NDK_ROOT
include $(call all-makefiles-under, $(LOCAL_PATH))
endif

