#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeCommon/Log.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include <YouMeIMEngine/YouMeIMManager.h>
#include <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/MonitorDataReport.h>
#include <YouMeCommon/NetworkService.h>
#include <YouMeIMEngine/version.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeIMEngine/Speech/YouMeSpeechManager.h>
#include <YouMeIMEngine/KeywordFilter.h>
bool s_bYouMeOpenLog = false;
bool s_bYouMeStartup = false;
ServerZone g_ServerZone = ServerZone_China;
int g_iYouMeIMMode = 0;
XString g_audioCacheDir = XString(__XT(""));

//youmecommon::CMonitorDataReport *g_pMonitorPtr = NULL;
CYouMeSpeechManager* g_pSpeechManager = NULL;
CKeywordFilter* g_pKeywordFilter = NULL;
IYouMeIMGroupMemberInfo::~IYouMeIMGroupMemberInfo()
{

}

IYouMeIMMessageBodyBase::~IYouMeIMMessageBodyBase()
{

}
IYouMeIMMessage::~IYouMeIMMessage()
{

}

IYouMeIMContactManager::~IYouMeIMContactManager()
{
    
}
IYouMeIMGroupManager::~IYouMeIMGroupManager()
{
    
}
IYouMeIMMessageManager::~IYouMeIMMessageManager()
{
    
}

IYouMeIMManager* IYouMeIMManager::CreateInstance()
{
	return new YouMeIMManager;
}

void IYouMeIMManager::DestroyInstance(IYouMeIMManager* pInsance)
{
	delete pInsance;
}

IYouMeIMManager::~IYouMeIMManager()
{

}

void IYouMeIMManager::Startup()
{
	if (s_bYouMeStartup)
	{
		return;
	}
#ifdef WIN32
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif // WIN32
	youmecommon::CNetworkService::Instance();
	//g_pMonitorPtr = new youmecommon::CMonitorDataReport;
#ifdef OS_ANDROID
    //暂时还是放到sd 卡上吧
    YouMe_LOG_Init(__XT("/sdcard/YouMeIMLog.txt"));
#else
	YouMe_LOG_Init(youmecommon::CXFile::CombinePath(CYMApplication::GetInstance().GetDocumentDir(), __XT("YouMeIMLog.txt")));
#endif
    g_pSpeechManager = new CYouMeSpeechManager;
	g_pSpeechManager->Init();
	if (!g_audioCacheDir.empty())
	{
		g_pSpeechManager->SetAudioCacheDir(g_audioCacheDir);
	}

	g_pKeywordFilter = new CKeywordFilter;
    s_bYouMeStartup = true;
}

void IYouMeIMManager::CleanUp()
{
	if (!s_bYouMeStartup)
	{
		return;
	}
	youmecommon::CNetworkService::DestroyInstance();
	//g_pMonitorPtr->UnInit();
	//delete g_pMonitorPtr;
	//g_pMonitorPtr = NULL;
    g_pSpeechManager->UnInit();
    delete g_pSpeechManager;
    g_pSpeechManager = NULL;
	delete g_pKeywordFilter;
	g_pKeywordFilter = NULL;
#ifdef WIN32
	WSACleanup();
#endif // WIN32
     s_bYouMeStartup = false;
}

int IYouMeIMManager::GetSDKVersion()
{
 return	SDK_VER;
}

void IYouMeIMManager::SetServerZone(ServerZone zone)
{
	g_ServerZone = zone;
}
void IYouMeIMManager::SetMode(int iMode)
{
    g_iYouMeIMMode = iMode;
}
XString IYouMeIMManager::FilterKeyword(const XString& strSource)
{
	if (NULL == g_pKeywordFilter)
	{
		return strSource;
	}
	return g_pKeywordFilter->GetFilterText(strSource);
}

void IYouMeIMManager::SetAudioCacheDir(const XString& path)
{
	if (path.empty())
	{
		return;
	}
	g_audioCacheDir = path;
	if (g_pSpeechManager != NULL)
	{
		g_pSpeechManager->SetAudioCacheDir(path);
	}
}