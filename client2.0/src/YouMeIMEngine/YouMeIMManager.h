// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "YouMeIMCommonDef.h"
#include <YouMeIMEngine/pb/im_common.pb.h>
#include <YouMeIMEngine/pb/im_contact.pb.h>
#include <YouMeIMEngine/XUDP.h>
#include <YouMeIMEngine/pb/im_redirect.pb.h>
#include <YouMeIMEngine/YouMeIMContactManager.h>
#include <YouMeIMEngine/YouMeIMMessageManager.h>
#include <YouMeIMEngine/YouMeIMGroupManager.h>
#include <YouMeCommon//XAny.h>
#include <thread>
#include <mutex>
#include <YouMeCommon/Log.h>
#include <YouMeCommon/NetworkService.h>
#include <YouMeCommon/TimeUtil.h>
#include <YouMeCommon/XCondWait.h>
//数据上报的事件类型
enum DataReportEvtType
{
	DataReportEvtType_SDKValidEnd = 1000,
	DataReportEvtType_RedirectEnd
};


class YouMeIMContactManager;
//引擎状态
enum IMManagerStatus
{
	IMManagerStatus_NotInit,//没有初始化
	IMManagerStatus_Ing,//正在初始化
	IMManagerStatus_Init,//已经初始化
	IMManagerStatus_uniniting,//正在反初始化
	IMManagerStatus_initfailed //初始化失败了
};

enum IMManangerLoginStatus
{
	IMManangerLoginStatus_Not,//没有登陆
	IMManangerLoginStatus_Ing,//正在登陆
	IMManangerLoginStatus_Success,//登陆完成
	IMManangerLoginStatus_Logouting,//正在登出
	IMManangerLoginStatus_Reconnecting,//正在登出
};

struct ServerZoneInfo
{
	XString strSDKValidateURL;
	XString strSDKValidateIP;

	/*
	XString strDataReportURL;
	XString strDataReportIP;*/
};


