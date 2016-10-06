#include "stdafx.h"
#include "ChatManager.h"
#include "ChatDlg.h"
#include "DemoDlg.h"
#include <mmsystem.h>
#include "YouMeCommon/StringUtil.hpp"


#pragma comment(lib, "WINMM.LIB")

ChatManager::ChatManager()
{
	
}

ChatManager::~ChatManager()
{

}

void ChatManager::Init(IYouMeIMManager* imManager)
{
	m_pIMManager = imManager;
	m_pIMManager->SetMessageCallback(this);
	m_pIMManager->SetDownloadCallback(this);
}

void ChatManager::OnRecvMessage(IYouMeIMMessage* pMessage)
{
	CDemoDlg* pDemoDlg = dynamic_cast<CDemoDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDemoDlg == NULL)
	{
		return;
	}
	
	IYouMeIMMessageBodyBase* pbodyBase = pMessage->GetMessageBody();
	switch (pbodyBase->GetMessageType())
	{
	case MessageBodyType_TXT:
	{
		IYouMeIMMessageBodyText* pMsgBody = dynamic_cast<IYouMeIMMessageBodyText*>(pbodyBase);
		if (pMsgBody != NULL)
		{
			MessageInfo* pMessageInfo = new MessageInfo;
			pMessageInfo->messageID = pMessage->GetMessageID();
			pMessageInfo->chatType = pMessage->GetChatType();
			pMessageInfo->messageType = MSGTYPE_TEXT;
			pMessageInfo->senderID = pMessage->GetSenderID();
			pMessageInfo->extendInfo["MessageText"] = pMsgBody->GetMessageContent();
			::PostMessage(pDemoDlg->m_hWnd, WM_RECV_CHAT_MESSAGE, (WPARAM)pMessageInfo, NULL);
		}
	}
		break;
	case MessageBodyType_CustomMesssage:
	{
		IYouMeIMMessageBodyCustom* pMsgBody = dynamic_cast<IYouMeIMMessageBodyCustom*>(pbodyBase);
		//std::string strData = pMsgBody->GetCustomMessage();
		MessageInfo* pMessageInfo = new MessageInfo;
		pMessageInfo->messageID = pMessage->GetMessageID();
		pMessageInfo->chatType = pMessage->GetChatType();
		pMessageInfo->messageType = MSGTYPE_CUSTOM;
		pMessageInfo->senderID = pMessage->GetSenderID();
		pMessageInfo->extendInfo["MessageText"] = UTF8TOXString(pMsgBody->GetCustomMessage());
		::PostMessage(pDemoDlg->m_hWnd, WM_RECV_CHAT_MESSAGE, (WPARAM)pMessageInfo, NULL);
	}
		break;
	case MessageBodyType_Voice:
	{
		IYouMeIMMessageBodyAudio* pMsgBody = dynamic_cast<IYouMeIMMessageBodyAudio*>(pbodyBase);
		if (pMsgBody != NULL)
		{
			MessageInfo* pMessageInfo = new MessageInfo;
			pMessageInfo->messageID = pMessage->GetMessageID();
			pMessageInfo->chatType = pMessage->GetChatType();
			pMessageInfo->messageType = MSGTYPE_AUDIO;
			pMessageInfo->senderID = pMessage->GetSenderID();
			pMessageInfo->extendInfo["FileSize"] = CStringUtilT<wchar_t>::to_string(pMsgBody->GetFileSize());
			pMessageInfo->extendInfo["AudioTime"] = CStringUtilT<wchar_t>::to_string(pMsgBody->GetAudioTime());
			pMessageInfo->extendInfo["AudioText"] = pMsgBody->GetText();
			::PostMessage(pDemoDlg->m_hWnd, WM_RECV_CHAT_MESSAGE, (WPARAM)pMessageInfo, NULL);
		}
	}
	break;
	case MessageBodyType_File:
	{
		IYouMeIMMessageBodyFile* pMsgBody = dynamic_cast<IYouMeIMMessageBodyFile*>(pbodyBase);
		if (pMsgBody != NULL)
		{
			MessageInfo* pMessageInfo = new MessageInfo;
			pMessageInfo->messageID = pMessage->GetMessageID();
			pMessageInfo->chatType = pMessage->GetChatType();
			pMessageInfo->messageType = MSGTYPE_FILE;
			pMessageInfo->senderID = pMessage->GetSenderID();
			pMessageInfo->extendInfo["FileSize"] = CStringUtilT<wchar_t>::to_string(pMsgBody->GetFileSize());
			pMessageInfo->extendInfo["FileName"] = pMsgBody->GetFileName();
			::PostMessage(pDemoDlg->m_hWnd, WM_RECV_CHAT_MESSAGE, (WPARAM)pMessageInfo, NULL);
		}
	}
	break;
	default:
		break;
	}
}

