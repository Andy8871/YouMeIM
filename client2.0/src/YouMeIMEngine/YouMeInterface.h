#pragma once
#ifdef WIN32
    #ifdef YOUMEDLL_EXPORTS
        #define YOUMEDLL_API __declspec(dllexport)
    #else
        #define YOUMEDLL_API __declspec(dllimport)
    #endif
#else
    #define YOUMEDLL_API 
#endif


//对外的错误码
enum YouMeIMErrorcode
{
	//这上面是服务器的，要和服务器的一致，如果这里没有的话，会强制转换成一个数字
	YouMeIMErrorcode_Success = 0,
	YouMeIMErrorcode_EngineNotInit = 1,
	YouMeIMErrorcode_NotLogin = 2,	
	YouMeIMErrorcode_ParamInvalid = 3,
	YouMeIMErrorcode_TimeOut = 4,
	YouMeIMErrorcode_StatusError = 5,
	YouMeIMErrorcode_SDKInvalid = 6,
	YouMeIMErrorcode_AlreadyLogin = 7,
	YouMeIMErrorcode_ServerError = 8,
	YouMeIMErrorcode_NetError = 9,
	YouMeIMErrorcode_LoginSessionError = 10,
	YouMeIMErrorcode_NotStartUp = 11,
	YouMeIMErrorcode_FileNotExist = 12,
	YouMeIMErrorcode_SendFileError = 13,
	YouMeIMErrorcode_UploadFailed = 14,

	//服务器的错误码
	YouMeIMErrorcode_ALREADYFRIENDS = 1000,
	YouMeIMErrorcode_LoginInvalid = 1001,

    //语音部分错误码
    YouMeIMErrorcode_PTT_Start = 2000,
    YouMeIMErrorcode_PTT_Fail = 2001,
    YouMeIMErrorcode_PTT_DownloadFail = 2002,
	YouMeIMErrorcode_PTT_GetUploadTokenFail = 2003,
	YouMeIMErrorcode_PTT_UploadFail = 2004,
    YouMeIMErrorcode_PTT_NotSpeech = 2005,
	YouMeIMErrorcode_Fail = 10000
};




/************************************************************************/
/* 联系人相关                                                                     */
/***********************************************************************/

enum ContactStatus
{
	ContactStatus_Online = 0,
	ContactStatus_Offline = 1
};


//添加好友回复事件
enum YouMeIMContactEvent
{
	AddContactEvent_Agree = 0,
	AddContactEvent_Refuse = 1
};


//具体的联系人的信息
class IYouMeIMContactInfo
{
public:
	//获取id
	virtual XString GeYouMeID() = 0;
	//获取在线状态
	virtual int GetStatus() = 0;
};