class YouMeIMManager :public IYouMeIMManager,
	public IPacketCallback,
	public youmecommon::INgnNetworkChangCallback
{
	friend class CXUDP;
	friend class YouMeIMContactManager;
	friend class YouMeIMMessageManager;
	friend class YouMeIMGroupManager;
public:
	~YouMeIMManager();
	YouMeIMManager()
	{
		m_iLossPacketCount = 0;
		m_localTime = youmecommon::CTimeUtil::GetTimeOfDay_MS() / 1000;
		s_iPacketSerial = 10000;
		m_pContactManager = NULL;
		m_pMessageManager = NULL;
		m_pGroupManager = NULL;
		m_logStatus = IMManangerLoginStatus_Not;
		m_pLoginCallback = NULL;
        m_pGroupCallback = NULL;
        m_pContactCallback = NULL;
        m_pMessageCallback = NULL;
		m_pUDPManager = NULL;
		m_initStatus = IMManagerStatus_NotInit;
		m_engineInitErrorcode = YouMeIMErrorcode_Success;
		m_iUserLevel = 0;
		if (m_allServerInfo.size() == 0)
		{
			//为了方便，这里按照ServerZone 定义顺序来插入
			//SDK域名格式：xx.im.youme.im
			//日志上报格式：xx.imlog.youme.im
			ServerZoneInfo info;
			// 中国		cn
			info.strSDKValidateURL = __XT("cn.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
			//info.strDataReportURL = __XT("cn.imlog.youme.im");
			//info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 新加坡	sg
			info.strSDKValidateURL = __XT("sg.im.youme.im");
			info.strSDKValidateIP = __XT("103.218.243.126");
			//info.strDataReportURL = __XT("sg.imlog.youme.im");
			//info.strDataReportIP = __XT("103.218.243.125");
			m_allServerInfo.push_back(info);
			// 美国		us
			info.strSDKValidateURL = __XT("us.im.youme.im");
			info.strSDKValidateIP = __XT("107.150.109.205");
			//info.strDataReportURL = __XT("us.imlog.youme.im");
			//info.strDataReportIP = __XT("107.150.109.204");
			m_allServerInfo.push_back(info);
			// 香港		hk
			info.strSDKValidateURL = __XT("hk.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
			//info.strDataReportURL = __XT("hk.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 韩国		kr
			info.strSDKValidateURL = __XT("kr.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
		//	info.strDataReportURL = __XT("kr.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 澳洲		au
			info.strSDKValidateURL = __XT("au.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
		//	info.strDataReportURL = __XT("au.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 德国		de
			info.strSDKValidateURL = __XT("de.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
		//	info.strDataReportURL = __XT("de.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 巴西		br
			info.strSDKValidateURL = __XT("br.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
		//	info.strDataReportURL = __XT("br.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 印度		in
			info.strSDKValidateURL = __XT("in.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
		//	info.strDataReportURL = __XT("in.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 日本		jp
			info.strSDKValidateURL = __XT("jp.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
		//	info.strDataReportURL = __XT("jp.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
			// 爱尔兰	ie
			info.strSDKValidateURL = __XT("ie.im.youme.im");
			info.strSDKValidateIP = __XT("106.75.25.67");
		//	info.strDataReportURL = __XT("ie.imlog.youme.im");
		//	info.strDataReportIP = __XT("106.75.35.102");
			m_allServerInfo.push_back(info);
		}
	}
	
    //初始化和反初始化
	YouMeIMErrorcode Init(const XString& strAppKey, const XString& strAppSecurity, const XString& strPackage) override;
	void UnInit()override;

	//登陆和登出
	void SetLoginCallback(IYouMeIMLoginCallback* pCallback)override;
    //设置好友回掉
    virtual void SetContactCallback(IYouMeIMContactCallback* pCallback) override;
    //设置消息回调
    virtual void SetMessageCallback(IYouMeIMMessageCallback* pCallback) override;
    //设置群组回调
    virtual void SetGroupCallback(IYouMeIMGroupCallback* pCallback) override;
    //设置下载回掉
	virtual void SetDownloadCallback(IYouMeIMDownloadCallback* pCallback) override;
    //如果带了旧密码说明是需要修改密码，则需要保证旧密码正确
	YouMeIMErrorcode Login(const XString& strYouMeID, const XString&strPasswd,const XString& strOldPasswd)override;
	YouMeIMErrorcode Logout()override;


	//获取好友管理器
	IYouMeIMContactManager* GetContactManager()override;
	IYouMeIMGroupManager* GetGroupManager()override;
	IYouMeIMMessageManager* GetMessageManager()override;

	void SetUserLevle(int level) override;


	//获取一个唯一的标识
	XINT64 GetUniqueSerial()
	{
		XINT64 iTmp = 0;
		{
			std::lock_guard<std::mutex> lock(m_serialMutex);
			iTmp = ++s_iPacketSerial;
		}
		return iTmp;
	}
	
	IMManangerLoginStatus GetLoginStatus(){ return m_logStatus; };

	//包回复的虚函数
	virtual void OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize) override;
	
	virtual void OnRecvPacketTimeOut(XINT64 iUniqueID) override;
private:
	//创建protobuf公共头
	YouMeIMProtocol::PacketHead* CreatePacketHead(YouMeIMProtocol::Command_TYPE command, int iProVer = 3, XINT64 ulServerSerial =0);


	//获取SDK下发的配置
	template <class T>
	T GetConfigure(const std::string& strKey,const T& defaultValue)
	{
		std::map<std::string, youmecommon::CXAny>::iterator it =  m_configurations.find(strKey);
		if (it != m_configurations.end())
		{
			return youmecommon::CXAny::XAny_Cast<T>(it->second);
		}
		return defaultValue;
	}

private:
	//解析rsa 生成的密码
	YouMeIMErrorcode ParseRSASecret(const char*pBuffer, int iBufferSize, YouMeIMProtocol::YouMeIM_Command_LoginResponse& response);


	//发送普通的包，只有一个包头的
	template <class T>
	YouMeIMErrorcode SendCommnadPacket(YouMeIMProtocol::Command_TYPE command, XINT64& ulSerial, XINT64 ulTimeout = 5000)
	{
		if (m_logStatus != IMManangerLoginStatus_Success)
		{
			YouMe_LOG_Warning(__XT("还没有登陆：status:%d errorcode:%d loginstatus:%d"), m_initStatus, m_engineInitErrorcode, m_logStatus); 
			return YouMeIMErrorcode_NotLogin; 
		}
		T request;
		request.set_allocated_msghead(CreatePacketHead(command));
		std::string strData;
		request.SerializeToString(&strData);
		m_pUDPManager->SendData(command,strData.c_str(), strData.length(), request.msghead().msgserialnum(), ulTimeout);
		ulSerial = request.msghead().msgserialnum();
		return YouMeIMErrorcode_Success;
	}
	//发送一个协议包，这种的需要先构造号结构体
	template <class T>
	YouMeIMErrorcode SendProtoPacket(const T& request, XINT64 ulTimeout = 20000)
	{
		if (m_logStatus != IMManangerLoginStatus_Success)
		{
			YouMe_LOG_Warning(__XT("还没有登陆：status:%d errorcode:%d loginstatus:%d"), m_initStatus, m_engineInitErrorcode, m_logStatus);
			return YouMeIMErrorcode_NotLogin;
		}
		std::string strData;
		request.SerializeToString(&strData);
		bool bRet = m_pUDPManager->SendData(request.msghead().msgtype(), strData.c_str(), strData.length(), request.msghead().msgserialnum(), ulTimeout);
		return bRet ? YouMeIMErrorcode_Success : YouMeIMErrorcode_Fail;
	}
	//获取一个和服务器对过时的时间
	XINT64 GetServerValidTime();
	//用户登陆的YMID
	XString m_strYouMeID;
	//保存密码，为了要重连
	XString m_strPasswd;
	XString m_strOldPasswd;

	//服务器返回的一个sessionid
	XString m_strSessionID;
    //是否允许使用移动网络,默认允许
    bool m_bAllow3G;

	//发包序列号
	static XINT64 s_iPacketSerial;
	static std::mutex m_serialMutex;

	//是否已经登陆
	IMManangerLoginStatus m_logStatus;
	//用户等级(用于禁言)
	unsigned int m_iUserLevel;

	//引擎初始化线程，包括SDK 验证，重定向， 验证相关的变量已经函数
	std::thread m_engInitThread;
	void EngineInitThreadProc();
	YouMeIMErrorcode m_engineInitErrorcode;
	IMManagerStatus m_initStatus;

	//登陆线程，其实主要是重定向，如果登陆的时候正在sdk 初始化，那就等待初始化完成
	std::thread m_loginThread;
	void LoginThread(const XString& strPasswd, const XString& strOldPasswd);
	void ReconnectThread();
	void ReconectProc();
	std::thread m_reconnectThread;
	youmecommon::CXCondWait m_reconnectTimeout;


	youmecommon::CXCondWait m_redirectWait;

	//心跳相关
	std::thread m_heartBeatThread;
	void HeartBeatThreadProc();
	youmecommon::CXCondWait m_heartBeatWait;
	int m_iLossPacketCount;
	//UDP 接收，发送包管理器
	CXUDP* m_pUDPManager;

	//登陆回掉指针
	IYouMeIMLoginCallback* m_pLoginCallback;
    IYouMeIMContactCallback* m_pContactCallback;
    IYouMeIMMessageCallback* m_pMessageCallback;
	IYouMeIMDownloadCallback* m_pDownloadCallback;
    IYouMeIMGroupCallback* m_pGroupCallback;
	//SDK 验证下发的配置
	std::map<std::string, youmecommon::CXAny> m_configurations;
    
    //处理登录的善后工作
	youmecommon::CXCondWait m_logoutWait;
    void DealLogoutCleanup();
    
	//SDK 验证的wait
	youmecommon::CXCondWait m_sdkValidWait;

	//获取管理器的锁
	std::mutex m_managerMutex;
	YouMeIMContactManager* m_pContactManager;
	YouMeIMMessageManager* m_pMessageManager;
	YouMeIMGroupManager* m_pGroupManager;
	//保存一个当前类发送的数据
	std::map<XINT64, std::shared_ptr<MessageInfo> > m_messageMap;
	std::mutex m_messageCacheMutex;
	virtual void onNetWorkChanged(youmecommon::NetworkType type) override;
	//本地的一个时间，用来和服务器对时,秒作为单位
	XINT64 m_localTime;
	//服务器返回的密钥
	youmecommon::CXSharedArray<unsigned char> m_pSecretbuffer;
	//void 异或加密算法,直接修改内存
	void EncryDataXOR(char* pBuffer, int iBufferLen);

	//下载加载关键词线程
	void DownloadKeyword(const XString& strURL, const XString& strMD5);
	//各个地区服务器地址和IP
	static std::vector<ServerZoneInfo> m_allServerInfo;
};