void ChatManager::OnSendMessageStatus(XINT64 iRequestID, YouMeIMErrorcode errorcode)
{
	CDemoDlg* pDemoDlg = dynamic_cast<CDemoDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDemoDlg == NULL)
	{
		return;
	}
	MessageInfo* pMessageInfo = new MessageInfo;
	pMessageInfo->messageType = MSGTYPE_SENDSTATUS;
	pMessageInfo->extendInfo["ErrorCode"] = CStringUtilT<wchar_t>::to_string(errorcode);
	pMessageInfo->extendInfo["MsgSeaial"] = CStringUtilT<wchar_t>::to_string(iRequestID);
	::PostMessage(pDemoDlg->m_hWnd, WM_RECV_CHAT_MESSAGE, (WPARAM)pMessageInfo, NULL);
}

void ChatManager::OnSendAudioMessageStatus(XINT64 iRequestID, YouMeIMErrorcode errorcode, const XString&strText, const XString& strAudioPath, unsigned int audioTime)
{
	CDemoDlg* pDemoDlg = dynamic_cast<CDemoDlg*>(AfxGetApp()->m_pMainWnd);
	if (pDemoDlg == NULL)
	{
		return;
	}
	MessageInfo* pMessageInfo = new MessageInfo;
	pMessageInfo->messageType = MSGTYPE_SENDSTATUS;
	pMessageInfo->extendInfo["ErrorCode"] = CStringUtilT<wchar_t>::to_string(errorcode);
	pMessageInfo->extendInfo["MsgSeaial"] = CStringUtilT<wchar_t>::to_string(iRequestID);
	pMessageInfo->extendInfo["Text"] = strText;
	pMessageInfo->extendInfo["AudioTime"] = CStringUtilT<wchar_t>::to_string(audioTime);;

	::PostMessage(pDemoDlg->m_hWnd, WM_RECV_CHAT_MESSAGE, (WPARAM)pMessageInfo, NULL);
}

CChatDlg* ChatManager::ShowChatDialog(XString& userID, YouMeIMChatType chatType)
{
	if (m_pIMManager == NULL)
	{
		return NULL;
	}
	CChatDlg* pChatDlg;
	std::map<XString, CChatDlg*>::const_iterator itr = m_chatDlgMap.find(userID);
	if (itr != m_chatDlgMap.end())
	{
		pChatDlg = itr->second;
	}
	else
	{
		pChatDlg = new CChatDlg();
		pChatDlg->Create(IDD_DIALOG_CHATDLG);
		pChatDlg->SetChatProperty(userID, chatType);
		pChatDlg->ShowWindow(SW_SHOW);
		m_chatDlgMap[userID] = pChatDlg;
	}
	pChatDlg->ShowWindow(SW_SHOW);

	return pChatDlg;
}

void ChatManager::CloseChatDailog(XString chatID)
{
	std::map<XString, CChatDlg*>::const_iterator itr = m_chatDlgMap.find(chatID);
	if (itr != m_chatDlgMap.end())
	{
		delete itr->second;
		m_chatDlgMap.erase(itr);
	}
}

