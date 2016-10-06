// stdafx.cpp : 只包括标准包含文件的源文件
// CrossPlatformDefine.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

/************************************************************************/
/*
											  _ooOoo_
                                           o8888888888o
                                        888    " . "   888
                                      (|        -_-       |)
                                     O\          =         /O
                                     ____/`     ---   '\____
                               .'  \\|                     |//  `.
                               /  \\|||          :          |||//  \
                               /  _|||||        -:-         |||||-  \
                          |   | \\\              -               /// |   |
                           | \_|             ''\---/''              |_/ |
                           \  .-\__             `-`              __/-. /
                     __    ___`. .'           /--.--\            `. . _____
                         ."" '<  `.___\_        <|>          _/___.'  >' "".
                   | | :  `- \`.;`             \ _ /              `;.`/ - ` : | |
                     \  \ `-.              \_ __\ /__ _/              .-` /  /
				    	 ========`-.____`-.___\_____/___.-`____.-'========
                                              `=---='
								      佛祖保佑       永无BUG
 * /
************************************************************************/

#include "YouMeIMManager.h"

#include <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/SyncUDP.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeCommon/RSAUtil.h>
#include <YouMeCommon/MonitorDataReport.h>
#include <YouMeCommon/SDKValidate.hpp>
#include <YouMeIMEngine/ConfigEntry.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeIMEngine/version.h>
#include <YouMeCommon/curl/curl.h>
#include <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/DownloadUploadManager.h>
#include <YouMeCommon/minizip/MiniZip.h>
#include <YouMeCommon/minizip/MiniUnZip.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeIMEngine/KeywordFilter.h>
#include <YouMeCommon/amrFileCodec.h>
#define RECONNECT_COUNT 5
#define RECONNECT_TIMEOUT 5000
extern CKeywordFilter* g_pKeywordFilter;
extern bool s_bYouMeStartup;
extern ServerZone g_ServerZone;
XINT64 YouMeIMManager::s_iPacketSerial = 0;
std::mutex YouMeIMManager::m_serialMutex;
//extern youmecommon::CMonitorDataReport *g_pMonitorPtr ;
extern int g_iYouMeIMMode;

std::vector<ServerZoneInfo> YouMeIMManager::m_allServerInfo;

#define SDK_AUTH_URL __XT("imconfig.youme.im")
#define SDK_AUTH_URL_DEV __XT("d.imconfig.youme.im")
#define SDK_AUTH_URL_TEST __XT("t.imconfig.youme.im")
#define SDK_AUTH_URL_BUSSINESS __XT("b.imconfig.youme.im")
//心跳包连续3次就认为掉线了，要重连了
#define HEART_TIMEOUT_COUNT 1

#define ParseServerPacketFail() \
	{\
		YouMe_LOG_Warning(__XT("解析服务器的包失败"));\
		return ;\
	}\

#define LoginAssert() \
	if (m_logStatus != IMManangerLoginStatus_Success)\
			{\
		YouMe_LOG_Warning(__XT("状态错误：status:%d errorcode:%d loginstatus:%d"),m_initStatus,m_engineInitErrorcode,m_logStatus);\
		return YouMeIMErrorcode_NotLogin;\
			}\

void YouMeIMManager::SetLoginCallback(IYouMeIMLoginCallback* pCallback)
{
	m_pLoginCallback = pCallback;
}

//设置好友回掉
void YouMeIMManager::SetContactCallback(IYouMeIMContactCallback* pCallback)
{
    m_pContactCallback = pCallback;
	if (NULL != m_pContactManager)
	{
		m_pContactManager->SetCallback(pCallback);
	}
	
}
//设置消息回调
void YouMeIMManager::SetMessageCallback(IYouMeIMMessageCallback* pCallback)
{
    m_pMessageCallback = pCallback;
	if (NULL != m_pMessageManager)
	{
		m_pMessageManager->SetMessageCallback(pCallback);
	}
}
//设置群组回调
void YouMeIMManager::SetGroupCallback(IYouMeIMGroupCallback* pCallback)
{
    m_pGroupCallback = pCallback;
	if (NULL != m_pGroupManager)
	{
		m_pGroupManager->SetCallback(pCallback);
	}
}


