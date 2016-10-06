#pragma once

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include <map>
#include <list>
#include <string>

typedef struct MyMessageBase
{
	std::string strSendID;

	std::string strBody;

	YouMeIMMessageBodyType ymType;
} MyMessageBase, *PMyMessageBase;

typedef std::list<PMyMessageBase> MyMessageList;
typedef MyMessageList* PMyMessageList;

class MyIMManager
{
	MyIMManager();
public:
	static MyIMManager& Instance();
	~MyIMManager();

public:
	static IYouMeIMManager* GetYouMeIMEngine();

	void SetLoginUser(const std::string& strUserID);
	std::string GetLoginUser();

private:
	IYouMeIMManager* myInstance;

	std::string m_loginUserID;
};

class IMyIMMessageResponseCallback
{
public:
	virtual void OnMessageResponse(const std::string& strID) = 0;
};

class MyIMMessageContainer : public IYouMeIMMessageCallback,
public IYouMeIMDownloadCallback
{
	MyIMMessageContainer();
public:
	static MyIMMessageContainer& Instance();
	~MyIMMessageContainer();

public:
    
    void OnDownload(XINT64 ulSerial, YouMeIMErrorcode errorcode,const XString& strSavePath);
    void OnSendAudioMessageStatus(XINT64 iRequestID,YouMeIMErrorcode errorcode,const XString&strText,const XString& strAudioPath);
    //发
    
    
    //送消息状态
	void OnSendMessageStatus(XINT64 iRequestID,  YouMeIMErrorcode errorcode) override;
	//接收到用户发来的消息
	void OnRecvMessage(IYouMeIMMessage* pMessage) override;

	void SetResponseCallback(IMyIMMessageResponseCallback* pCallback);

	void GetMessageList(const std::string& strID, MyMessageList& mymsglist);

	bool HaveMessage(const std::string& strID);
private:
	std::map<std::string, PMyMessageList> m_msgHistorys;

	IMyIMMessageResponseCallback* m_pResponseCallback = NULL;
};