void ChatManager::ShowMessage(MessageInfo* messageInfo)
{
	if (messageInfo == NULL)
	{
		return;
	}

	CChatDlg* pChatDialog = NULL;
	if (!messageInfo->senderID.empty())
	{
		std::map<XString, CChatDlg*>::const_iterator itr = m_chatDlgMap.find(messageInfo->senderID);
		if (itr != m_chatDlgMap.end())
		{
			pChatDialog = itr->second;
		}
		else
		{
			pChatDialog = ShowChatDialog(messageInfo->senderID, messageInfo->chatType);
		}

		if (pChatDialog == NULL)
		{
			delete messageInfo;
			return;
		}
	}

	switch (messageInfo->messageType)
	{
	case MSGTYPE_TEXT:
	{
		std::map<std::string, XString>::const_iterator itr = messageInfo->extendInfo.find("MessageText");
		if (itr != messageInfo->extendInfo.end())
		{
			pChatDialog->ShowTextMessage(itr->second);
		}
	}
		break;
	case MSGTYPE_AUDIO:
	{
		unsigned int iFileSize = 0;
		unsigned int iAudiTime = 0;
		XString strText;
		std::map<std::string, XString>::const_iterator itr = messageInfo->extendInfo.find("FileSize");
		if (itr != messageInfo->extendInfo.end())
		{
			iFileSize = _wtoi(itr->second.c_str());
		}
		itr = messageInfo->extendInfo.find("AudioTime");
		if (itr != messageInfo->extendInfo.end())
		{
			iAudiTime = _wtoi(itr->second.c_str());
		}
		itr = messageInfo->extendInfo.find("AudioText");
		if (itr != messageInfo->extendInfo.end())
		{
			strText = itr->second;
		}
		pChatDialog->ReceiveAudio(messageInfo->messageID, strText, iFileSize, iAudiTime);
	}
		break;
	case MSGTYPE_FILE:
	{
		unsigned int iFileSize = 0;
		XString strFileName;
		std::map<std::string, XString>::const_iterator itr = messageInfo->extendInfo.find("FileSize");
		if (itr != messageInfo->extendInfo.end())
		{
			iFileSize = _wtoi(itr->second.c_str());
		}
		itr = messageInfo->extendInfo.find("FileName");
		if (itr != messageInfo->extendInfo.end())
		{
			strFileName = itr->second;
		}
		else
		{
			strFileName = __XT("temp");

		}
		pChatDialog->ReceiveFile(messageInfo->messageID, strFileName, iFileSize);
	}
		break;
	case MSGTYPE_CUSTOM:
	{

	}
		break;
	case MSGTYPE_SENDSTATUS:
	{
		XINT64 msgSeaial = 0;
		YouMeIMErrorcode errorCode = YouMeIMErrorcode_Success;
		std::map<std::string, XString>::const_iterator itr = messageInfo->extendInfo.find("MsgSeaial");
		if (itr != messageInfo->extendInfo.end())
		{
			msgSeaial = _wtoi64(itr->second.c_str());
		}
		itr = messageInfo->extendInfo.find("ErrorCode");
		if (itr != messageInfo->extendInfo.end())
		{
			errorCode = static_cast<YouMeIMErrorcode>(_wtoi(itr->second.c_str()));
		}

		int audioTime = 0;
		bool bAudio = false;
		itr = messageInfo->extendInfo.find("AudioTime");
		if (itr != messageInfo->extendInfo.end())
		{
			audioTime = _wtoi(itr->second.c_str());
			bAudio = true;
		}
		
		CString csText;
		if (errorCode != YouMeIMErrorcode_Success)
		{
			csText.Format(__XT("·¢ËÍ%sÊ§°Ü£¬errorCode:%d"), bAudio ? L"ÓïÒô" : L"", errorCode);
		}
		else
		{
			if (bAudio)
			{
				itr = messageInfo->extendInfo.find("Text");
				if (itr != messageInfo->extendInfo.end())
				{
					csText.Format(__XT("ÓïÒô %s (Ê±³¤£º%ds)"), itr->second.c_str(), audioTime);
				}
			}
		}

		XString strText = csText.GetString();
		for (std::map<XString, CChatDlg*>::const_iterator itr = m_chatDlgMap.begin(); itr != m_chatDlgMap.end(); ++itr)
		{
			itr->second->SendMessageStatus(msgSeaial, strText);
		}
	}
		break;
	default:
		break;
	}

	delete messageInfo;
}

void ChatManager::DownloadFile(XINT64 msgID, XString path)
{
	if (m_pIMManager == NULL || m_pIMManager->GetMessageManager() == NULL)
	{
		return;
	}

	m_pIMManager->GetMessageManager()->DownloadAudio(msgID, path);
}

void ChatManager::OnDownload(XINT64 ulSerial, YouMeIMErrorcode errorcode, const XString& strSavePath)
{
	for (std::map<XString, CChatDlg*>::const_iterator itr = m_chatDlgMap.begin(); itr != m_chatDlgMap.end(); ++itr)
	{
		itr->second->DownloadFinish(errorcode, ulSerial, strSavePath);
	}
}