YouMeIMProtocol::PacketHead* YouMeIMManager::CreatePacketHead(YouMeIMProtocol::Command_TYPE command, int iProVer /*= 1*/,XINT64 ulServerSerial)
{
	YouMeIMProtocol::PacketHead* pHead = new YouMeIMProtocol::PacketHead;
	pHead->set_msgtype(command);
	pHead->set_msgversion(iProVer);
	pHead->set_msgserialnum(GetUniqueSerial());
#ifdef WIN32
	pHead->set_platform(YouMeIMProtocol::Platform_Windows);
#elif (defined OS_OSX)
	pHead->set_platform(YouMeIMProtocol::Platform_OSX);
#elif (defined OS_IOS)
	pHead->set_platform(YouMeIMProtocol::Platform_IOS);
#elif (defined OS_IOSSIMULATOR)
	pHead->set_platform(YouMeIMProtocol::Platform_IOS);
#elif (defined OS_ANDROID)
	pHead->set_platform(YouMeIMProtocol::Platform_Android);
#else
#error "不支持的平台"
#endif
	if ((command == YouMeIMProtocol::Command_Login) || (command == YouMeIMProtocol::Command_Redirect))
	{
		pHead->set_brand(XStringToUTF8(CYMApplication::GetInstance().GetBrand()));
		pHead->set_sysversion(XStringToUTF8(CYMApplication::GetInstance().GetSysVer()));
		pHead->set_ymsdkversion(CYMApplication::GetInstance().GetYMSDKVersion());
		pHead->set_cpuarch(XStringToUTF8(CYMApplication::GetInstance().GetCPUArch()));
		pHead->set_packagename(XStringToUTF8(CYMApplication::GetInstance().GetPackageName()));
		pHead->set_identify(XStringToUTF8(CYMApplication::GetInstance().GetIdentify()));
		pHead->set_appkey(XStringToUTF8(CYMApplication::GetInstance().GetAppKey()));
		pHead->set_model(XStringToUTF8(CYMApplication::GetInstance().GetModel()));
		pHead->set_networktype((YouMeIMProtocol::NetworkType)youmecommon::CNetworkService::Instance()->getNetworkType());
		pHead->set_cpuchip(XStringToUTF8(CYMApplication::GetInstance().GetCPUChip()));
		pHead->set_sessionid(XStringToUTF8(m_strSessionID));
		pHead->set_youmeid(XStringToUTF8(m_strYouMeID));
		pHead->set_serverserialnum(ulServerSerial);

		pHead->set_localtime(GetServerValidTime());
		pHead->set_sdkname("");
	}
	else
	{
		pHead->set_brand("");
		pHead->set_sysversion("");
		pHead->set_ymsdkversion(CYMApplication::GetInstance().GetYMSDKVersion());
		pHead->set_cpuarch("");
		pHead->set_packagename("");
		pHead->set_identify("");
	//	pHead->set_appkey("");
		pHead->set_appkey(XStringToUTF8(CYMApplication::GetInstance().GetAppKey()));
		pHead->set_model("");
		pHead->set_networktype((YouMeIMProtocol::NetworkType)youmecommon::CNetworkService::Instance()->getNetworkType());
		pHead->set_cpuchip("");
		pHead->set_sessionid(XStringToUTF8(m_strSessionID));
		pHead->set_youmeid(XStringToUTF8(m_strYouMeID));
		pHead->set_serverserialnum(ulServerSerial);

		pHead->set_localtime(GetServerValidTime());
		pHead->set_sdkname("");
	}
	

	YouMeIMProtocol::NameValue* pUserLevel = pHead->add_params();
	if (pUserLevel != NULL)
	{
		pUserLevel->set_name("PlayerLevel");
		pUserLevel->set_value(CStringUtilT<char>::to_string(m_iUserLevel));
		pUserLevel->set_type(YouMeIMProtocol::NAME_UIN32);
	}
	return pHead;
}
YouMeIMErrorcode YouMeIMManager::Login(const XString& strYouMeID, const XString&strPasswd, const XString& strOldPasswd)
{
	YouMe_LOG_Info(__XT("登陆:%s enginestatus:%d  errorcode:%d loginstatus:%d"), strYouMeID.c_str(), m_initStatus, m_engineInitErrorcode, m_logStatus);
	//没有初始化
	if (m_initStatus == IMManagerStatus_NotInit)
	{
		if (m_engineInitErrorcode != YouMeIMErrorcode_Success)
		{
			return m_engineInitErrorcode;
		}
		return YouMeIMErrorcode_EngineNotInit;
	}
	if (m_initStatus == IMManagerStatus_uniniting)
	{
		return YouMeIMErrorcode_StatusError;
	}
	if (m_logStatus == IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_AlreadyLogin;
	}
	if((m_logStatus == IMManangerLoginStatus_Ing) || (m_logStatus == IMManangerLoginStatus_Logouting))
	{
		return YouMeIMErrorcode_StatusError;
	}
	if ((strYouMeID.empty()) || (strPasswd.empty()))
	{
		return YouMeIMErrorcode_ParamInvalid;
	}
    //校验一下房间ID，只允许字母，数字，下划线
    for (size_t i=0; i<strYouMeID.length(); i++) {
        if (!CStringUtil::IsValidChar(strYouMeID.at(i))) {
            return YouMeIMErrorcode_ParamInvalid;
        }
    }
    
	//放在最前面
	m_strYouMeID = strYouMeID;
	m_strPasswd = strPasswd;
	m_strOldPasswd = strOldPasswd;
	//启动线程用来重定向
	m_redirectWait.Reset();
	m_heartBeatWait.Reset();
	m_reconnectTimeout.Reset();
	m_logoutWait.Reset();
	m_logStatus = IMManangerLoginStatus_Ing;
	if (m_loginThread.joinable())
	{
		m_loginThread.join();
	}
	if (m_initStatus == IMManagerStatus_initfailed)
	{
		m_initStatus = IMManagerStatus_Ing;
		if (m_engInitThread.joinable())
		{
			m_engInitThread.join();
		}
		m_engInitThread = std::thread(&YouMeIMManager::EngineInitThreadProc, this);
	}
	m_loginThread = std::thread(&YouMeIMManager::LoginThread, this,strPasswd,strOldPasswd);	
	YouMe_LOG_Info(__XT("Leave"));
	return YouMeIMErrorcode_Success;
}

YouMeIMErrorcode YouMeIMManager::Logout()
{
	LoginAssert();
	YouMe_LOG_Info(__XT("Enter"));
	m_logStatus = IMManangerLoginStatus_Logouting;
	{
		std::lock_guard<std::mutex> lock(m_messageCacheMutex);
		m_messageMap.clear();
	}
	m_redirectWait.SetSignal();
	//停止心跳线程
	m_heartBeatWait.SetSignal();
	m_reconnectTimeout.SetSignal();
	//是否需要等待登陆线程退出
	if (m_loginThread.joinable())
	{
		m_loginThread.join();
	}
	if (m_reconnectThread.joinable())
	{		
		m_reconnectThread.join();
	}
	//构造请求包
	YouMeIMProtocol::YouMeIM_Command_Logout login;
	login.set_allocated_msghead(CreatePacketHead(YouMeIMProtocol::Command_LogOut));
	//构造数据
	std::string strData;
	login.SerializeToString(&strData);
	bool bSuccess = m_pUDPManager->SendData(YouMeIMProtocol::Command_LogOut,strData.c_str(), strData.length(), login.msghead().msgserialnum());
	if (bSuccess)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_LogOut));
		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[login.msghead().msgserialnum()] = pMessage;
		}
	}
	else
	{
		std::thread cleanup(&YouMeIMManager::DealLogoutCleanup, this);
		cleanup.detach();
	}
	YouMe_LOG_Info(__XT("Leave"));
	return YouMeIMErrorcode_Success;
}



