// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/pb/im_common.pb.h>
#include <YouMeIMEngine/pb/im_message.pb.h>
#include "YouMeIMCommonDef.h"
#include <memory>
#include <map>
#include <mutex>
#include <YouMeIMEngine/Speech/SpeechListenManager.h>
#include <YouMeCommon/XSemaphore.h>
#include <thread>

enum FileFormat
{
	FileFormat_Unknow,
	FileFormat_WAV,
	FileFormat_AMR,
};

struct FileUploadInfo
{
	XINT64 ulSerial;
	XString strRecvID;
	YouMeIMChatType chatType;
	FileType fileType;
	XString strUrl;
	XString strLocalPath;
	unsigned int iFileSize;
	XString strMd5;
	XString strExtension;
	std::map<std::string, std::string> httpHead;
	std::map<std::string, youmecommon::CXAny> extendInfo;

	FileUploadInfo() : ulSerial(0), chatType(ChatType_Unknow), fileType(FileType_Other), iFileSize(0){}
};

struct FileDownloadInfo
{
	XINT64 ulSerial;
	XString strURL;
	XString strSavePath;
};


class CYouMeIMMessageBodyCustom :public BaseMessageBodyTypeT<IYouMeIMMessageBodyCustom>
{
	friend class YouMeIMMessageManager;
public:
	CYouMeIMMessageBodyCustom()
	{
		m_messageType = MessageBodyType_CustomMesssage;
	}
	void SetCustomMessage(const std::string& strCustomMessage);
	virtual std::string GetCustomMessage() override;
	
private:
	std::string m_strData;
	
};


class CYouMeIMMessageBodyText : public BaseMessageBodyTypeT<IYouMeIMMessageBodyText>
{
	friend class YouMeIMMessageManager;
public:
	CYouMeIMMessageBodyText()
	{
		m_messageType = MessageBodyType_TXT;
	}
	virtual XString GetMessageContent() override;
	void SetMessageContent(const XString&strContent);
private:
	XString m_strContent;
};

class CYouMeIMMessageBodyAudio: public BaseMessageBodyTypeT<IYouMeIMMessageBodyAudio>
{
    friend class YouMeIMMessageManager;
public:
    CYouMeIMMessageBodyAudio()
    {
        m_messageType = MessageBodyType_Voice;
		m_iFileSize = 0;
		m_iTime = 0;
    }
    virtual XString GetText() override;
	virtual XString GetParam() override;
	virtual unsigned int GetFileSize() override;
	virtual unsigned int GetAudioTime() override;
    void SetText(const XString& strText);
	void Play();
private:
    XString m_strText;
	XString m_strDownloadURL;
    XString m_strLocalPath;
	XString m_strParam;
	XString m_strMd5;
	XString m_strSuffix;
	unsigned int m_iFileSize;
	unsigned int m_iTime;
};

class CYouMeIMMessageBodyFile : public BaseMessageBodyTypeT<IYouMeIMMessageBodyFile>
{
	friend class YouMeIMMessageManager;
public:
	CYouMeIMMessageBodyFile()
	{
		m_messageType = MessageBodyType_File;
		m_iFileSize = 0;
	}

	virtual unsigned int GetFileSize() override;
	virtual XString GetFileName() override;
	virtual XString GetFileExtension() override;
	virtual FileType GetFileType() override;
	virtual XString GetParam() override;
private:
	XString m_strDownloadURL;
	XString m_strLocalPath;
	XString m_strMd5;
	XString m_strExtension;
	XString m_strFileName;
	XString m_strParam;
	FileType m_fileType;
	unsigned int m_iFileSize;
};

class CYouMeIMMessage :public IYouMeIMMessage
{
    friend class YouMeIMMessageManager;
public:
	CYouMeIMMessage();
	~CYouMeIMMessage();
	//聊天类型， 私聊，群组，聊天室
	virtual YouMeIMChatType GetChatType() override;
	void SetChatType(YouMeIMChatType chatType);
	//聊天对象， 可以是好友ID， 群ID
	virtual XString GetReceiveID() override;
	void SetReceiveID(const XString&strReceiverID);
	//消息体
	void SetMessageBody(IYouMeIMMessageBodyBase* pBody);
	virtual IYouMeIMMessageBodyBase* GetMessageBody() override;
	//这个只能是发送者的ID
	void SetSenderID(const XString& strSenderID);
	virtual XString GetSenderID() override;
	virtual XString GetGroupID() override;
	void SetGroupID(const XString& groupID);
    //获取消息ID
    virtual XINT64 GetMessageID() override;
private:
	YouMeIMChatType m_chatType;
	XString m_strReceiveID;
    XINT64 m_ulMsgSerial;
	XString m_strSenderid;
	XString m_strGroupID;
	IYouMeIMMessageBodyBase* m_pMessageBody;
};