//联系人回掉接口
class IYouMeIMContactCallback
{
public:
	//获取联系人信息回掉, 回调的指针不能保存，回调完之后指针可能失效
	virtual void OnGetContactInfo(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) = 0;
	//获取联系人列表，返回的用户id 回调的指针不能保存，回调完之后指针可能失效
	virtual void OnGetContactList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) = 0;
	//获取黑名单列表
	virtual void OnGetBlacklistList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) = 0;

	//添加好友
	//添加好友请求
	virtual void OnAddContactRequest(YouMeIMErrorcode errorcode, const XString& strYouMeID) = 0;
	//添加好友回复
	virtual void OnAddContactResponse(YouMeIMContactEvent evtType, IYouMeIMContactInfo* pContactInfo) = 0;
	//接收到好友申请
	virtual void OnReceiveContactInvited(IYouMeIMContactInfo*pContactInfo, const XString& strReason) = 0;
	//接收或者拒绝好友请求回掉 
	virtual void OnContactInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMContactEvent evtType, const XString&strYouMeID) = 0;


	//被动接受的删除，黑名单消息
	virtual void OnContactBeDeleted(const XString& strYouMeID) = 0;
	virtual void OnContactAddedBlacklist(const XString& strYouMeID) = 0;
	virtual void OnKickoutedBlacklist(const XString& strYouMeID) = 0;

	//主动发送的消息回掉
	virtual void OnDeleteContactResponse(YouMeIMErrorcode errorcode, const XString& strYouMeID) = 0;
	virtual void OnAddBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strYouMeID) = 0;
	virtual void OnKickoutBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strYouMeID) = 0;

	//好友登陆，登出回调
	virtual void OnContactOnline(const XString& strYouMeID) = 0;
	virtual void OnContactOffline(const XString& strYouMeID) = 0;
};
//联系人管理接口
class IYouMeIMContactManager
{
public:
    virtual ~IYouMeIMContactManager();
	//获取好友列表
	virtual YouMeIMErrorcode GetContactList() = 0;
	//获取黑名单列表
	virtual YouMeIMErrorcode GetBlacklistList() = 0;
	//添加好友
	virtual YouMeIMErrorcode AddContact(const XString& strYouMeID, const XString& strReason) = 0;
	//同意好友请求
	virtual YouMeIMErrorcode AgreeContactInvited(const XString& strYouMeID) = 0;
	//拒绝好友请求
	virtual YouMeIMErrorcode RefuseContactInvited(const XString& strYouMeID, const XString& strReason) = 0;
	//删除好友
	virtual YouMeIMErrorcode DeleteContact(const XString&strYouMeID) = 0;
	//添加到黑名单
	virtual YouMeIMErrorcode AddToBlacklist(const XString& strYouMeID) = 0;
	//踢出黑名单
	virtual YouMeIMErrorcode KickoutBlacklist(const XString&strYouMeID) = 0;
	//获取联系人信息
	virtual YouMeIMErrorcode GetContactInfo(const std::list<XString>& contactIDList) = 0;
};



/************************************************************************/
/*                              登录，连接接口                          */
/************************************************************************/
//登陆回掉
class IYouMeIMLoginCallback
{
public:
	virtual void OnLogin(YouMeIMErrorcode errorcode, IYouMeIMContactInfo* pUserInfo) = 0;
	virtual void OnLogout(YouMeIMErrorcode errorcode) = 0;
};

//
enum YouMeIMConnectEventType
{
	//账号被删除
	YouMeIMConnectEventType_AcountDeleted = 0,
	//账号被踢下线了
	YouMeIMConnectEventType_AcountKickout = 1,
	//网络断开了
	YouMeIMConnectEventType_NetInvalid = 100
};


/************************************************************************/
/*                                群组   或者聊天室                               */
/************************************************************************/


enum YouMeIMGroupEvent
{
	YouMeIMGroupEvent_Agree = 0,
	YouMeIMGroupEvent_Refuse = 1,
};

enum YouMeIMGroupPermission
{
	//啥都木有
	GroupPermission_None = 0,

	//普通成员
	GroupPermission_Common = 1,
	GroupPermission_AddMember = 2,
	GroupPermission_Kickout = 4,

	//超级权限，啥都有，32位都是1
	GroupPermission_All = -1
};

enum ServerZone
{
	ServerZone_China = 0,		// 中国
	ServerZone_Singapore = 1,	// 新加坡
	ServerZone_America = 2,		// 美国
	ServerZone_HongKong = 3,	// 香港
	ServerZone_Korea = 4,		// 韩国
	ServerZone_Australia = 5,	// 澳洲
	ServerZone_Deutschland = 6,	// 德国
	ServerZone_Brazil = 7,		// 巴西
	ServerZone_India = 8,		// 印度
	ServerZone_Japan = 9,		// 日本
	ServerZone_Ireland = 10,	// 爱尔兰
};