void YouMeIMManager::OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize)
{
    if (NULL == m_pLoginCallback) {
        YouMe_LOG_Error(__XT("IMManager 回调指针为空，上层是不是忘记设置了"));
    }
	//先自己处理，自己处理完了再转发给其他的管理器
	if (head.head().msgtype() != YouMeIMProtocol::Command_HeartResponse)
	{
		YouMe_LOG_Info(__XT("接收到服务器发来的包:type:%d  serial:%lld code:%d size:%d clientserial:%lld"), head.head().msgtype(), head.head().msgserialnum(), head.head().code(), iBufferSize, head.head().clientserialnum());
	}
	switch (head.head().msgtype())
	{
		case YouMeIMProtocol::Command_LoginResponse:
		{
			assert((m_logStatus == IMManangerLoginStatus_Ing) || (m_logStatus == IMManangerLoginStatus_Reconnecting));
			if (m_logStatus == IMManangerLoginStatus_Success)
			{
				return;
			}
			
			{
				std::lock_guard<std::mutex> lock(m_messageCacheMutex);
				std::map<XINT64, std::shared_ptr<MessageInfo> >::iterator it = m_messageMap.find(head.head().clientserialnum());
				if (it != m_messageMap.end())
				{
					m_messageMap.erase(it);
				}
				else
				{
					assert(false);
				}
			}

			//登陆回复
			YouMeIMProtocol::YouMeIM_Command_LoginResponse response;
			YouMeIMErrorcode errorcode = ParseRSASecret((const char*)pBuffer, iBufferSize, response);
			if (errorcode == YouMeIMErrorcode_Success)
			{
				m_strSessionID = UTF8TOXString(response.sessionid());
				IMManangerLoginStatus lastLogStatus = m_logStatus;
				m_logStatus = IMManangerLoginStatus_Success;

				std::shared_ptr<CYouMeIMContactInfo> pInfo(new CYouMeIMContactInfo);
				pInfo->m_iStatus = response.userinfo().status();
				pInfo->m_strYouMeID = UTF8TOXString(response.userinfo().youmeid());
                if (NULL != m_pLoginCallback)
				{
					if (lastLogStatus != IMManangerLoginStatus_Reconnecting)
					{
						m_pLoginCallback->OnLogin((YouMeIMErrorcode)response.msghead().code(), pInfo.get());
					}
					else
					{
						//获取一下离线消息。因为在重连之前可能有很多消息没收到
					//	GetMessageManager()->GetContactOfflineMessage();
						//重新加入房间
						YouMeIMGroupManager* pGroupManager = (YouMeIMGroupManager*) GetGroupManager();
						pGroupManager->ReJoinChatRoom();
					}
                }
				

                //获取一下各接口对象
                GetGroupManager();
                GetContactManager();
                GetMessageManager();
				//启动心跳线程	
				m_iLossPacketCount = 0;
				m_heartBeatWait.SetSignal();
				if (m_heartBeatThread.joinable())
				{
					m_heartBeatThread.join();
				}

				m_heartBeatWait.Reset();
				m_heartBeatThread = std::thread(&YouMeIMManager::HeartBeatThreadProc, this);
			}
			else
			{
				m_logStatus = IMManangerLoginStatus_Not;
				if (NULL != m_pLoginCallback)
				{
					m_pLoginCallback->OnLogin(errorcode, NULL);
				}
			}
			
		}
		break;
		case YouMeIMProtocol::Command_LogOutResponse:
		{	
            //不要在回调里面做，否则会死锁
            std::thread cleanup(&YouMeIMManager::DealLogoutCleanup,this);
            cleanup.detach();
			
		}
		break;
		case YouMeIMProtocol::Command_HeartResponse:
		{
			//收到心跳了
			m_iLossPacketCount = 0;
		}
		break;
		default:
		{
			bool bDeal = false;
			if (NULL != m_pContactManager)
			{
				bDeal = m_pContactManager->OnRecvPacket(head, pBuffer, iBufferSize);
			}
			if ((!bDeal) && (NULL != m_pMessageManager))
			{
				bDeal = m_pMessageManager->OnRecvPacket(head, pBuffer, iBufferSize);
			}
			if ((!bDeal) && (NULL != m_pGroupManager))
			{
				bDeal = m_pGroupManager->OnRecvPacket(head, pBuffer, iBufferSize);
			}
			if (!bDeal)
			{
				YouMe_LOG_Warning(__XT("收到了没有处理的业务包  serial:%lld type:%d"), head.head().msgserialnum(), head.head().msgtype());
			}
		}
		break;
	}
}

// curl 的回调
size_t OnRequestWriteData (void *buffer, size_t size, size_t nmemb, void *user_p)
{
    std::string *pResponse = (std::string *)user_p;
    pResponse->append ((char *)buffer, nmemb);
    return nmemb;
}

