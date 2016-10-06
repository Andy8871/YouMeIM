LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := YouMeIMEngine
LOCAL_SRC_FILES := ../$(LOCAL_PATH)/../../../../lib/Android/Release/libYouMeIMEngine.so
include $(PREBUILT_SHARED_LIBRARY)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp \
		   ../../Classes/AddContactNotifier.cpp \
		   ../../Classes/ContactListScene.cpp \
		   ../../Classes/MyIMManager.cpp \
		   ../../Classes/ReceiveContactInviteNotifier.cpp \
		   ../../Classes/SoloChatLayer.cpp \
		   ../../Classes/utils.cpp \
		   ../../Classes/YouMeMsgBox.cpp \
		   ../../Classes/YouMeNotifyCenter.cpp \
		   ../../Classes/CursorTextField.cpp \
		   ../../Classes/MyCharSet.cpp \
		   ../../Classes/YoumeNotifier.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes $(LOCAL_PATH)/../../../../src

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_STATIC_LIBRARIES += bugly_crashreport_cocos_static

LOCAL_SHARED_LIBRARIES := YouMeIMEngine

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := Bugly
LOCAL_SRC_FILES := prebuilt/$(TARGET_ARCH_ABI)/libBugly.so
include $(PREBUILT_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,external/bugly)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