class IYouMeIMGroupMemberInfo
{
public:
	virtual ~IYouMeIMGroupMemberInfo();
	//成员的ID
	virtual XString GetYouMeID() = 0;
	virtual YouMeIMGroupPermission GetMemberPermission() = 0;
	virtual ContactStatus GetStatus() = 0;
};
	
//群类型

//群类型
enum YouMeIMGroupType
{
	//普通群
	GroupType_Group = 0,
	//聊天室
	GroupType_ChatRoom = 1
};


class IYouMeIMGroupInfo
{
public:
	virtual XString GetGroupID() = 0;
	virtual YouMeIMGroupType GetGroupType() = 0;
	virtual XString GetGroupName() = 0;
};

//群组以及聊天室回掉
class IYouMeIMGroupCallback
{
public:
	virtual void OnGetAllGroup(YouMeIMErrorcode errorcode, const std::list<IYouMeIMGroupInfo*>* pGroupInfoList) = 0;
	virtual void OnCreateGroup(YouMeIMErrorcode errorcode, XINT64 iRequestID,IYouMeIMGroupInfo* pGroupInfo) = 0;
	virtual void OnDestroyGroup(YouMeIMErrorcode errorcode, const XString& strGroupID) = 0;
	virtual void OnGroupDestroyed(const XString& strGroupID) = 0;
	virtual void OnGetGroupMembers(YouMeIMErrorcode errorcode, const XString& strGroupID, const std::list<IYouMeIMGroupMemberInfo*>* pGroupMembers) = 0;
	virtual void OnJoinGroupRequest(YouMeIMErrorcode errorcode, const XString& strGroupID) = 0;
	virtual void OnJoinGroupResponse(YouMeIMErrorcode errorcode,YouMeIMGroupEvent evtType, IYouMeIMGroupInfo* pGroupInfo) = 0;
	virtual void OnReceiveGroupInvited(const XString&strGroupID, IYouMeIMContactInfo* pContact, const XString& strReason) = 0;
	//给服务器发送了同意或者拒绝，服务器返回的
	virtual void OnGroupInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMGroupEvent evtType, const XString&strGroupID, const XString& strYouMeID) = 0;
	//有其他成员加入到群
	virtual void OnOtherJoinGroup(const XString&strGroupID, IYouMeIMGroupMemberInfo* pMemberInfo) = 0;
	//踢出成员回调
	virtual void OnKickMemberResponse(YouMeIMErrorcode errorcode, const XString&strGroupID, const XString&strYouMeID) = 0;
	virtual void OnOtherKickedGroup(const XString&strGroupID, const XString& strYouMeID) = 0;
	//离开分组
	virtual void OnLeaveGroupResponse(YouMeIMErrorcode errorcode, const XString&strGroupID) = 0;
	virtual void OnOtherLeaveGroup(const XString&strGroupID, const XString& strYouMeID) = 0;
};
class IYouMeIMGroupManager
{
public:
    virtual ~IYouMeIMGroupManager();
	
	//群方法
	virtual YouMeIMErrorcode GetAllGroup() = 0;
	virtual YouMeIMErrorcode CreateGroup(const XString&strGroupName, XINT64 *iRequestID) = 0;
	virtual YouMeIMErrorcode DestroyGroup(const XString& strGroupID) = 0;
	virtual YouMeIMErrorcode GetGroupMembers(const XString& strGroupID) = 0;
	virtual YouMeIMErrorcode JoinGroup(const XString& strGroupID, const XString&strReason) = 0;
	virtual YouMeIMErrorcode AgreeGroupInvited(const XString& strGroupID, const XString&strYouMeID) = 0;
	virtual YouMeIMErrorcode RefuseGroupInvited(const XString& strGroupID, const XString&strYouMeID, const XString& strReason) = 0;
	virtual YouMeIMErrorcode KickoutMember(const XString& strGroupID, const XString&strYouMeID) = 0;
	virtual YouMeIMErrorcode LeaveGroup(const XString&strGroupID) = 0;