YouMeIMErrorcode YouMeIMManager::Init(const XString& strAppKey, const XString& strAppSecurity, const XString& strPackage)
{
	YouMe_LOG_Info(__XT("Enter"));
	if (!s_bYouMeStartup)
	{
		YouMe_LOG_Warning(__XT("没有调用startup 函数"));
		return YouMeIMErrorcode_NotStartUp;
	}
	//判断一下状态了
	if ((m_initStatus != IMManagerStatus_NotInit) && (m_initStatus != IMManagerStatus_initfailed))
	{
		return YouMeIMErrorcode_StatusError;
	}
	//先简单校验一下参数，都不能为空
	if ((strAppKey.empty()) || (strAppSecurity.empty()))
	{
		YouMe_LOG_Warning(__XT("传入的参数不对:%s :%s"),strAppKey.c_str(),strAppSecurity.c_str());
		return YouMeIMErrorcode_ParamInvalid;
	}
#ifdef WIN32
	CYMApplication::GetInstance().SetPackageName(__XT("com.youme.windows"));
#else
	if (!strPackage.empty())
	{
		YouMe_LOG_Warning(__XT("其他平台不需要传入package参数，即使传入也会忽略"));
	}
#endif

	m_initStatus = IMManagerStatus_Ing;
	m_engineInitErrorcode = YouMeIMErrorcode_Success;
	CYMApplication::GetInstance().SetAppKey(strAppKey);
	CYMApplication::GetInstance().SetAppSecret(strAppSecurity);
	CYMApplication::GetInstance().SetYMSDKVersion(SDK_VER);//先随便设置一个

	/************************************************************************/
	/*   打印一下开始                                                                     */
	/************************************************************************/
	YouMe_LOG_Info(__XT("------------------------------------Start---------------------------------------------"));

	YouMe_LOG_Info(__XT("\nappkey:%s\npackage:%s\nbrand:%s\nmodel:%s\niosver:%s\nsdkver:%d\ncpuarch:%s\nidentify:%s\nserverzone:%d\nmode:%d\n"), strAppKey.c_str(),
		CYMApplication::GetInstance().GetPackageName().c_str(),
		CYMApplication::GetInstance().GetBrand().c_str(),
		CYMApplication::GetInstance().GetModel().c_str(),
		CYMApplication::GetInstance().GetSysVer().c_str(),
		CYMApplication::GetInstance().GetYMSDKVersion(),
		CYMApplication::GetInstance().GetCPUArch().c_str(),
		CYMApplication::GetInstance().GetIdentify().c_str(),
		g_ServerZone,
		g_iYouMeIMMode);
	//初始化网络监听
	youmecommon::CNetworkService::Instance()->registerCallback(this);
	youmecommon::CNetworkService::Instance()->start();
	//启动线程用来SDK 验证
	m_engInitThread = std::thread(&YouMeIMManager::EngineInitThreadProc, this);
	YouMe_LOG_Info(__XT("Leave"));
	return YouMeIMErrorcode_Success;
}

void YouMeIMManager::UnInit()
{
	YouMe_LOG_Info(__XT("Enter enginestatus:%d errorcode:%d loginstatus:%d"),m_initStatus, m_engineInitErrorcode,m_logStatus);
	
	if (m_logStatus == IMManangerLoginStatus_Success)
	{
		Logout();
		YouMe_LOG_Info(__XT("登出完成"));
	}
	if (m_logStatus == IMManangerLoginStatus_Logouting)
	{
		m_logoutWait.WaitTime(2000);
	}
	//是否需要等待登陆线程退出
	if (m_loginThread.joinable())
	{
		m_loginThread.join();
	}
	if (m_reconnectThread.joinable())
	{
		m_reconnectThread.join();
	}
	m_sdkValidWait.SetSignal();
	if (m_engInitThread.joinable()) {
		m_engInitThread.join();
	}
	m_heartBeatWait.SetSignal();
	if (m_heartBeatThread.joinable())
	{
		m_heartBeatThread.join();
	}
	m_initStatus = IMManagerStatus_NotInit;
	youmecommon::CNetworkService::Instance()->cancleCallback(this);
	YouMe_LOG_Info(__XT("Leave"));
}

IYouMeIMContactManager* YouMeIMManager::GetContactManager()
{
	if (NULL == m_pContactManager)
	{
		std::lock_guard<std::mutex> lock(m_managerMutex);
		if (NULL == m_pContactManager)
		{
			m_pContactManager = new YouMeIMContactManager(this);
			m_pContactManager->Init();
            m_pContactManager->SetCallback(m_pContactCallback);
		}
	}
	return m_pContactManager;
}

IYouMeIMGroupManager* YouMeIMManager::GetGroupManager()
{
	if (NULL == m_pGroupManager)
	{
		std::lock_guard<std::mutex> lock(m_managerMutex);
		if (NULL == m_pGroupManager)
		{
			m_pGroupManager = new YouMeIMGroupManager(this);
			m_pGroupManager->Init();
            m_pGroupManager->SetCallback(m_pGroupCallback);
		}
	}
	return m_pGroupManager;
}

IYouMeIMMessageManager* YouMeIMManager::GetMessageManager()
{
	if (NULL == m_pMessageManager)
	{
		std::lock_guard<std::mutex> lock(m_managerMutex);
		if (NULL == m_pMessageManager)
		{
			m_pMessageManager = new YouMeIMMessageManager(this);
			m_pMessageManager->Init();
            m_pMessageManager->SetMessageCallback(m_pMessageCallback);
			m_pMessageManager->SetDownloadCallback(m_pDownloadCallback);
		}
	}
	return m_pMessageManager;
}

void YouMeIMManager::SetUserLevle(int level)
{
	m_iUserLevel = level;
}

