//
//  com_youme_imengine_NativeEngine.cpp
//

#include "com.youme.imengine_NativeEngine.h"
#include <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/NetworkService.h>
#include <YouMeIMEngine/Speech/Android/AndroidSpeechManager.hpp>
#include "../../CInterface/IYouMeCInterface.h"
#include <YouMeCommon/Log.h>
static JavaVM *mJvm = NULL;
static jclass mYouMeManagerClass = NULL;
static jmethodID m_YouMeStartListen = 0;
static jmethodID m_YouMeStartListenOnlyAudio = 0;
static jmethodID m_YouMeStopListen = 0;
static jmethodID m_YouMeCancleListen = 0;
static jmethodID m_YouMePlayAudio = 0;
static jmethodID m_YouMeSetAudioCacheDir = 0;
ISpeechListen* g_AndroidSpeechListen = NULL;

/*
typedef union jvalue {
    jboolean    z;
    jbyte       b;
    jchar       c;
    jshort      s;
    jint        i;
    jlong       j;
    jfloat      f;
    jdouble     d;
    jobject     l;
} jvalue;

*/
class JNIEvnWrap
{
public:
    JNIEvnWrap()
    {
        m_pThreadJni = NULL;
        m_bAttachThread = false;
        if (NULL == mJvm) {
            return;
        }
        
        if (mJvm->GetEnv ((void **)&m_pThreadJni, JNI_VERSION_1_4) != JNI_OK)
        {
            int status = mJvm->AttachCurrentThread (&m_pThreadJni, 0);
            if (status >= 0)
            {
                m_bAttachThread = true;
            }
        }
        
    }
    ~JNIEvnWrap()
    {
        if ((NULL != m_pThreadJni) && m_bAttachThread)
        {
            mJvm->DetachCurrentThread ();
        }
    }
    
    JNIEnv* m_pThreadJni;
    bool m_bAttachThread;
};