	//聊天室方法
	virtual YouMeIMErrorcode JoinChatRoom(const XString& strChatRoomID) = 0;
	virtual YouMeIMErrorcode LeaveChatRoom(const XString& strChatRoomID) = 0;
};


/************************************************************************/
/*                         消息部分接口                                                                     */
/************************************************************************/

enum YouMeIMChatType
{
	ChatType_Unknow = 0,
	ChatType_PrivateChat = 1,
	ChatType_GroupChat = 2,
	ChatType_RoomChat = 3,
};

enum YouMeIMMessageBodyType
{
	MessageBodyType_Unknow = 0,
	MessageBodyType_TXT = 1,
	MessageBodyType_CustomMesssage = 2,
    MessageBodyType_Emoji = 3,
    MessageBodyType_Image = 4,
    MessageBodyType_Voice = 5,
    MessageBodyType_Video = 6,
    MessageBodyType_File = 7
};

// 发送文件类型
enum FileType
{
	FileType_Other = 0,
	FileType_Audio = 1,
	FileType_Image = 2,
	FileType_Video = 3
};

//所有消息的基类
class IYouMeIMMessageBodyBase
{
public:
	virtual ~IYouMeIMMessageBodyBase();
	virtual YouMeIMMessageBodyType GetMessageType() = 0;
};



//自定义消息
class IYouMeIMMessageBodyCustom :public IYouMeIMMessageBodyBase
{
public:
	//strCustomMessage 可以包含0 终止符。
	/************************************************************************/
	/*  char buffer[20] = {0};
		buffer[10] = 1;
		buffer[15] = 2;
		std::string strMessage;
		strMessage.append(buffer, 20);//strMessage 内容长度20 . 包含若干0
	*/
	/************************************************************************/
	virtual std::string GetCustomMessage() = 0;
};

//文本消息
class IYouMeIMMessageBodyText :public IYouMeIMMessageBodyBase
{
public:
	virtual XString GetMessageContent() = 0;
};

//语音消息
class IYouMeIMMessageBodyAudio :public IYouMeIMMessageBodyBase
{
public:
    virtual XString GetText() = 0;
	virtual XString GetParam() = 0;
	virtual unsigned int GetFileSize() = 0;
	virtual unsigned int GetAudioTime() = 0;	// 单位：秒
};

//文件消息
class IYouMeIMMessageBodyFile :public IYouMeIMMessageBodyBase
{
public:
	virtual unsigned int GetFileSize() = 0;
	virtual XString GetFileName() = 0;
	virtual XString GetFileExtension() = 0;
	virtual FileType GetFileType() = 0;
	virtual XString GetParam() = 0;
};

//一个完整的消息，包括接收者，聊天类型，消息体等
class IYouMeIMMessage
{
public:
	virtual ~IYouMeIMMessage();
	//聊天类型
	virtual YouMeIMChatType GetChatType() = 0;
	//接收者
	virtual XString GetReceiveID() = 0;
	//群组ID(空：私聊	非空：群组 聊天室)
	virtual XString GetGroupID() = 0;
	//发送者
	virtual XString GetSenderID() = 0;
	//消息体
	virtual IYouMeIMMessageBodyBase* GetMessageBody() = 0;
    
    virtual XINT64 GetMessageID() = 0;
};

class IYouMeIMMessageCallback
{
public:
	//发送消息状态
	virtual void OnSendMessageStatus(XINT64 iRequestID, YouMeIMErrorcode errorcode) = 0;
	//接收到用户发来的消息
	virtual void OnRecvMessage(IYouMeIMMessage* pMessage) = 0;
    
    //语音消息的回掉
    virtual void OnSendAudioMessageStatus(XINT64 iRequestID,YouMeIMErrorcode errorcode,const XString&strText,const XString& strAudioPath, unsigned int audioTime) = 0;
};