void YouMeIMManager::EngineInitThreadProc()
{
	YouMe_LOG_Info(__XT("Enter"));
	youmecommon::CSDKValidate sdkValid;
	XString strAuthURL = SDK_AUTH_URL;
	XString strAuthIP = __XT("106.75.25.67");
	if (g_iYouMeIMMode == 0)
	{
		//正式环境
		if (g_ServerZone < m_allServerInfo.size())
		{
			strAuthURL = m_allServerInfo[g_ServerZone].strSDKValidateURL;
			strAuthIP = m_allServerInfo[g_ServerZone].strSDKValidateIP;
		}
	}
	else if (g_iYouMeIMMode == 1)
	{
		strAuthURL = SDK_AUTH_URL_DEV;
	}
	else if (g_iYouMeIMMode == 2)
	{
		strAuthURL = SDK_AUTH_URL_TEST;
	}
	else if (g_iYouMeIMMode == 3)
	{
		strAuthURL = SDK_AUTH_URL_BUSSINESS;
	}
	YouMe_LOG_Info(__XT("传入SDK 验证参数:%s %s"),strAuthURL.c_str(),strAuthIP.c_str());
	if (!sdkValid.Init(strAuthURL, 8010, strAuthIP))
	{
		YouMe_LOG_Error(__XT("SDK 校验传入参数就失败了"));
		m_initStatus = IMManagerStatus_NotInit;
		m_logStatus = IMManangerLoginStatus_Not;
		m_engineInitErrorcode = YouMeIMErrorcode_SDKInvalid;
		return;
	}
	//记录一个开始时间
	XINT64 ulStartTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	m_configurations.clear();
	youmecommon::SDKValidateErrorcode sdkValidErrocode = sdkValid.StartValidate(m_configurations, &m_sdkValidWait);
	//结束时间
	XINT64 ulEndTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
    YouMe_LOG_Info(__XT("SDK 校验验证结束 time:%lld errrocode: %d"),ulEndTime-ulStartTime,sdkValidErrocode);
	if (sdkValidErrocode != youmecommon::SDKValidateErrorcode_Success)
	{
		//失败的时候也要上报的
		m_initStatus = IMManagerStatus_initfailed;
		m_logStatus = IMManangerLoginStatus_Not;
		m_engineInitErrorcode = YouMeIMErrorcode_SDKInvalid;
		return;
	}
    YouMe_LOG_Info(__XT("当前服务器时间戳:%lld"),GetServerValidTime());
	if (!g_pKeywordFilter->IsLoadSuccess())
    {
		XString strURL = GetConfigure(CConfigEntry::KEYWORD_ADDRESS, XString(__XT("")));
		if (strURL != __XT(""))
		{
			XString strKeywordMD5 = GetConfigure(CConfigEntry::KEYWORD_MD5, XString(__XT("")));
			std::thread keywordHttpThread(&YouMeIMManager::DownloadKeyword, this, strURL,strKeywordMD5);
			keywordHttpThread.detach();
		}		
    }
	m_localTime = youmecommon::CTimeUtil::GetTimeOfDay_MS()/1000;
	m_initStatus = IMManagerStatus_Init;
	//初始化数据上报
	//g_pMonitorPtr->Init(GetConfigure<XString>(CConfigEntry::DATAREPORT_ADDR, CConfigEntry::DATAREPORT_ADDR_DEFAULT),
	//	GetConfigure<int>(CConfigEntry::DATAREPORT_PORT, CConfigEntry::DATAREPORT_PORT_DEFAULT), __XT("IMReport.db"), __XT("192.168.1.1"));
	{
		//开始上报了
		std::vector<youmecommon::CXVariant> reportData;
		//0 表示成功
		reportData.push_back(0);
		reportData.push_back(ulEndTime - ulStartTime);
		//g_pMonitorPtr->Report(DataReportEvtType_SDKValidEnd,reportData, GetServerValidTime());
	}

	YouMe_LOG_SetLevel(static_cast<YouMe_LOG_LEVEL>(GetConfigure<int>(CConfigEntry::LOG_LEVEL, LOG_LEVEL_INFO)));

	m_engineInitErrorcode = YouMeIMErrorcode_Success;
	YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMManager::LoginThread(const XString& strPasswd,const XString& strOldPasswd)
{
	YouMe_LOG_Info(__XT("Enter m_initStatus:%d"), m_initStatus);
	if (m_initStatus == IMManagerStatus_Ing)
	{
		//等待SDK验证线程退出
		if (m_engInitThread.joinable())
		{
			m_engInitThread.join();
		}
	}
	if (m_initStatus != IMManagerStatus_Init)
	{
		if (NULL != m_pLoginCallback)
		{
			m_pLoginCallback->OnLogin(m_engineInitErrorcode, NULL);
		}
		m_logStatus = IMManangerLoginStatus_Not;
		return;
	}
	//记录一下重定向开始时间
	XINT64 ulStartTime = youmecommon::CTimeUtil::GetTimeOfDay_MS() ;
	//开始重定向
	bool bParseRedirectSuccess = true;
	XString strServerIP;
	int iPort=-1;
	{
		//发送重定向请求
		YouMeIMProtocol::YouMeIM_Command_Redirect request;
		request.set_allocated_msghead(CreatePacketHead(YouMeIMProtocol::Command_Redirect));
		std::string strData;
		request.SerializeToString(&strData);
		youmecommon::CSyncTCP udp;

		udp.Init(GetConfigure<XString>(CConfigEntry::REDIRECT_ADDR, CConfigEntry::REDIRECT_ADDR_DEFAULT),
			GetConfigure<int>(CConfigEntry::REDIRECT_PORT,CConfigEntry::REDIRECT_PORT_DEFAULT));
		if (!udp.Connect(3000))
		{
			YouMe_LOG_Error(__XT("连接重定向服务器失败:%s"), GetConfigure<XString>(CConfigEntry::REDIRECT_ADDR, CConfigEntry::REDIRECT_ADDR_DEFAULT).c_str());
			m_logStatus = IMManangerLoginStatus_Not;
			if (NULL != m_pLoginCallback)
			{
				m_pLoginCallback->OnLogin(YouMeIMErrorcode_NetError, NULL);
			}
			return;
		}
		udp.SendData(strData.c_str(), strData.length());
		
		//接受数据
		youmecommon::CXSharedArray<char> recvBuffer;
		int num = udp.RecvData(recvBuffer);
		YouMe_LOG_Info(__XT("重定向 返回数据长度:%d"), num);
		if (num > 0)
		{
			YouMeIMProtocol::YouMeIM_Command_RedirectResponse response;
			if (!response.ParseFromArray(recvBuffer.Get(), num))
			{
				bParseRedirectSuccess = false;
				YouMe_LOG_Error(__XT("重定向 接受数据不能解析:%s"), GetConfigure<XString>(CConfigEntry::REDIRECT_ADDR, CConfigEntry::REDIRECT_ADDR_DEFAULT).c_str());
			}
			else
			{
				bParseRedirectSuccess = true;
				strServerIP = UTF8TOXString(response.ip());
				iPort = response.port();
			}			
		}
	}
	//测试用的代码
//	strServerIP = __XT("106.75.75.99 ");
	//iPort = 9999;
	XINT64 ulEndTime = youmecommon::CTimeUtil::GetTimeOfDay_MS() ;
	{
		//开始上报了
		std::vector<youmecommon::CXVariant> reportData;
		//0 表示成功
		if (strServerIP.empty())
		{
			if (!bParseRedirectSuccess)
			{
				reportData.push_back(YouMeIMErrorcode_ServerError);
			}
			else
			{
				reportData.push_back(YouMeIMErrorcode_NetError);
			}
		}
		else
		{
			reportData.push_back(0);
		}
		
		reportData.push_back(ulEndTime - ulStartTime);
		//g_pMonitorPtr->Report(DataReportEvtType_RedirectEnd, reportData, GetServerValidTime());
	}
	if (strServerIP.empty())
	{
		if (NULL != m_pLoginCallback)
		{
			if (!bParseRedirectSuccess)
			{
				m_pLoginCallback->OnLogin(YouMeIMErrorcode_ServerError, NULL);
			}
			else
			{
				m_pLoginCallback->OnLogin(YouMeIMErrorcode_NetError, NULL);
			}
		}
		m_logStatus = IMManangerLoginStatus_Not;
		return;
	}
	YouMe_LOG_Info(__XT("重定向 返回的信息:%s %d"),strServerIP.c_str(),iPort);

	//发送登陆的请求
	delete m_pUDPManager;
	m_pUDPManager = new CXUDP(this);
	m_pUDPManager->Init(strServerIP, iPort, this);
	//构造请求包
	YouMeIMProtocol::YouMeIM_Command_Login login;
	login.set_allocated_msghead(CreatePacketHead(YouMeIMProtocol::Command_Login));
	login.set_userpasswd(XStringToUTF8(youmecommon::CCryptUtil::MD5(XStringToUTF8(strPasswd))));
	if (strOldPasswd != __XT(""))
	{
		login.set_olduserpasswd(XStringToUTF8(youmecommon::CCryptUtil::MD5(XStringToUTF8(strOldPasswd))));
	}
	else
	{
		login.set_olduserpasswd("");
	}
	//序列化数据
	std::string strData;
	login.SerializeToString(&strData);
	std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_Login));
	m_pUDPManager->SendData(YouMeIMProtocol::Command_Login, strData.c_str(), strData.length(), login.msghead().msgserialnum());

	{
		std::lock_guard<std::mutex> lock(m_messageCacheMutex);
		m_messageMap[login.msghead().msgserialnum()] = pMessage;
	}

	YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMManager::OnRecvPacketTimeOut(XINT64 iUniqueID)
{
	YouMe_LOG_Warning(__XT("需要服务器回的包，但是没有回：%lld"), iUniqueID);

	MessageInfo messageInfo(YouMeIMProtocol::Command_Unknow);

	{
		std::lock_guard<std::mutex> lock(m_messageCacheMutex);
		std::map<XINT64, std::shared_ptr<MessageInfo> >::iterator it = m_messageMap.find(iUniqueID);
		if (it != m_messageMap.end())
		{
			messageInfo = *(it->second);
			YouMe_LOG_Warning(__XT("超时包信息，类型：%d"), messageInfo.command);
			m_messageMap.erase(it);
		}
	}
	
	if (messageInfo.command == YouMeIMProtocol::Command_Unknow)
	{
		bool bDeal = false;
		if (NULL != m_pContactManager)
		{
			bDeal = m_pContactManager->OnRecvPacketTimeOut(iUniqueID);
		}
		if (!bDeal && (NULL != m_pMessageManager))
		{
			bDeal = m_pMessageManager->OnRecvPacketTimeOut(iUniqueID);
		}
		if (!bDeal && (NULL != m_pGroupManager))
		{
			bDeal = m_pGroupManager->OnRecvPacketTimeOut(iUniqueID);
		}
		if (!bDeal)
		{
			YouMe_LOG_Warning(__XT("收到了没有处理的的超时包  serial:%lld "), iUniqueID);
		}
	}
	else
	{
		if (messageInfo.command == YouMeIMProtocol::Command_Login)
		{
			if (NULL != m_pLoginCallback)
			{
				m_pLoginCallback->OnLogin(YouMeIMErrorcode_TimeOut, NULL);
			}
			m_logStatus = IMManangerLoginStatus_Not;
		}
		else if (messageInfo.command == YouMeIMProtocol::Command_LogOut)
		{
			std::thread cleanup(&YouMeIMManager::DealLogoutCleanup, this);
			cleanup.detach();
		}
		else if (messageInfo.command == YouMeIMProtocol::Command_Heart)
        {
            //心跳不用管
        }
		else
		{
			YouMe_LOG_Warning(__XT("没有处理的需要服务器回复的超时包:%d"), messageInfo.command);
		}
	}
}