//初始化的时候会调进来一次，在这个方法里持有jvm的引用
JNIEXPORT jint JNICALL JNI_OnLoad (JavaVM *jvm, void *reserved)
{
    mJvm = jvm;
    JNIEnv *pEnv = NULL;
    if (NULL == mJvm)
    {
        return -1;
    }
    if (mJvm->GetEnv ((void **)&pEnv, JNI_VERSION_1_4) != JNI_OK)
    {
        return -1;
    }

    //语音识别的接口部分
    //提供的更新接口
    jclass youmeMgrClass = pEnv->FindClass ("com/youme/imengine/YouMeSpeechRecognizer");
    if (NULL == youmeMgrClass)
    {
        return -1;
    }
    mYouMeManagerClass = (jclass)(pEnv->NewGlobalRef (youmeMgrClass));
    m_YouMeStartListen = pEnv->GetStaticMethodID (mYouMeManagerClass, "StartListening", "(J)I");
    m_YouMeStartListenOnlyAudio = pEnv->GetStaticMethodID (mYouMeManagerClass, "StartListeningOnlyAudio", "(J)I");
    m_YouMeStopListen = pEnv->GetStaticMethodID (mYouMeManagerClass, "StopListening", "()V");
    
     m_YouMeCancleListen = pEnv->GetStaticMethodID (mYouMeManagerClass, "CancleListening", "()V");
    
    m_YouMePlayAudio = pEnv->GetStaticMethodID (mYouMeManagerClass, "PlayAudio", "(Ljava/lang/String;)V");
    m_YouMeSetAudioCacheDir = pEnv->GetStaticMethodID (mYouMeManagerClass, "SetCacheDir", "(Ljava/lang/String;)V");
	
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload (JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;
    
    if (vm->GetEnv ((void **)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        return;
    }
    if (NULL == env)
    {
        return;
    }
    env->DeleteGlobalRef (mYouMeManagerClass);
}

// char* to jstring
jstring string2jstring (JNIEnv *env, const char *str)
{
    jstring rtstr = env->NewStringUTF (str);
    return rtstr;
}



std::string jstring2string (JNIEnv *env, jstring jstr)
{
    std::string strResult;
    jclass clsstring = env->FindClass ("java/lang/String");
    jstring strencode = env->NewStringUTF ("utf-8");
    jmethodID mid = env->GetMethodID (clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod (jstr, mid, strencode);
    jsize alen = env->GetArrayLength (barr);
    jbyte *ba = env->GetByteArrayElements (barr, JNI_FALSE);
    if (alen > 0)
    {
        strResult = std::string((const char*)ba,alen);
    }
    env->ReleaseByteArrayElements (barr, ba, 0);
    return strResult;
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setModel(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetModel (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setSysVersion(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetSysVer (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setDeviceIMEI(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetIdentify (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setCPUArch(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetCPUArch (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setPackageName(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetPackageName (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setDocumentPath(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetDocumentDir (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_onNetWorkChanged(JNIEnv *env, jclass object, jint para)
{
    youmecommon::CNetworkService::Instance()->onNetWorkChanged ((youmecommon::NetworkType)para);
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setBrand(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetBrand (jstring2string (env, para));
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_setCPUChip(JNIEnv *env, jclass object, jstring para)
{
    CYMApplication::GetInstance().SetCPUChip (jstring2string (env, para));
}
JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_NotifySpeechFinish
(JNIEnv *env, jclass,jlong ulSerial,jint iEvent, jint iState, jstring result,jstring wavpath)
{
    if (NULL != g_AndroidSpeechListen) {
        if (iEvent == 0) {
            //初始化事件
            g_AndroidSpeechListen->OnSpeechInit(iState);
        }
        else if(iEvent == 1)
        {
            g_AndroidSpeechListen->OnSpeechResult(ulSerial,iState,jstring2string(env,result),jstring2string(env,wavpath));
        }
    }
}



JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_Init
  (JNIEnv *env, jclass clazz, jstring strAppKey, jstring strSecrect)
{
	std::string appKey = jstring2string(env, strAppKey);
	std::string secrect = jstring2string(env, strSecrect);
	return (jint)IM_Init(appKey.c_str(), secrect.c_str());
}

JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_Uninit
  (JNIEnv * env, jclass clazz)
{
	IM_Uninit();
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_Login
  (JNIEnv *env, jclass clazz, jstring strYouMeID, jstring strPasswd, jstring strOldPasswd)
{
	std::string id = jstring2string(env, strYouMeID);
	std::string pw = jstring2string(env, strPasswd);
	std::string oldPW = jstring2string(env, strOldPasswd);
	return (jint)IM_Login(id.c_str(), pw.c_str(), oldPW.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_Logout
  (JNIEnv *env, jclass clazz)
{
	IM_Logout();
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_GetContactList
  (JNIEnv * env, jclass clazz)
{
	return (jint)IM_GetContactList();
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_AddContact
  (JNIEnv * env, jclass clazz, jstring strYouMeID, jstring strReason)
{
	std::string id = jstring2string(env, strYouMeID);
	std::string reason = jstring2string(env, strReason);
	return (jint)IM_AddContact(id.c_str(), reason.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_AgreeContactInvited
  (JNIEnv *env, jclass clazz, jstring strYouMeID)
{
	std::string id = jstring2string(env, strYouMeID);
	return (jint)IM_AgreeContactInvited(id.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_RefuseContactInvited
  (JNIEnv *env, jclass clazz, jstring strYouMeID, jstring strReason)
{
	std::string id = jstring2string(env, strYouMeID);
	std::string reason = jstring2string(env, strReason);
	return (jint)IM_RefuseContactInvited(id.c_str(), reason.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_DeleteContact
  (JNIEnv * env, jclass clazz, jstring strYouMeID)
{
	std::string id = jstring2string(env, strYouMeID);
	return (jint)IM_DeleteContact(id.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_GetContactOfflineMessage
  (JNIEnv * env, jclass clazz)
{
	return (jint)IM_GetContactOfflineMessage();
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendTextMessage
  (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jstring strContent, jobject requestID)
{
	std::string recvID = jstring2string(env, strRecvID);
	std::string content = jstring2string(env, strContent);
	int requestid = 0;
	jint errorcode = (jint)IM_SendTextMessage(recvID.c_str(), (YouMeIMChatType)chatType, content.c_str(), &requestid);
	YouMe_LOG_Debug("********requestid:%d\n", requestid);
	if (NULL == requestID)
		return 10000;
	jclass class_name = env->GetObjectClass(requestID);
	if (NULL == class_name)
		return 10000;
	jmethodID mid = env->GetMethodID(class_name, "setId", "(I)V");
	if (NULL == mid)
		return 10000;
	env->CallIntMethod(requestID, mid, requestid);
	return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendCustomMessage
  (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jstring strCustomMessage, jint iBufferLen, jobject requestID)
{
	std::string recvID = jstring2string(env, strRecvID);
	std::string customMsg = jstring2string(env, strCustomMessage);
	int requestid = 0;
	jint errorcode = (jint)IM_SendCustomMessage(recvID.c_str(), (YouMeIMChatType)chatType, customMsg.c_str(), (int)iBufferLen, (int*)&requestid);
	YouMe_LOG_Debug("********requestid:%d\n", requestid);
	if (NULL == requestID)
		return 10000;
	jclass class_name = env->GetObjectClass(requestID);
	if (NULL == class_name)
		return 10000;
	jmethodID mid = env->GetMethodID(class_name, "setId", "(I)V");
	if (NULL == mid)
		return 10000;
	env->CallIntMethod(requestID, mid, requestid);
	return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendAudioMessage
  (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jobject requestID)
{
	std::string recvID = jstring2string(env, strRecvID);
	int requestid = 0;
	jint errorcode = (jint)IM_SendAudioMessage(recvID.c_str(), (YouMeIMChatType)chatType, (int*)&requestid);
	YouMe_LOG_Debug("********requestid:%d\n", requestid);
	if (NULL == requestID)
		return 10000;
	jclass class_name = env->GetObjectClass(requestID);
	if (NULL == class_name)
		return 10000;
	jmethodID mid = env->GetMethodID(class_name, "setId", "(I)V");
	if (NULL == mid)
		return 10000;
	env->CallIntMethod(requestID, mid, requestid);
	return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_SendOnlyAudioMessage
  (JNIEnv * env, jclass clazz, jstring strRecvID, jint chatType, jobject requestID)
{
	std::string recvID = jstring2string(env, strRecvID);
	int requestid = 0;
	jint errorcode = (jint)IM_SendOnlyAudioMessage(recvID.c_str(), (YouMeIMChatType)chatType, (int*)&requestid);
	YouMe_LOG_Debug("********requestid:%d\n", requestid);
	if (NULL == requestID)
		return 10000;
	jclass class_name = env->GetObjectClass(requestID);
	if (NULL == class_name)
		return 10000;
	jmethodID mid = env->GetMethodID(class_name, "setId", "(I)V");
	if (NULL == mid)
		return 10000;
	env->CallIntMethod(requestID, mid, requestid);
	return errorcode;
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_StopAudioMessage
  (JNIEnv * env, jclass clazz, jstring strParam)
{
	std::string param = jstring2string(env, strParam);
	return (jint)IM_StopAudioMessage(param.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_CancleAudioMessage
  (JNIEnv * env, jclass clazz)
{
	return (jint)IM_CancleAudioMessage();
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_DownloadAudioFile
  (JNIEnv * env, jclass clazz, jint iSerial, jstring strSavePath)
{
	std::string path = jstring2string(env, strSavePath);
	return (jint)IM_DownloadAudioFile((int)iSerial, path.c_str());
}


JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_JoinChatRoom
  (JNIEnv * env, jclass clazz, jstring strID)
{
	std::string id = jstring2string(env, strID);
	return (jint)IM_JoinChatRoom(id.c_str());
}

JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_LeaveChatRoom
  (JNIEnv * env, jclass clazz, jstring strID)
{
	std::string id = jstring2string(env, strID);
	return (jint)IM_LeaveChatRoom(id.c_str());
}


JNIEXPORT jstring JNICALL Java_com_youme_imengine_NativeEngine_GetMessage
  (JNIEnv * env, jclass clazz)
{
	const XCHAR* strMsg = IM_GetMessage();
	return string2jstring(env, strMsg);
}


JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_PopMessage
  (JNIEnv * env, jclass clazz)
{
	IM_PopMessage();
}


JNIEXPORT jint JNICALL Java_com_youme_imengine_NativeEngine_GetSDKVer
  (JNIEnv * env, jclass clazz)
{
	return (jint)IM_GetSDKVer();
}


JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_SetServerZone
  (JNIEnv *env, jclass clazz, jint zone)
{
	IM_SetServerZone((ServerZone)zone);
}


JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_SetMode
  (JNIEnv * env, jclass clazz, jint iMode)
{
	IM_SetMode((int)iMode);
}


JNIEXPORT jstring JNICALL Java_com_youme_imengine_NativeEngine_GetFilterText
  (JNIEnv * env, jclass clazz, jstring strSource)
{
	std::string source = jstring2string(env, strSource);
	XCHAR* strRes = IM_GetFilterText(source.c_str());
	return string2jstring(env, strRes);
}


JNIEXPORT void JNICALL Java_com_youme_imengine_NativeEngine_DestroyFilterText
  (JNIEnv * env, jclass clazz, jstring strSource)
{
	std::string source = jstring2string(env, strSource);
	//XCHAR* pszSource = source.c_str();
	//IM_DestroyFilterText(pszSource);
}


int YouMe_StartListening(long long ulSerial)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    
    return jniWrap.m_pThreadJni->CallStaticIntMethod (mYouMeManagerClass, m_YouMeStartListen,ulSerial);
    
}
int YouMe_StartListeningOnlyAudio(long long ulSerial)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return -1;
    }
    
    return jniWrap.m_pThreadJni->CallStaticIntMethod (mYouMeManagerClass, m_YouMeStartListenOnlyAudio,ulSerial);
    
}


void YouMe_SetAndroidSpeechListen(ISpeechListen* pListen)
{
    g_AndroidSpeechListen = pListen;
}

void YouMe_StopListening()
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    
    return jniWrap.m_pThreadJni->CallStaticVoidMethod (mYouMeManagerClass, m_YouMeStopListen);
    
}

void YouMe_CancleListening()
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    
    return jniWrap.m_pThreadJni->CallStaticVoidMethod (mYouMeManagerClass, m_YouMeCancleListen);
    
}
void YouMe_PlayAudio(const char* pszPath)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    
    return jniWrap.m_pThreadJni->CallStaticVoidMethod (mYouMeManagerClass, m_YouMePlayAudio,string2jstring(jniWrap.m_pThreadJni,pszPath));
}

void YouMe_SetCacheDir(const char* pszPath)
{
    JNIEvnWrap jniWrap;
    if (NULL == jniWrap.m_pThreadJni)
    {
        return;
    }
    
    return jniWrap.m_pThreadJni->CallStaticVoidMethod (mYouMeManagerClass, m_YouMeSetAudioCacheDir,string2jstring(jniWrap.m_pThreadJni,pszPath));
}






