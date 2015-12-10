LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := libraknet

FILE_LIST := $(wildcard $(LOCAL_PATH)/../../Source/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

include $(BUILD_SHARED_LIBRARY)