void YouMeIMManager::HeartBeatThreadProc()
{
	m_iLossPacketCount = 0;
	while (true)
	{
		if (youmecommon::WaitResult_Timeout != m_heartBeatWait.WaitTime(30000))
		{
			break;
		}
		if (m_iLossPacketCount >= HEART_TIMEOUT_COUNT)
		{
			//退出心跳，重新登陆
			YouMe_LOG_Info(__XT("心跳包有好多没收到，需要重连了"));
			ReconectProc();
			break;
		}
		YouMeIMProtocol::YouMeIM_Command_Heart request;
		request.set_allocated_msghead(CreatePacketHead(YouMeIMProtocol::Command_Heart));
		std::string strData;
		request.SerializeToString(&strData);
		m_pUDPManager->SendData(YouMeIMProtocol::Command_Heart,strData.c_str(), strData.length(), request.msghead().msgserialnum(),200000);
		m_iLossPacketCount++;
	}
	YouMe_LOG_Info(__XT("心跳线程退出"));
}

void YouMeIMManager::DealLogoutCleanup()
{
    //删掉udp 管理
    YouMe_LOG_Info(__XT("Enter"));
	assert(m_logStatus == IMManangerLoginStatus_Logouting);
	if (m_heartBeatThread.joinable())
	{
		m_heartBeatThread.join();
	}
    assert(NULL != m_pUDPManager);
    m_pUDPManager->UnInit();
    delete m_pUDPManager;
    m_pUDPManager = NULL;
	
    m_pContactCallback = NULL;
    m_pMessageCallback = NULL;
    m_pGroupCallback = NULL;
	m_logoutWait.SetSignal();
	if (NULL != m_pContactManager)
	{
		m_pContactManager->UnInit();
		delete m_pContactManager;
		m_pContactManager = NULL;
	}
	if (NULL != m_pMessageManager)
	{
		m_pMessageManager->UnInit();
		delete m_pMessageManager;
		m_pMessageManager = NULL;
	}
	if (NULL != m_pGroupManager)
	{
		m_pGroupManager->UnInit();
		delete m_pGroupManager;
		m_pGroupManager = NULL;
	}
	m_logStatus = IMManangerLoginStatus_Not;
	if (NULL != m_pLoginCallback)
	{
		m_pLoginCallback->OnLogout(YouMeIMErrorcode_Success);
	}
	m_pLoginCallback = NULL;
    YouMe_LOG_Info(__XT("Leave"));
}

