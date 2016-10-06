/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_youme_imengine_NativeEngine */

#ifndef _Included_com_youme_imengine_NativeEngine
#define _Included_com_youme_imengine_NativeEngine
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setModel
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setModel
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setSysVersion
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setSysVersion
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setDeviceIMEI
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setDeviceIMEI
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setCPUArch
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setCPUArch
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setPackageName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setPackageName
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setDocumentPath
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setDocumentPath
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    onNetWorkChanged
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_onNetWorkChanged
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setBrand
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setBrand
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    setCPUChip
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setCPUChip
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    NotifySpeechFinish
 * Signature: (JIILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_NotifySpeechFinish
  (JNIEnv *, jclass, jlong, jint, jint, jstring, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    Init
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_Init
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    Uninit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_Uninit
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    Login
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_Login
  (JNIEnv *, jclass, jstring, jstring, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    Logout
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_Logout
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    GetContactList
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_GetContactList
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    AddContact
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_AddContact
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    AgreeContactInvited
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_AgreeContactInvited
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    RefuseContactInvited
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_RefuseContactInvited
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    DeleteContact
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_DeleteContact
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    GetContactOfflineMessage
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_GetContactOfflineMessage
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    SendTextMessage
 * Signature: (Ljava/lang/String;ILjava/lang/String;Lcom/youme/imengine/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendTextMessage
  (JNIEnv *, jclass, jstring, jint, jstring, jobject);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    SendCustomMessage
 * Signature: (Ljava/lang/String;ILjava/lang/String;ILcom/youme/imengine/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendCustomMessage
  (JNIEnv *, jclass, jstring, jint, jstring, jint, jobject);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    SendAudioMessage
 * Signature: (Ljava/lang/String;ILcom/youme/imengine/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendAudioMessage
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    SendOnlyAudioMessage
 * Signature: (Ljava/lang/String;ILcom/youme/imengine/IMEngine/MessageRequestId;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendOnlyAudioMessage
  (JNIEnv *, jclass, jstring, jint, jobject);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    StopAudioMessage
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_StopAudioMessage
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    CancleAudioMessage
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_CancleAudioMessage
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    DownloadAudioFile
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_DownloadAudioFile
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    JoinChatRoom
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_JoinChatRoom
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    LeaveChatRoom
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_LeaveChatRoom
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    GetMessage
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_youme_imengine_NativeEngine_GetMessage
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    PopMessage
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_PopMessage
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    GetSDKVer
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_GetSDKVer
  (JNIEnv *, jclass);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    SetServerZone
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_SetServerZone
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    SetMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_SetMode
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    GetFilterText
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_youme_imengine_NativeEngine_GetFilterText
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_youme_imengine_NativeEngine
 * Method:    DestroyFilterText
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_DestroyFilterText
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif