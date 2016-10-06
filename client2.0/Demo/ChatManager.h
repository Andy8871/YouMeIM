#pragma once


#include "YouMeCommon/CrossPlatformDefine/PlatformDef.h"
#include "YouMeIMEngine/YouMeInterface.h"
#include <map>

class CChatDlg;

enum MessageType
{
	MSGTYPE_INVALID,
	MSGTYPE_TEXT,
	MSGTYPE_AUDIO,
	MSGTYPE_FILE,
	MSGTYPE_CUSTOM,
	MSGTYPE_SENDSTATUS
};

struct MessageInfo 
{
	XINT64 messageID;
	YouMeIMChatType chatType;
	MessageType messageType;
	XString senderID;
	std::map<std::string, XString> extendInfo;

	MessageInfo() : messageID(0), chatType(ChatType_Unknow), messageType(MSGTYPE_INVALID){}
};



class ChatManager : public IYouMeIMMessageCallback, public IYouMeIMDownloadCallback
{
public:
	ChatManager();
	~ChatManager();

	void Init(IYouMeIMManager* imManager);
	CChatDlg* ShowChatDialog(XString& userID, YouMeIMChatType chatType);
	void ShowMessage(MessageInfo* messageInfo);
	void CloseChatDailog(XString chatID);
	void DownloadFile(XINT64 msgID, XString path);
	void SendMessageStatus(XString text);

private:
	virtual void OnSendMessageStatus(XINT64 iRequestID, YouMeIMErrorcode errorcode);
	virtual void OnRecvMessage(IYouMeIMMessage* pMessage);
	virtual void OnSendAudioMessageStatus(XINT64 iRequestID, YouMeIMErrorcode errorcode, const XString&strText, const XString& strAudioPath, unsigned int audioTime);

	virtual void OnDownload(XINT64 ulSerial, YouMeIMErrorcode errorcode, const XString& strSavePath) override;

private:
	IYouMeIMManager* m_pIMManager;
	std::map<XString, CChatDlg*> m_chatDlgMap;
};