YouMeIMManager::~YouMeIMManager()
{
	UnInit();
}

void YouMeIMManager::onNetWorkChanged(youmecommon::NetworkType type)
{
	YouMe_LOG_Info(__XT("Enter NetworkType:%d"), type);
}

void YouMeIMManager::EncryDataXOR(char* pBuffer, int iBufferLen)
{
	if (m_pSecretbuffer.GetBufferLen() == 0)
	{
		return;
	}
	int j = 0;
	for (int i = 0; i < iBufferLen;i++)
	{
		pBuffer[i] ^= m_pSecretbuffer.Get()[j % m_pSecretbuffer.GetBufferLen()];
		j++;
	}
}

YouMeIMErrorcode YouMeIMManager::ParseRSASecret(const char*pBuffer, int iBufferSize, YouMeIMProtocol::YouMeIM_Command_LoginResponse& response)
{
	if (!response.ParseFromArray(pBuffer, iBufferSize))
	{
		return YouMeIMErrorcode_ServerError;
	}
	YouMeIMErrorcode errorcode = (YouMeIMErrorcode) response.msghead().code();
	//需要使用RSA 解密一个密钥
	youmecommon::CRSAUtil rsa;
	do
	{
		youmecommon::CXSharedArray<char> pDecodeBuffer;
		if (!youmecommon::CCryptUtil::Base64Decoder(XStringToUTF8(CYMApplication::GetInstance().GetAppSecret()), pDecodeBuffer))
		{
			errorcode = YouMeIMErrorcode_ServerError;
			break;
		}
		if (131 != pDecodeBuffer.GetBufferLen())
		{
			errorcode = YouMeIMErrorcode_ServerError;
			break;
		}
		youmecommon::CXSharedArray<unsigned char> publicKey;
		youmecommon::CXSharedArray<unsigned char>  module;

		//模数
		module.Allocate(128);
		memcpy(module.Get(), pDecodeBuffer.Get(), 128);

		// publickey
		publicKey.Allocate(3);
		memcpy(publicKey.Get(), pDecodeBuffer.Get() + 128, 3);

		bool bParseSuccess = rsa.SetPublicKey(publicKey, module);
		if (!bParseSuccess)
		{
			YouMe_LOG_Error(__XT("不可能到这里的，SDK 验证是怎么通过的。"));
			break;
		}
		if (!rsa.DecryptByPublicKey((const unsigned char *)response.secrect().c_str(), response.secrect().length(), m_pSecretbuffer))
		{
			YouMe_LOG_Error(__XT("登陆回复 解密失败"));
			errorcode = YouMeIMErrorcode_ServerError;
			break;
		}
		if (m_pSecretbuffer.GetBufferLen() <= 0)
		{
			YouMe_LOG_Error(__XT("登陆回复 解密内存失败，居然没有"));
			errorcode = YouMeIMErrorcode_ServerError;
			break;
		}
	} while (0);
	return errorcode;
}

XINT64 YouMeIMManager::GetServerValidTime()
{
	XINT64 ulCurTime = youmecommon::CTimeUtil::GetTimeOfDay_MS() / 1000;
	XINT64 ulServerTime = GetConfigure(CConfigEntry::VERIFY_TIME, (unsigned int)ulCurTime);
	return ulServerTime + ulCurTime - m_localTime;
}