class YouMeIMMessageManager:
	public IYouMeIMMessageManager,
	public IManagerBase,
    public ISpeechListen
{
	friend class YouMeIMManager;
    friend class CYouMeIMMessage;
public:
	YouMeIMMessageManager(YouMeIMManager* pIManager);
	YouMeIMErrorcode Init();
	void UnInit();
   //设置回掉
	void SetMessageCallback(IYouMeIMMessageCallback* pCallback);
	void SetDownloadCallback(IYouMeIMDownloadCallback* pCallback);
	//获取离线消息
	YouMeIMErrorcode GetContactOfflineMessage()override;
	YouMeIMErrorcode GetGroupOfflineMessage(const XString& strGroupID)override;
	//一些高级接口，直接调用就可以发送，
	YouMeIMErrorcode SendTextMessage(const XString&strRecvID, YouMeIMChatType chatType, const XString& strContent, XINT64 *pRequestID) override;
	YouMeIMErrorcode SendCustomMessage(const XString&strRecvID, YouMeIMChatType chatType, const std::string& strCustomMessage, XINT64 *pRequestID) override;
	YouMeIMErrorcode SendFile(const XString&strRecvID, YouMeIMChatType chatType, const XString& strPath, XINT64 *pRequestID, const XString& strParam, FileType fileType) override;
    
    //语音PTT
    YouMeIMErrorcode SendAudioMessage(const XString&strRecvID, YouMeIMChatType chatType,  XINT64 *pRequestID) override;
	YouMeIMErrorcode SendOnlyAudioMessage(const XString&strRecvID, YouMeIMChatType chatType, XINT64 *pRequestID) override;
	YouMeIMErrorcode StopAudioMessage(const XString& strParam) override;
    YouMeIMErrorcode CancleAudioMessage() override;
	YouMeIMErrorcode DownloadAudio(XINT64 ulSerial,const XString& strSavePath) override;
	//同步下载语音文件
	YouMeIMErrorcode DownloadAudioSync(XINT64 ulSerial, const XString& strSavePath) ;
    //语音PTT 的回调
    void OnSpeechInit(int iErrorcode) override;
    void OnSpeechResult(XINT64 ulSerial,int iErrorcode, const XString& strResult, const XString& strWavPath) override;
    
private:
	FileFormat GetFileFormat(const XString& strFilePath);
	YouMeIMErrorcode InterGetOfflineMsg(const XString& strYouMeGroupID, YouMeIMProtocol::YouMeIM_ChatType chatType);
	CYouMeIMMessage* CreateMessage(YouMeIMChatType chatType);
    //一般情况下都是 函数返回序号，但是语音消息需要传递序号进去
	YouMeIMErrorcode SendMessage(IYouMeIMMessage* pMessage, XINT64 *pRequestID,XINT64 ulPassSerial=0);
	
	//下面两个函数用来处理上一个管理器没有处理的包
	virtual bool OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize)override;
	virtual bool OnRecvPacketTimeOut(XINT64 iUniqueID) override;
	//上传文件
	std::thread m_uploadThread;
	bool m_bExitUpload;
	youmecommon::CXSemaphore m_uploadSemaphore;
	void UploadThreadProc();

	void UpdateRecvMessageSerial(XINT64 messageSerial);

	IYouMeIMMessageCallback* m_pCallback;
	IYouMeIMDownloadCallback* m_pDownloadCallback;
	bool m_bInit;
    
    //ptt 信息使用的锁
    std::mutex m_uploadInfoLock;
	std::map<XINT64, std::shared_ptr<FileUploadInfo> > m_fileUploadMap;
    
	//下载文件使用的锁
	std::mutex m_DownloadInfoLock;
	std::map<XINT64, FileDownloadInfo> m_downloadInfo;
	std::thread m_downloadThread;
	youmecommon::CXSemaphore m_downloadSemaphore;
	void DownloadThreadProc();
	std::list<FileDownloadInfo> m_downloadList;
	bool m_bExitDownload;
	//离线消息只能获取一次。否则就要出bug了
	bool m_bIsGetOfflineMsg;


	//语音 结束的时候带上的参数，由于每次只能启动一个，所以用一个成员变量
	XString m_strSendAudioParam;
};