class IYouMeIMDownloadCallback
{
public:
	virtual void OnDownload(XINT64 ulSerial, YouMeIMErrorcode errorcode,const XString& strSavePath) = 0;

};

class IYouMeIMMessageManager
{
public:
    virtual ~IYouMeIMMessageManager();
	//获取离线消息
	virtual YouMeIMErrorcode GetContactOfflineMessage() = 0;
	virtual YouMeIMErrorcode GetGroupOfflineMessage(const XString& strGroupID) = 0;
	//一些更简单的接口，用来发送文本
	virtual YouMeIMErrorcode SendTextMessage(const XString&strRecvID, YouMeIMChatType chatType, const XString& strContent, XINT64 *pRequestID) = 0;
	virtual YouMeIMErrorcode SendCustomMessage(const XString&strRecvID, YouMeIMChatType chatType, const std::string& strCustomMessage, XINT64 *pRequestID) = 0;
	virtual YouMeIMErrorcode SendFile(const XString&strRecvID, YouMeIMChatType chatType, const XString& strPath, XINT64 *pRequestID, const XString& strParam, FileType fileType = FileType_Other) = 0;
    
    //发送语音消息
	virtual YouMeIMErrorcode SendAudioMessage(const XString&strRecvID, YouMeIMChatType chatType, XINT64 *pRequestID) = 0;
	//发送语音消息，不会转文本
	virtual YouMeIMErrorcode SendOnlyAudioMessage(const XString&strRecvID, YouMeIMChatType chatType,  XINT64 *pRequestID) = 0;
	virtual YouMeIMErrorcode StopAudioMessage(const XString& strParam) = 0;
    virtual YouMeIMErrorcode CancleAudioMessage() =0;
	virtual YouMeIMErrorcode DownloadAudio(XINT64 ulSerial, const XString& strSavePath) = 0;
};


class YOUMEDLL_API IYouMeIMManager
{
public:
	//这些函数不需要任何初始化都可以调用
	static int GetSDKVersion();
	//设置使用的服务器地区
	static void SetServerZone(ServerZone zone);
    static void SetMode(int iMode);
	static void SetAudioCacheDir(const XString& path);
    
	//程序开始调用一次，结束调用一次，用来初始化一些进程相关信息，比如winsock 库等
	static void Startup();
	static void CleanUp();
	//创建实例
	static IYouMeIMManager* CreateInstance();
	//关键词过滤
	static XString FilterKeyword(const XString& strSource);

	static void DestroyInstance(IYouMeIMManager* pInsance);
	virtual ~IYouMeIMManager();
	//初始化和反初始化, 只有windows 需要传入包名，可以认为是应用的唯一标识。类似andriod 和ios
	virtual YouMeIMErrorcode Init(const XString& strAppKey, const XString& strAppSecurity,const XString&strPackageName) = 0;
	virtual void UnInit() = 0;
    
	//登陆和登出
	virtual void SetLoginCallback(IYouMeIMLoginCallback* pCallback) = 0;
    //设置好友回掉
    virtual void SetContactCallback(IYouMeIMContactCallback* pCallback) = 0;
    //设置消息回调
    virtual void SetMessageCallback(IYouMeIMMessageCallback* pCallback) = 0;
    //设置群组回调
    virtual void SetGroupCallback(IYouMeIMGroupCallback* pCallback) = 0;
	virtual void SetDownloadCallback(IYouMeIMDownloadCallback* pCallback) =0;
	virtual YouMeIMErrorcode Login(const XString& strYouMeID, const XString&strPasswd, const XString& strOldPasswd) = 0;
	virtual YouMeIMErrorcode Logout()=0;


	//获取各种管理器
	virtual IYouMeIMContactManager* GetContactManager() = 0;
	virtual IYouMeIMGroupManager* GetGroupManager() = 0;
	virtual IYouMeIMMessageManager* GetMessageManager() = 0;

	virtual void SetUserLevle(int level) = 0;
};