void YouMeIMManager::ReconnectThread()
{
	//开始重定向
	bool bParseRedirectSuccess = true;
	XString strServerIP;
	int iPort = -1;
	//发送重定向请求
	for (int i = 0; i < RECONNECT_COUNT;i++)
	{
		YouMeIMProtocol::YouMeIM_Command_Redirect request;
		request.set_allocated_msghead(CreatePacketHead(YouMeIMProtocol::Command_Redirect));
		std::string strData;
		request.SerializeToString(&strData);
		youmecommon::CSyncTCP udp;

		udp.Init(GetConfigure<XString>(CConfigEntry::REDIRECT_ADDR, CConfigEntry::REDIRECT_ADDR_DEFAULT),
			GetConfigure<int>(CConfigEntry::REDIRECT_PORT, CConfigEntry::REDIRECT_PORT_DEFAULT));
		udp.Connect(3000);
		udp.SendData(strData.c_str(), (int)strData.length());

		//接受数据
		youmecommon::CXSharedArray<char> recvBuffer;
		int num = udp.RecvData(recvBuffer);
		YouMe_LOG_Info(__XT("重连过程中 重定向 返回数据长度:%d"), num);
		if (num > 0)
		{
			YouMeIMProtocol::YouMeIM_Command_RedirectResponse response;
			if (!response.ParseFromArray(recvBuffer.Get(), num))
			{
				bParseRedirectSuccess = false;
				YouMe_LOG_Error(__XT("重定向 接受数据不能解析:%s %d"), GetConfigure<XString>(CConfigEntry::REDIRECT_ADDR, CConfigEntry::REDIRECT_ADDR_DEFAULT).c_str());
			}
			else
			{
				bParseRedirectSuccess = true;
				strServerIP = UTF8TOXString(response.ip());
				iPort = response.port();
				break;
			}
		}
		m_reconnectTimeout.WaitTime(RECONNECT_TIMEOUT);
	}
	if (strServerIP.empty())
	{
		//启动一个线程，用来调用退出,把状态先改成成功，走正常的登出流程
		m_logStatus = IMManangerLoginStatus_Success;
		std::thread logoutThread(&YouMeIMManager::Logout, this);
		logoutThread.detach();
		
		YouMe_LOG_Info(__XT("重连失败了 重定向 返回的信息:%s %d"), strServerIP.c_str(), iPort);
		return;
	}
	YouMe_LOG_Info(__XT("重连过程中 重定向 返回的信息:%s %d"), strServerIP.c_str(), iPort);

	//发送登陆的请求
	delete m_pUDPManager;
	m_pUDPManager = new CXUDP(this);
	m_pUDPManager->Init(strServerIP, iPort, this);
	//构造请求包
	YouMeIMProtocol::YouMeIM_Command_Login login;
	login.set_allocated_msghead(CreatePacketHead(YouMeIMProtocol::Command_Login));
	login.set_userpasswd(XStringToUTF8(youmecommon::CCryptUtil::MD5(XStringToUTF8(m_strPasswd))));
	if (m_strOldPasswd != __XT(""))
	{
		login.set_olduserpasswd(XStringToUTF8(youmecommon::CCryptUtil::MD5(XStringToUTF8(m_strOldPasswd))));
	}
	else
	{
		login.set_olduserpasswd("");
	}
	//序列化数据
	std::string strData;
	login.SerializeToString(&strData);
	std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_Login));
	m_pUDPManager->SendData(YouMeIMProtocol::Command_Login, strData.c_str(), strData.length(), login.msghead().msgserialnum());

	{
		std::lock_guard<std::mutex> lock(m_messageCacheMutex);
		m_messageMap[login.msghead().msgserialnum()] = pMessage;
	}
	
	YouMe_LOG_Info(__XT("Leave"));
}
void YouMeIMManager::SetDownloadCallback(IYouMeIMDownloadCallback* pCallback)
{
	m_pDownloadCallback = pCallback;
	if (NULL != m_pMessageManager)
	{
		m_pMessageManager->SetDownloadCallback(m_pDownloadCallback);
	}
}

void YouMeIMManager::ReconectProc()
{
	if (m_logStatus == IMManangerLoginStatus_Reconnecting)
	{
		return;
	}
	m_heartBeatWait.SetSignal();
	m_logStatus = IMManangerLoginStatus_Reconnecting;
	if (m_reconnectThread.joinable())
	{
		m_reconnectThread.join();
	}
	m_reconnectThread = std::thread(&YouMeIMManager::ReconnectThread, this);
}

void YouMeIMManager::DownloadKeyword(const XString& strURL, const XString& strMD5)
{
	XString strSavePath = youmecommon::CXFile::CombinePath(CYMApplication::GetInstance().GetDocumentDir(), __XT("keyword.zip"));
	XString srtFileMD5 = youmecommon::CCryptUtil::MD5File(strSavePath);
	XString strXMLPath = youmecommon::CXFile::CombinePath(CYMApplication::GetInstance().GetDocumentDir(), __XT("keyword.xml"));

	if (0 != CStringUtil::compare_nocase(strMD5.c_str(), srtFileMD5.c_str()))
	{
		YouMe_LOG_Info(__XT("download keyword config"));
		if (!CDownloadUploadManager::DownloadFile(strURL, strSavePath))
		{
			YouMe_LOG_Error(__XT("下载keyword 失败::%s"), strURL.c_str());
			return;
		}		
		youmecommon::CXFile::remove_file(strXMLPath);
	}
	if (!youmecommon::CXFile::IsFileExist(strXMLPath))
	{
		//解压
		CMiniUnZip unzip;
		if (!unzip.Open(strSavePath))
		{
			YouMe_LOG_Error(__XT("打开keyword失败::%s"), strSavePath.c_str());
			return;
		}
		if (!unzip.UnZipToDir(CYMApplication::GetInstance().GetDocumentDir()))
		{
			YouMe_LOG_Error(__XT("打开keyword失败::%s"), strSavePath.c_str());
			return;
		}
	}
	YouMe_LOG_Info(__XT("load keyword config md5:%s"), srtFileMD5.c_str());
	g_pKeywordFilter->LoadFile(strXMLPath);
}

