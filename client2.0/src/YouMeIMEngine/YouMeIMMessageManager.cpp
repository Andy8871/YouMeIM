// stdafx.cpp : 只包括标准包含文件的源文件
// CrossPlatformDefine.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息


#include "YouMeIMMessageManager.h"
#include <YouMeCommon/Log.h>
#include <YouMeCommon/XFile.h>
#include "YouMeIMManager.h"
#include <YouMeCommon/DownloadUploadManager.h>
#include <YouMeIMEngine/Speech/YouMeSpeechManager.h>
#include <YouMeCommon/json/json.h>
#include <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeIMEngine/Speech/SpeechUtil.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/profiledb.h>
#include <YouMeCommon/amrFileCodec.h>
#include <YouMeCommon/XFile.h>
extern CYouMeSpeechManager* g_pSpeechManager;
extern ServerZone g_ServerZone;
extern int g_iYouMeIMMode;

YouMeIMErrorcode YouMeIMMessageManager::Init()
{
	if (m_bInit)
	{
		return YouMeIMErrorcode_Success;
	}
	m_bIsGetOfflineMsg = false;
	m_bExitUpload = false;
	m_bExitDownload = false;
	m_pCallback = NULL;
    g_pSpeechManager->SetSpeechListen(this);
	m_uploadThread = std::thread(&YouMeIMMessageManager::UploadThreadProc, this);
	m_downloadThread = std::thread(&YouMeIMMessageManager::DownloadThreadProc, this);
	m_bInit = true;
	return YouMeIMErrorcode_Success;
}

void YouMeIMMessageManager::UnInit()
{
	if (!m_bInit)
	{
		return;
	}
    {
        std::lock_guard<std::mutex> lock(m_uploadInfoLock);
        m_fileUploadMap.clear();
    }
    if (m_uploadThread.joinable())
    {
		m_bExitUpload = true;
		m_uploadSemaphore.Increment();
		m_uploadThread.join();
    }
	if (m_downloadThread.joinable())
	{
		m_bExitDownload = true;
		m_downloadSemaphore.Increment();
		m_downloadThread.join();
	}
    g_pSpeechManager->SetSpeechListen(NULL);
    m_bInit = false;
	m_pCallback = NULL; 
}


void YouMeIMMessageManager::SetMessageCallback(IYouMeIMMessageCallback* pCallback)
{
	m_pCallback = pCallback;
}

YouMeIMErrorcode YouMeIMMessageManager::SendMessage(IYouMeIMMessage* pMessage, XINT64 *pRequestID,XINT64 ulPassSerial)
{
	//没有消息体那就不用发了
	IYouMeIMMessageBodyBase* pBodyBase = pMessage->GetMessageBody();
	if (NULL == pBodyBase)
	{
		YouMe_LOG_Error(__XT("参数错误，没有消息体"));
		return YouMeIMErrorcode_ParamInvalid;
	}
	YouMeIMErrorcode errorcode = YouMeIMErrorcode_Fail;
	std::shared_ptr<MessageInfo> pSaveMessage;
	XString strRecvID = pMessage->GetReceiveID();
	YouMeIMProtocol::YouMeIM_ChatType chatType = (YouMeIMProtocol::YouMeIM_ChatType)pMessage->GetChatType();
	XINT64 ulPackSerial = 0;
	switch (pBodyBase->GetMessageType())
	{
		case MessageBodyType_TXT:
		{
			//构造消息体
			IYouMeIMMessageBodyText* pBody = (IYouMeIMMessageBodyText*)pBodyBase;
			YouMeIMProtocol::YouMeIM_MessageBodyText* pBodyText = new YouMeIMProtocol::YouMeIM_MessageBodyText;
			pBodyText->set_msgtype(YouMeIMProtocol::MessageBodyType_TXT);
			pBodyText->set_content(XStringToUTF8(pBody->GetMessageContent()));


			YouMeIMProtocol::YouMeIM_Command_ClientToServerText request;
			request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Commnad_Chat_Text));
			request.set_allocated_messagebody(pBodyText);
			request.set_chattype(chatType);
			request.set_youmegroupid(XStringToUTF8(strRecvID));
			errorcode = m_pIManager->SendProtoPacket(request);
			pSaveMessage = std::shared_ptr<MessageInfo>(new MessageInfo(YouMeIMProtocol::Commnad_Chat_Text));
			ulPackSerial = request.msghead().msgserialnum();
			if (NULL != pRequestID)
			{
				*pRequestID = request.msghead().msgserialnum();
			}
		}
			break;
		case MessageBodyType_CustomMesssage:
		{
			//构造消息体
			IYouMeIMMessageBodyCustom* pBody = (IYouMeIMMessageBodyCustom*)pBodyBase;
			YouMeIMProtocol::YouMeIM_MessageBodyCustom* pBodyCustom = new YouMeIMProtocol::YouMeIM_MessageBodyCustom;
			pBodyCustom->set_msgtype(YouMeIMProtocol::MessageBodyType_CustomMesssage);
	
			pBodyCustom->set_customcontent(pBody->GetCustomMessage());

			YouMeIMProtocol::YouMeIM_Command_ClientToServerCustom request;
			request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Commnad_Chat_Custom));
			request.set_allocated_messagebody(pBodyCustom);
			request.set_chattype(chatType);
			request.set_youmegroupid(XStringToUTF8(strRecvID));
			errorcode = m_pIManager->SendProtoPacket(request);
			pSaveMessage = std::shared_ptr<MessageInfo>(new MessageInfo(YouMeIMProtocol::Commnad_Chat_Custom));
			ulPackSerial = request.msghead().msgserialnum();
			if (NULL != pRequestID)
			{
				*pRequestID = request.msghead().msgserialnum();
			}
		}
			break;
        case MessageBodyType_Voice:
        {
            //构造消息体
			CYouMeIMMessageBodyAudio* pBody = (CYouMeIMMessageBodyAudio*)pBodyBase;
            YouMeIMProtocol::YouMeIM_MessageBodyVoice* pBodyAudio = new YouMeIMProtocol::YouMeIM_MessageBodyVoice;
            pBodyAudio->set_msgtype(YouMeIMProtocol::MessageBodyType_Voice);
            pBodyAudio->set_characters(XStringToUTF8(pBody->GetText()));
			pBodyAudio->set_key("");
			pBodyAudio->set_param(XStringToUTF8(pBody->m_strParam));
            pBodyAudio->set_downloadurl("");
			pBodyAudio->set_time(pBody->m_iTime);
			pBodyAudio->set_areacode(g_ServerZone);
			pBodyAudio->set_filesize(pBody->m_iFileSize);
			pBodyAudio->set_md5(XStringToUTF8(pBody->m_strMd5));
			pBodyAudio->set_suffix(XStringToUTF8(pBody->m_strSuffix));
            YouMeIMProtocol::YouMeIM_Command_ClientToServerVoice request;
            request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Commnad_Chat_Voice));
            request.set_allocated_messagebody(pBodyAudio);
            request.set_chattype(chatType);
            request.set_youmegroupid(XStringToUTF8(strRecvID));
            //修改一下包头的序号
            request.mutable_msghead()->set_msgserialnum(ulPassSerial);
            
            errorcode = m_pIManager->SendProtoPacket(request);
            pSaveMessage = std::shared_ptr<MessageInfo>(new MessageInfo(YouMeIMProtocol::Commnad_Chat_Voice));
            pSaveMessage->SetParam("Text", pBody->GetText());
            pSaveMessage->SetParam("LocalPath", pBody->m_strLocalPath);
			pSaveMessage->SetParam("AudioTime", pBody->m_iTime);
            ulPackSerial = ulPassSerial;
        }
            break;
		case MessageBodyType_File:
		{
			CYouMeIMMessageBodyFile* pMsgBody = dynamic_cast<CYouMeIMMessageBodyFile*>(pBodyBase);
			if (pMsgBody != NULL)
			{
				YouMeIMProtocol::YouMeIM_MessageBodyFile* pMsgBodyFile = new YouMeIMProtocol::YouMeIM_MessageBodyFile;
				pMsgBodyFile->set_msgtype(YouMeIMProtocol::MessageBodyType_File);
				pMsgBodyFile->set_url("");
				pMsgBodyFile->set_areacode(g_ServerZone);
				pMsgBodyFile->set_md5(XStringToUTF8(pMsgBody->m_strMd5));
				pMsgBodyFile->set_suffix(XStringToUTF8(pMsgBody->m_strExtension));
				pMsgBodyFile->set_filesize(pMsgBody->m_iFileSize);
				YouMeIMProtocol::NameValue* pOriginFileName = pMsgBodyFile->add_extend();
				if (pOriginFileName != NULL)
				{
					XString strFileName = youmecommon::CXFile::GetFileName(pMsgBody->m_strLocalPath.c_str());
					pOriginFileName->set_name("OriginFileName");
					pOriginFileName->set_value(XStringToUTF8(strFileName));
					pOriginFileName->set_type(YouMeIMProtocol::NAME_STRING);
				}
				YouMeIMProtocol::NameValue* pFileType = pMsgBodyFile->add_extend();
				if (pFileType != NULL)
				{
					pFileType->set_name("FileType");
					std::string strFileType = CStringUtilT<char>::to_string(pMsgBody->m_fileType);
					pFileType->set_value(strFileType);
					pFileType->set_type(YouMeIMProtocol::NAME_STRING);
				}
				YouMeIMProtocol::NameValue* pParam = pMsgBodyFile->add_extend();
				if (pParam != NULL)
				{
					pParam->set_name("Param");
					pParam->set_value(XStringToUTF8(pMsgBody->m_strParam));
					pParam->set_type(YouMeIMProtocol::NAME_STRING);
				}

				YouMeIMProtocol::YouMeIM_Command_ClientToServerFile request;
				request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Commnad_Chat_File));
				request.set_allocated_messagebody(pMsgBodyFile);
				request.set_chattype(chatType);
				request.set_youmegroupid(XStringToUTF8(strRecvID));
				request.mutable_msghead()->set_msgserialnum(ulPassSerial);

				errorcode = m_pIManager->SendProtoPacket(request);
				pSaveMessage = std::shared_ptr<MessageInfo>(new MessageInfo(YouMeIMProtocol::Commnad_Chat_File));
				pSaveMessage->SetParam("LocalPath", pMsgBody->m_strLocalPath);
				ulPackSerial = ulPassSerial;
			}
		}
			break;
		default:
			YouMe_LOG_Error(__XT("参数错误，没有设置消息类型"));
			break;
	}
	//成功则保存
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::lock_guard<std::mutex> lock(m_messageCacheMutex);
		m_messageMap[ulPackSerial] = pSaveMessage;
	}
	return errorcode;
}

CYouMeIMMessage* YouMeIMMessageManager::CreateMessage(YouMeIMChatType chatType)
{
	CYouMeIMMessage* pMessage = new CYouMeIMMessage();
	pMessage->SetChatType(chatType);
	return pMessage;
}



YouMeIMMessageManager::YouMeIMMessageManager(YouMeIMManager* pIManager):
IManagerBase(pIManager)
{
	m_bInit = false;
}

YouMeIMErrorcode YouMeIMMessageManager::SendTextMessage(const XString&strRecvID, YouMeIMChatType chatType, const XString& strContent, XINT64 *pRequestID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMe_LOG_Info(__XT("Enter"));
	CYouMeIMMessageBodyText* pBodyText = new CYouMeIMMessageBodyText;
	pBodyText->SetMessageContent(strContent);
	CYouMeIMMessage* pMessage = CreateMessage(chatType);
	pMessage->SetMessageBody(pBodyText);
	pMessage->SetReceiveID(strRecvID);
	YouMeIMErrorcode errorcode =  SendMessage(pMessage, pRequestID);

	return errorcode;
}

YouMeIMErrorcode YouMeIMMessageManager::SendCustomMessage(const XString&strRecvID, YouMeIMChatType chatType, const std::string& strCustomMessage, XINT64 *pRequestID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	CYouMeIMMessageBodyCustom* pBodyCustom = new CYouMeIMMessageBodyCustom;
	pBodyCustom->SetCustomMessage(strCustomMessage);
	CYouMeIMMessage* pMessage = CreateMessage(chatType);
	pMessage->SetMessageBody(pBodyCustom);
	pMessage->SetReceiveID(strRecvID);
	YouMeIMErrorcode errorcode = SendMessage(pMessage, pRequestID);
	delete pMessage;
	return errorcode;
}

YouMeIMErrorcode YouMeIMMessageManager::SendFile(const XString&strRecvID, YouMeIMChatType chatType, const XString& strPath, XINT64 *pRequestID, const XString& strParam, FileType fileType)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	youmecommon::CXFile file;
	if (file.LoadFile(strPath, youmecommon::CXFile::Mode_OpenExist_ReadOnly) != 0)
	{
		YouMe_LOG_Error(__XT("file not exist %s"), strPath.c_str());
		return YouMeIMErrorcode_FileNotExist;
	}
	unsigned int iFileSize = static_cast<unsigned int>(file.GetFileSize());
	file.Close();

	if (NULL == m_pIManager)
	{
		return YouMeIMErrorcode_StatusError;
	}
	XINT64 ulSerial = m_pIManager->GetUniqueSerial();
	if (NULL != pRequestID)
	{
		*pRequestID = ulSerial;
	}

	XString srtFileMD5 = youmecommon::CCryptUtil::MD5File(strPath);
	XString strExtension;
	XString::size_type stPos = strPath.find_last_of(__XT("."));
	if (stPos != XString::npos)
	{
		strExtension = strPath.substr(stPos + 1);
	}

	std::shared_ptr<FileUploadInfo> pFileUploadInfo(new FileUploadInfo);
	pFileUploadInfo->ulSerial = ulSerial;
	pFileUploadInfo->strRecvID = strRecvID;
	pFileUploadInfo->chatType = chatType;
	pFileUploadInfo->fileType = fileType;
	pFileUploadInfo->strLocalPath = strPath;
	pFileUploadInfo->iFileSize = iFileSize;
	pFileUploadInfo->strMd5 = srtFileMD5;
	pFileUploadInfo->strExtension = strExtension;
	if (strParam.size() > 0)
	{
		pFileUploadInfo->extendInfo["Param"] = strParam;
	}
	{
		std::lock_guard<std::mutex> lock(m_uploadInfoLock);
		m_fileUploadMap[ulSerial] = pFileUploadInfo;
	}

	YouMeIMProtocol::YouMeIM_Command_GetUploadToken request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_GetUploadtoken));
	request.set_areacode(g_ServerZone);
	request.set_md5(XStringToUTF8(srtFileMD5));
	request.set_filesize(iFileSize);
	request.set_suffix(XStringToUTF8(strExtension));
	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_GetUploadtoken));
		pMessage->SetParam("uploadSerial", ulSerial);
		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	else
	{
		std::map<XINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(ulSerial);
		if (itr != m_fileUploadMap.end())
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			m_fileUploadMap.erase(itr);
		}
	}

	return errorcode;
}

YouMeIMErrorcode YouMeIMMessageManager::SendAudioMessage(const XString&strRecvID, YouMeIMChatType chatType,  XINT64 *pRequestID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMe_LOG_Info(__XT("Enter"));
    XINT64 ulSerial = m_pIManager->GetUniqueSerial();
    if (NULL != pRequestID)
	{
        *pRequestID = ulSerial;
    }
    if (0 == g_pSpeechManager->StartSpeech(ulSerial))
    {
        {
            std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			std::shared_ptr<FileUploadInfo> pPtr(new FileUploadInfo);
			pPtr->ulSerial = ulSerial;
			pPtr->strRecvID = strRecvID;
			pPtr->chatType = chatType;
			pPtr->fileType = FileType_Audio;
            m_fileUploadMap[ulSerial] = pPtr;
        }
		m_strSendAudioParam = __XT("");
		YouMe_LOG_Info(__XT("Leave Success"));
        return YouMeIMErrorcode_Success;
    }
	YouMe_LOG_Info(__XT("Leave Fail"));
    return YouMeIMErrorcode_PTT_Fail;
}

YouMeIMErrorcode YouMeIMMessageManager::SendOnlyAudioMessage(const XString&strRecvID, YouMeIMChatType chatType,XINT64 *pRequestID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMe_LOG_Info(__XT("Enter"));
	XINT64 ulSerial = m_pIManager->GetUniqueSerial();
	if (NULL != pRequestID) {
		*pRequestID = ulSerial;
	}
	if (0 == g_pSpeechManager->StartOnlySpeech(ulSerial))
	{
		std::shared_ptr<FileUploadInfo> pPtr(new FileUploadInfo);
		pPtr->ulSerial = ulSerial;
		pPtr->strRecvID = strRecvID;
		pPtr->chatType = chatType;
		pPtr->fileType = FileType_Audio;
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			m_fileUploadMap[ulSerial] = pPtr;
		}
		YouMe_LOG_Info(__XT("Leave Success"));
		m_strSendAudioParam = __XT("");
		return YouMeIMErrorcode_Success;
	}
	YouMe_LOG_Info(__XT("Leave Fail"));
	return YouMeIMErrorcode_PTT_Fail;
}

YouMeIMErrorcode YouMeIMMessageManager::StopAudioMessage(const XString& strParam)
{
	YouMe_LOG_Info(__XT("Enter"));
	if (m_strSendAudioParam == __XT(""))
	{
		m_strSendAudioParam = strParam;
	}
    g_pSpeechManager->StopSpeech();
    return YouMeIMErrorcode_Success;
}
YouMeIMErrorcode YouMeIMMessageManager::CancleAudioMessage()
{
	YouMe_LOG_Info(__XT("Enter"));
    g_pSpeechManager->CancleSpeech();
    return YouMeIMErrorcode_Success;

}

bool YouMeIMMessageManager::OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize)
{
    if (NULL == m_pCallback)
	{
		YouMe_LOG_Warning(__XT("消息回调指针为空，上层是不是忘记设置了"));
    }
	bool bDeal = true;
	//有一些包是需要服务器立即回复的，这些包如果没有回复的话会回调给上层一个超时。如果已经回复了就需要删掉
	bool bCheckResopnse = false;
	MessageInfo messageInfo(YouMeIMProtocol::Command_Unknow);

	{
		std::lock_guard<std::mutex> lock(m_messageCacheMutex);
		std::map<XINT64, std::shared_ptr<MessageInfo> >::iterator it = m_messageMap.find(head.head().clientserialnum());
		if (it != m_messageMap.end())
		{
			messageInfo = *(it->second);
			m_messageMap.erase(it);
		}
	}

	switch (head.head().msgtype())
	{
	case YouMeIMProtocol::Command_GetUploadtokenResponse:
	{
        YouMe_LOG_Info(__XT("获取到了上传的token包"));
		bCheckResopnse = true;
		YouMeIMProtocol::YouMeIM_Command_GetUploadTokenResponse response;
		if (response.ParseFromArray(pBuffer, iBufferSize) && messageInfo.command != YouMeIMProtocol::Command_Unknow)
		{
			XINT64 ulUploadSerial = messageInfo.GetParam("uploadSerial", (XINT64)0);
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			std::map<XINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(ulUploadSerial);
			if (itr != m_fileUploadMap.end())
			{
				if (response.headinfo_size() > 0)
				{
					itr->second->strUrl = UTF8TOXString(response.uploadtoken());
					for (int i = 0; i < response.headinfo_size(); ++i)
					{
						itr->second->httpHead.insert(std::map<std::string, std::string>::value_type(response.headinfo(i).name(), response.headinfo(i).value()));
					}
					m_uploadSemaphore.Increment();
				}
				else
				{
					YouMe_LOG_Info(__XT("has not http headInfo"));
				}
			}
			else
			{
				YouMe_LOG_Error(__XT("has not found in m_fileUploadMap，serial:%lld"), ulUploadSerial);
			}
		}
		else
		{
			YouMe_LOG_Error(__XT("解析 Command_GetUploadtokenResponse，长度:%d"), iBufferSize);
		}
	}
	break;
	case YouMeIMProtocol::Command_GetOfflineMessageResponse:
	{
		bCheckResopnse = true;
		YouMeIMProtocol::YouMeIM_Command_GetOfflineMsgResponse response;
		if (response.ParseFromArray(pBuffer, iBufferSize))
		{
			if (NULL != m_pCallback)
			{				
				XINT64 ulLastMsg = 0;
				for (int i = 0; i < response.msglists_size(); i++)
				{
					YouMeIMProtocol::ServerReqHead head;
					//先解析头
					if (head.ParseFromArray(response.msglists(i).c_str(), response.msglists(i).length()))
					{
						if (head.head().msgserialnum() > ulLastMsg)
						{
							ulLastMsg = head.head().msgserialnum();
						}
						m_pIManager->OnRecvPacket(head, (const byte*)response.msglists(i).c_str(), response.msglists(i).length());
					}
					else
					{
						YouMe_LOG_Error(__XT("解析离线消息的head 失败：%d  当前消息，长度:%d"), i, response.msglists(i).length());
					}
				}
				UpdateRecvMessageSerial(ulLastMsg);
			}
		}		
		else
		{
			YouMe_LOG_Error(__XT("解析 YouMeIM_Command_GetOfflineMsgResponse，长度:%d"), iBufferSize);
		}
		m_bIsGetOfflineMsg = false;
	}
	break;
	case YouMeIMProtocol::Command_MessageToServerACK:
	{
		bCheckResopnse = true;
		if (NULL != m_pCallback/* && messageInfo.command != YouMeIMProtocol::Command_Unknow*/)
		{
			if (messageInfo.command == YouMeIMProtocol::Commnad_Chat_Voice)
			{
				YouMe_LOG_Info(__XT("Command_MessageToServerACK Commnad_Chat_Voice"));
				XString strLocalPath = messageInfo.GetParam("LocalPath", __XT(""));
				XString strWavTmpPath = strLocalPath;
				if (GetFileFormat(strLocalPath) == FileFormat_AMR)
				{
					//amr 转成wav
					strWavTmpPath = strLocalPath + __XT(".wav");
					DecodeAMRFileToWAVEFile(strLocalPath, strWavTmpPath);
					youmecommon::CXFile::remove_file(strLocalPath);
				}
				m_pCallback->OnSendAudioMessageStatus(head.head().clientserialnum(), YouMeIMErrorcode_Success, messageInfo.GetParam("Text", __XT("")), strWavTmpPath, messageInfo.GetParam<unsigned int>("AudioTime", 0));
			}
			else
			{
				m_pCallback->OnSendMessageStatus(head.head().clientserialnum(), YouMeIMErrorcode_Success);
			}
		}
		else
		{
			assert(false);
			YouMe_LOG_Error(__XT("Command_MessageToServerACK 没有找到序号:%lld"), head.head().clientserialnum());
		}
	}
	break;
	case YouMeIMProtocol::Commnad_Chat_Custom_Server:
	{
		YouMeIMProtocol::YouMeIM_Command_ServerToClientCustom response;
		if (response.ParseFromArray(pBuffer, iBufferSize))
		{
			if (NULL != m_pCallback)
			{
				std::shared_ptr<CYouMeIMMessage> pMessage(new CYouMeIMMessage);
				pMessage->SetChatType((YouMeIMChatType)response.chattype());
				//直接转成字符串
				pMessage->SetReceiveID(UTF8TOXString(response.youmegroupid()));
				pMessage->SetSenderID(UTF8TOXString(response.senderyoumeid()));
				pMessage->SetGroupID(UTF8TOXString(response.youmegroupid()));
				CYouMeIMMessageBodyCustom* pBody = new CYouMeIMMessageBodyCustom;
				pBody->m_messageType = ((YouMeIMMessageBodyType)response.messagebody().msgtype());
				pBody->SetCustomMessage(response.messagebody().customcontent());
				pMessage->SetMessageBody(pBody);
				pMessage->m_ulMsgSerial = head.head().msgserialnum();
				//回调
				m_pCallback->OnRecvMessage(pMessage.get());

				//UpdateRecvMessageSerial(head.head().msgserialnum());
			}
		}
		else
		{
			YouMe_LOG_Error(__XT("服务器发来的自定义消息解析失败，长度:%d"), iBufferSize);
		}
	}
	break;
	case YouMeIMProtocol::Commnad_Chat_Text_Server:
	{
		YouMeIMProtocol::YouMeIM_Command_ServerToClientText response;
		if (response.ParseFromArray(pBuffer, iBufferSize))
		{
			if (NULL != m_pCallback)
			{
				std::shared_ptr<CYouMeIMMessage> pMessage(new CYouMeIMMessage);
				pMessage->SetChatType((YouMeIMChatType)response.chattype());
				//直接转成字符串
				pMessage->SetReceiveID(UTF8TOXString(response.youmegroupid()));
				pMessage->SetSenderID(UTF8TOXString(response.senderyoumeid()));
				pMessage->SetGroupID(UTF8TOXString(response.youmegroupid()));
				CYouMeIMMessageBodyText* pBody = new CYouMeIMMessageBodyText;
				pBody->m_messageType = ((YouMeIMMessageBodyType)response.messagebody().msgtype());
				pBody->SetMessageContent(UTF8TOXString(response.messagebody().content()));
				pMessage->SetMessageBody(pBody);
				pMessage->m_ulMsgSerial = head.head().msgserialnum();
				//回调
				m_pCallback->OnRecvMessage(pMessage.get());

				//UpdateRecvMessageSerial(head.head().msgserialnum());
			}
		}
		else
		{
			YouMe_LOG_Error(__XT("服务器发来的文本消息解析失败，长度:%d"),iBufferSize);
		}
	}
	break;
    case YouMeIMProtocol::Commnad_Chat_Voice_Server:
    {
        YouMeIMProtocol::YouMeIM_Command_ServerToClientVoice response;
        if (response.ParseFromArray(pBuffer, iBufferSize))
        {
            if (NULL != m_pCallback)
            {
                std::shared_ptr<CYouMeIMMessage> pMessage(new CYouMeIMMessage);
                pMessage->SetChatType((YouMeIMChatType)response.chattype());
                //直接转成字符串
                pMessage->SetReceiveID(UTF8TOXString(response.youmegroupid()));
                pMessage->SetSenderID(UTF8TOXString(response.senderyoumeid()));
				pMessage->SetGroupID(UTF8TOXString(response.youmegroupid()));
                CYouMeIMMessageBodyAudio* pBody = new CYouMeIMMessageBodyAudio;
                pBody->m_messageType = ((YouMeIMMessageBodyType)response.messagebody().msgtype());
                pBody->SetText(UTF8TOXString(response.messagebody().characters()));
				pBody->m_strDownloadURL = UTF8TOXString(response.messagebody().downloadurl());
				pBody->m_strParam = UTF8TOXString(response.messagebody().param());
				pBody->m_strMd5 = UTF8TOXString(response.messagebody().md5());
				pBody->m_strSuffix = UTF8TOXString(response.messagebody().suffix());
				pBody->m_iFileSize = response.messagebody().filesize();
				pBody->m_iTime = response.messagebody().time();

                pMessage->SetMessageBody(pBody);
				pMessage->m_ulMsgSerial = head.head().msgserialnum();

				{
					std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
					FileDownloadInfo downloadInfo;
					downloadInfo.strURL = pBody->m_strDownloadURL;
					downloadInfo.ulSerial = pMessage->m_ulMsgSerial;
					m_downloadInfo[pMessage->m_ulMsgSerial] = downloadInfo;
				}	
				//回调
				m_pCallback->OnRecvMessage(pMessage.get());

				//UpdateRecvMessageSerial(head.head().msgserialnum());
            }
        }
        else
        {
            YouMe_LOG_Error(__XT("服务器发来的语音消息解析失败，长度:%d"),iBufferSize);
        }
    }
		break;
	case YouMeIMProtocol::Commnad_Chat_File_Server:
	{
		YouMeIMProtocol::YouMeIM_Command_ServerToClientFile response;
		if (response.ParseFromArray(pBuffer, iBufferSize))
		{
			if (NULL != m_pCallback)
			{
				CYouMeIMMessageBodyFile* pMsgBody = new CYouMeIMMessageBodyFile;
				pMsgBody->m_messageType = ((YouMeIMMessageBodyType)response.messagebody().msgtype());
				pMsgBody->m_strDownloadURL = UTF8TOXString(response.messagebody().url());
				//pMsgBody->m_strMd5 = UTF8TOXString(response.messagebody().md5());
				pMsgBody->m_strExtension = UTF8TOXString(response.messagebody().suffix());
				pMsgBody->m_iFileSize = response.messagebody().filesize();
				for (int i = 0; i < response.messagebody().extend_size(); ++i)
				{
					YouMeIMProtocol::NameValue nv = response.messagebody().extend(i);
					if (nv.name() == "OrginFileName")
					{
						pMsgBody->m_strFileName = UTF8TOXString(nv.value());
						break;
					}
					else if (nv.name() == "FileType")
					{
						pMsgBody->m_fileType = (FileType) atoi(nv.value().c_str());
						break;
					}
					else if (nv.name() == "Param")
					{
						pMsgBody->m_strParam = UTF8TOXString(nv.value());
						break;
					}
				}

				std::shared_ptr<CYouMeIMMessage> pMessage(new CYouMeIMMessage);
				pMessage->SetChatType((YouMeIMChatType)response.chattype());
				pMessage->SetReceiveID(UTF8TOXString(response.youmegroupid()));
				pMessage->SetSenderID(UTF8TOXString(response.senderyoumeid()));
				pMessage->SetGroupID(UTF8TOXString(response.youmegroupid()));
				pMessage->SetMessageBody(pMsgBody);
				pMessage->m_ulMsgSerial = head.head().msgserialnum();

				FileDownloadInfo downloadInfo;
				downloadInfo.strURL = pMsgBody->m_strDownloadURL;
				downloadInfo.ulSerial = pMessage->m_ulMsgSerial;
				{
					std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
					m_downloadInfo[pMessage->m_ulMsgSerial] = downloadInfo;
				}
				
				m_pCallback->OnRecvMessage(pMessage.get());

				//UpdateRecvMessageSerial(head.head().msgserialnum());
			}
		}
		else
		{
			YouMe_LOG_Error(__XT("服务器发来的语音消息解析失败，长度:%d"), iBufferSize);
		}
	}
    break;
	case YouMeIMProtocol::Commnad_GetMessage_Notify:
	{
		if (!m_bIsGetOfflineMsg)
		{
			XINT64 ulMsgSerial = head.head().msgserialnum();
			XString strLastMsgSerial;
			std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%s_%d", XStringToUTF8(m_pIManager->m_strYouMeID).c_str(), g_iYouMeIMMode);
			CProfileDB::Instance().getSetting(strKey, strLastMsgSerial);
			XINT64 ulCurMsgSerial = CStringUtil::str_to_sint64(strLastMsgSerial);
			if (ulMsgSerial > ulCurMsgSerial)
			{
				GetContactOfflineMessage();
			}
		}
		else
		{
			YouMe_LOG_Info(__XT("已经再获取离线消息了，就不用去获取了"));
		}
	}
	break;
    default:
		bDeal = false;
		break;
	}
	return bDeal;
}


bool YouMeIMMessageManager::OnRecvPacketTimeOut(XINT64 iUniqueID)
{
	MessageInfo messageInfo(YouMeIMProtocol::Command_Unknow);

	{
		std::lock_guard<std::mutex> lock(m_messageCacheMutex);
		std::map<XINT64, std::shared_ptr<MessageInfo> >::iterator it = m_messageMap.find(iUniqueID);
		if (it == m_messageMap.end())
		{
			return false;
		}
		messageInfo = *(it->second);
		YouMe_LOG_Warning(__XT("超时包信息，类型：%d"), messageInfo.command);
		m_messageMap.erase(it);
	}

	switch (messageInfo.command)
	{
	case YouMeIMProtocol::Commnad_Chat_Text:
	case YouMeIMProtocol::Commnad_Chat_Custom:
   
	{
		if (NULL != m_pCallback)
		{
			m_pCallback->OnSendMessageStatus(iUniqueID,YouMeIMErrorcode_TimeOut);
		}
	}
	break;
    case YouMeIMProtocol::Commnad_Chat_Voice:
    {
        if (NULL != m_pCallback) {
            m_pCallback->OnSendAudioMessageStatus(iUniqueID, YouMeIMErrorcode_TimeOut, __XT(""), __XT(""), 0);
        }
    }
    break;
	case YouMeIMProtocol::Command_GetUploadtoken:
	{
		XINT64 ulPTTSerial = messageInfo.GetParam("uploadSerial", (XINT64)0);
		if (ulPTTSerial == 0 || m_pCallback == NULL)
		{
			return false;
		}

		FileUploadInfo uploadInfo;
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			std::map<XINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(ulPTTSerial);
			if (itr != m_fileUploadMap.end())
			{
				uploadInfo = *(itr->second);
				m_fileUploadMap.erase(itr);
			}
		}

		if (uploadInfo.fileType == FileType_Audio)
		{
			m_pCallback->OnSendAudioMessageStatus(uploadInfo.ulSerial, YouMeIMErrorcode_TimeOut, __XT(""), __XT(""), 0);
		}
		else
		{
			m_pCallback->OnSendMessageStatus(uploadInfo.ulSerial, YouMeIMErrorcode_TimeOut);
		}
	}
	break;
	case YouMeIMProtocol::Command_GetOfflineMessage:
	{
		//这个消息暂时不用处理
		m_bIsGetOfflineMsg = false;
	}
	break;
	default:
		YouMe_LOG_Warning(__XT("没有处理的需要服务器回复的超时包:%d"), messageInfo.command);
		break;
	}
	return true;
}

YouMeIMErrorcode YouMeIMMessageManager::GetContactOfflineMessage()
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	return InterGetOfflineMsg(m_pIManager->m_strYouMeID, YouMeIMProtocol::ChatType_PrivateChat);
}

YouMeIMErrorcode YouMeIMMessageManager::GetGroupOfflineMessage(const XString& strGroupID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	return InterGetOfflineMsg(strGroupID, YouMeIMProtocol::ChatType_GroupChat);
}

FileFormat YouMeIMMessageManager::GetFileFormat(const XString& strFilePath)
{
	youmecommon::CXFile file;
	if (0 != file.LoadFile(strFilePath, youmecommon::CXFile::Mode_OpenExist_ReadOnly))
	{
		return FileFormat_Unknow;
	}
	//先判断是不是wav 格式
	RIFFHEADER riff;
	memset(&riff, 0,sizeof(riff));
	file.Read((byte*)&riff, sizeof(riff));
	if (0 == memcmp(riff.chRiffID, "RIFF", 4))
	{
		return FileFormat_WAV;
	}
	char amrMagic[10] = { 0 };
	file.Seek(0, SEEK_SET);
	file.Read((byte*)amrMagic, strlen(AMR_MAGIC_NUMBER));
	if (0 == memcmp(amrMagic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
	{
		return FileFormat_AMR;
	}
	return FileFormat_Unknow;
}

YouMeIMErrorcode YouMeIMMessageManager::InterGetOfflineMsg(const XString& strYouMeGroupID, YouMeIMProtocol::YouMeIM_ChatType chatType)
{
	XString strLastMsgSerial;
	std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%s_%d", XStringToUTF8(m_pIManager->m_strYouMeID).c_str(), g_iYouMeIMMode);
	CProfileDB::Instance().getSetting(strKey, strLastMsgSerial);
	XINT64 ulCurMsgSerial = CStringUtil::str_to_sint64(strLastMsgSerial);

	YouMeIMProtocol::YouMeIM_Command_GetOfflineMsg request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_GetOfflineMessage));
	request.set_chattype(chatType);
	request.set_youmegroupid(XStringToUTF8(strYouMeGroupID));
	request.set_lastmsgserial(ulCurMsgSerial);
	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		m_bIsGetOfflineMsg = true;
		std::shared_ptr<MessageInfo> pSaveMessage(new MessageInfo(YouMeIMProtocol::Command_GetOfflineMessage));
		pSaveMessage->SetParam("ChatType", chatType);
		pSaveMessage->SetParam("YouMeGroupID", strYouMeGroupID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pSaveMessage;
		}
	}
	return errorcode;
}


YouMeIMChatType CYouMeIMMessage::GetChatType()
{
	return m_chatType;
}

void CYouMeIMMessage::SetChatType(YouMeIMChatType chatType)
{
	m_chatType = chatType;
}

XString CYouMeIMMessage::GetReceiveID()
{
	return m_strReceiveID;
}
XINT64 CYouMeIMMessage::GetMessageID()
{
    return m_ulMsgSerial;
}
void CYouMeIMMessage::SetReceiveID(const XString&strReceiverID)
{
	m_strReceiveID = strReceiverID;
}

void CYouMeIMMessage::SetMessageBody(IYouMeIMMessageBodyBase* pBody)
{
	if (NULL != m_pMessageBody)
	{
		delete m_pMessageBody;
	}
	m_pMessageBody = pBody;
}

IYouMeIMMessageBodyBase* CYouMeIMMessage::GetMessageBody()
{
	return m_pMessageBody;
}

CYouMeIMMessage::CYouMeIMMessage()
{
	m_chatType = ChatType_Unknow;
	m_pMessageBody = NULL;
}

void CYouMeIMMessage::SetSenderID(const XString& strSenderID)
{
	m_strSenderid = strSenderID;
}

XString CYouMeIMMessage::GetSenderID()
{
	return m_strSenderid;
}

CYouMeIMMessage::~CYouMeIMMessage()
{
	delete m_pMessageBody;
}

XString CYouMeIMMessage::GetGroupID()
{
	return m_strGroupID;
}

void CYouMeIMMessage::SetGroupID(const XString& groupID)
{
	m_strGroupID = groupID;
}


//文本消息
XString CYouMeIMMessageBodyText::GetMessageContent()
{
	return m_strContent;
}

void CYouMeIMMessageBodyText::SetMessageContent(const XString&strContent)
{
	m_strContent = strContent;
}


//自定义消息
void CYouMeIMMessageBodyCustom::SetCustomMessage(const std::string& strCustomMessage)
{
	m_strData.clear();
	m_strData = strCustomMessage;
}

std::string CYouMeIMMessageBodyCustom::GetCustomMessage()
{
	return m_strData;
}

//语音消息
XString CYouMeIMMessageBodyAudio::GetText()
{
    return m_strText;
}

XString CYouMeIMMessageBodyAudio::GetParam()
{
	return m_strParam;
}

void CYouMeIMMessageBodyAudio::SetText(const XString& strText)
{
    m_strText = strText;
}

unsigned int CYouMeIMMessageBodyAudio::GetFileSize()
{
	return m_iFileSize;
}

unsigned int CYouMeIMMessageBodyAudio::GetAudioTime()
{
	return m_iTime;
}

unsigned int CYouMeIMMessageBodyFile::GetFileSize()
{
	return m_iFileSize;
}

XString CYouMeIMMessageBodyFile::GetFileName()
{
	return m_strFileName;
}

XString CYouMeIMMessageBodyFile::GetFileExtension()
{
	return m_strExtension;
}

FileType CYouMeIMMessageBodyFile::GetFileType()
{
	return m_fileType;
}

XString CYouMeIMMessageBodyFile::GetParam()
{
	return m_strParam;
}



void YouMeIMMessageManager::OnSpeechInit(int iErrorcode)
{
    YouMe_LOG_Info(__XT("Speech 初始化错误码:%d"),iErrorcode);
    if (NULL != m_pCallback) {
       
    }
}
void YouMeIMMessageManager::OnSpeechResult(XINT64 ulSerial,int iErrorcode, const XString& strResult, const XString& strWavPath)
{
    YouMe_LOG_Info(__XT("Speech 结束:Serial:%lld  Errorcode:%d Result:%s Path:%s"),ulSerial,iErrorcode,strResult.c_str(),strWavPath.c_str());
    std::lock_guard<std::mutex> lock(m_uploadInfoLock);
    std::map<XINT64, std::shared_ptr<FileUploadInfo> >::iterator it = m_fileUploadMap.find(ulSerial);
    if (it == m_fileUploadMap.end()) {
        YouMe_LOG_Error(__XT("语音回调结束，但是map 中没有:serial:%lld 当前个数:%d"),ulSerial,m_fileUploadMap.size());
         assert(false);
        return;
    }
    if (iErrorcode == 0) {
		youmecommon::CXFile file;
		if (file.LoadFile(strWavPath, youmecommon::CXFile::Mode_OpenExist_ReadOnly) != 0)
		{
			YouMe_LOG_Error(__XT("file not exist %s"), strWavPath.c_str());
			return;
		}
		unsigned int iFileSize = static_cast<unsigned int>(file.GetFileSize());
		file.Close();

		XString srtFileMD5 = youmecommon::CCryptUtil::MD5File(strWavPath);
		XString strSuffix;
		XString::size_type stPos = strWavPath.find_last_of(__XT("."));
		if (stPos != XString::npos)
		{
			strSuffix = strWavPath.substr(stPos + 1);
		}

		it->second->strLocalPath = strWavPath;
		it->second->strMd5 = srtFileMD5;
		it->second->iFileSize = iFileSize;
		it->second->strExtension = strSuffix;
		it->second->extendInfo["AudioText"] = strResult;

		YouMeIMProtocol::YouMeIM_Command_GetUploadToken request;
		request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_GetUploadtoken));
		request.set_areacode(g_ServerZone);
		request.set_md5(XStringToUTF8(srtFileMD5));
		request.set_filesize(iFileSize);
		request.set_suffix(XStringToUTF8(strSuffix));
		YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
		if (errorcode == YouMeIMErrorcode_Success)
		{
			std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_GetUploadtoken));
			pMessage->SetParam("uploadSerial", ulSerial);

			{
				std::lock_guard<std::mutex> lock(m_messageCacheMutex);
				m_messageMap[request.msghead().msgserialnum()] = pMessage;
			}
		}
    }
    else
    {
        //失败了，需要回调上去
        if (NULL != m_pCallback) {
            m_pCallback->OnSendAudioMessageStatus(ulSerial, (YouMeIMErrorcode)iErrorcode, __XT(""), __XT(""), 0);
        }
		m_fileUploadMap.erase(it);
    }
}

void YouMeIMMessageManager::UploadThreadProc()
{
	while (m_uploadSemaphore.Decrement())
	{
		if (m_bExitUpload)
		{
			break;
		}
		std::shared_ptr<FileUploadInfo> uploadInfo;
		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			std::map<XINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.begin();
			for (; itr != m_fileUploadMap.end(); ++itr)
			{
				if (itr->second->httpHead.size() > 0)
				{
					uploadInfo = itr->second;
					break;
				}
			}
		}
		if (uploadInfo.get() == NULL)
		{
			YouMe_LOG_Info(__XT("没有获取到需要上传的信息"));
			continue;
		}

		YouMeIMErrorcode errorCode = YouMeIMErrorcode_Success;
		XString strTmpPath;

		if (uploadInfo->fileType == FileType_Audio)
		{
			//判断一下 文件类型
			FileFormat audioFormat = GetFileFormat(uploadInfo->strLocalPath);
			if (audioFormat == FileFormat_WAV)
			{
				strTmpPath = uploadInfo->strLocalPath + __XT(".amr");
				EncodeWAVEFileToAMRFile(uploadInfo->strLocalPath, strTmpPath, 1, 16);
			}
			else if (audioFormat == FileFormat_AMR)
			{
				strTmpPath = uploadInfo->strLocalPath;
			}
			else
			{
				strTmpPath = uploadInfo->strLocalPath;
				YouMe_LOG_Info(__XT("文件格式居然不对：%d"),audioFormat);
				assert(false);
			}
		}
		
		std::string strResponse;
		YouMe_LOG_Info(__XT("上传文件开始---------------------------"));
		bool bRet = CDownloadUploadManager::UploadFile(uploadInfo->strUrl, strTmpPath, uploadInfo->httpHead, strResponse);
		YouMe_LOG_Info(__XT("上传文件结束---------------------------:%s response:%s"), strTmpPath.c_str(), bRet ? __XT("success") : __XT("failed"));
#ifndef OS_ANDROID
		youmecommon::CXFile::remove_file(strTmpPath);
#endif
		if (bRet)
		{
			CYouMeIMMessage* pMessage = CreateMessage(uploadInfo->chatType);
			if (uploadInfo->fileType == FileType_Audio)
			{
				CYouMeIMMessageBodyAudio* pMsgBody = new CYouMeIMMessageBodyAudio;
				pMsgBody->m_strLocalPath = uploadInfo->strLocalPath;
				pMsgBody->m_strParam = m_strSendAudioParam;
				pMsgBody->m_strMd5 = uploadInfo->strMd5;
				pMsgBody->m_iFileSize = uploadInfo->iFileSize;
				pMsgBody->m_strSuffix = uploadInfo->strExtension;
#ifdef OS_ANDROID
				pMsgBody->m_iTime = SpeechUtil::GetAmrnbTime(uploadInfo->strLocalPath);
#else
				pMsgBody->m_iTime = SpeechUtil::GetWavTime(uploadInfo->strLocalPath);
#endif
				std::map<std::string, youmecommon::CXAny>::const_iterator itr = uploadInfo->extendInfo.find("AudioText");
				if (itr != uploadInfo->extendInfo.end())
				{
					pMsgBody->SetText(youmecommon::CXAny::XAny_Cast<XString>(itr->second));
				}
				pMessage->SetMessageBody(pMsgBody);
			}
			else
			{
				CYouMeIMMessageBodyFile* pMsgBody = new CYouMeIMMessageBodyFile;
				pMsgBody->m_strLocalPath = uploadInfo->strLocalPath;
				pMsgBody->m_strMd5 = uploadInfo->strMd5;
				pMsgBody->m_strExtension = uploadInfo->strExtension;
				pMsgBody->m_iFileSize = uploadInfo->iFileSize;
				pMsgBody->m_fileType = uploadInfo->fileType;
				std::map<std::string, youmecommon::CXAny>::const_iterator itr = uploadInfo->extendInfo.find("Param");
				if (itr != uploadInfo->extendInfo.end())
				{
					pMsgBody->m_strParam = youmecommon::CXAny::XAny_Cast<XString>(itr->second);
				}
				
				pMessage->SetMessageBody(pMsgBody);
			}

			pMessage->SetReceiveID(uploadInfo->strRecvID);
			errorCode = SendMessage(pMessage, NULL, uploadInfo->ulSerial);
			delete pMessage;
		}
		else
		{
			errorCode = YouMeIMErrorcode_UploadFailed;
		}

		if (errorCode != YouMeIMErrorcode_Success && m_pCallback != NULL)
		{
			if (uploadInfo->fileType == FileType_Audio)
			{
				m_pCallback->OnSendAudioMessageStatus(uploadInfo->ulSerial, errorCode, __XT(""), __XT(""), 0);
			}
			else
			{
				m_pCallback->OnSendMessageStatus(uploadInfo->ulSerial, errorCode);
			}
		}

		{
			std::lock_guard<std::mutex> lock(m_uploadInfoLock);
			std::map<XINT64, std::shared_ptr<FileUploadInfo> >::iterator itr = m_fileUploadMap.find(uploadInfo->ulSerial);
			if (itr != m_fileUploadMap.end())
			{
				m_fileUploadMap.erase(itr);
			}
		}
	}
	YouMe_LOG_Info(__XT("Leave"));
}

void YouMeIMMessageManager::UpdateRecvMessageSerial(XINT64 messageSerial)
{
	std::string strSerial = CStringUtilT<char>::to_string(messageSerial);
	XString strMessageSerial = UTF8TOXString(strSerial);
	std::string strKey = CStringUtilT<char>::formatString("LastMsgSerial_%s_%d", XStringToUTF8(m_pIManager->m_strYouMeID).c_str(), g_iYouMeIMMode);
	CProfileDB::Instance().setSetting(strKey, strMessageSerial);
}

YouMeIMErrorcode YouMeIMMessageManager::DownloadAudio(XINT64 ulSerial, const XString& strSavePath)
{
	std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
	std::map<XINT64, FileDownloadInfo>::iterator it = m_downloadInfo.find(ulSerial);
	if (it == m_downloadInfo.end())
	{
		return YouMeIMErrorcode_ParamInvalid;
	}
	it->second.strSavePath = strSavePath;
	m_downloadList.push_back(it->second);
	m_downloadSemaphore.Increment();
	return YouMeIMErrorcode_Success;
}

YouMeIMErrorcode YouMeIMMessageManager::DownloadAudioSync(XINT64 ulSerial, const XString& strSavePath)
{
	XString strDownloadURL;
	{
		std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
		std::map<XINT64, FileDownloadInfo>::iterator it = m_downloadInfo.find(ulSerial);
		if (it == m_downloadInfo.end())
		{
			return YouMeIMErrorcode_ParamInvalid;
		}
		strDownloadURL = it->second.strURL;
	}
	
	XString strTmpPath = strSavePath + __XT(".amr");
	bool bSuccess = CDownloadUploadManager::DownloadFile(strDownloadURL, strTmpPath);
	
	//先简单判断一下文件是否存在，后面需要根据错误码来判断是否下载成功
	if (bSuccess) {
		if (FileFormat_AMR == GetFileFormat(strTmpPath))
		{
			DecodeAMRFileToWAVEFile(strTmpPath, strSavePath);
		}
		else
		{
			youmecommon::CXFile::rename_file(strTmpPath, strSavePath);
		}
	}
	youmecommon::CXFile::remove_file(strTmpPath);

	{
		//删除对应表
		std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
		std::map<XINT64, FileDownloadInfo>::iterator it = m_downloadInfo.find(ulSerial);
		if (it != m_downloadInfo.end())
		{
			m_downloadInfo.erase(it);
		}
	}

	return bSuccess ? YouMeIMErrorcode_Success : YouMeIMErrorcode_PTT_DownloadFail;

}

void YouMeIMMessageManager::SetDownloadCallback(IYouMeIMDownloadCallback* pCallback)
{
	m_pDownloadCallback = pCallback;
}

void YouMeIMMessageManager::DownloadThreadProc()
{
	YouMe_LOG_Info(__XT("Enter"));
	while (m_downloadSemaphore.Decrement())
	{
		if (m_bExitDownload)
		{
			break;
		}
		FileDownloadInfo downloadInfo;
		{
			std::lock_guard<std::mutex> lock(m_DownloadInfoLock);
			if (m_downloadList.empty())
			{
				continue;
			}
			downloadInfo = m_downloadList.front();
			m_downloadList.pop_front();
		}
		XString strTmpPath = downloadInfo.strSavePath + __XT(".amr");
		bool bSuccess = CDownloadUploadManager::DownloadFile(downloadInfo.strURL, strTmpPath);
		if (NULL != m_pDownloadCallback)
		{
            //先简单判断一下文件是否存在，后面需要根据错误码来判断是否下载成功
			if (bSuccess) {
				if (FileFormat_AMR == GetFileFormat(strTmpPath))
				{
					DecodeAMRFileToWAVEFile(strTmpPath, downloadInfo.strSavePath);
				}
				else
				{
					youmecommon::CXFile::rename_file(strTmpPath, downloadInfo.strSavePath);
				}
				//重命名一下文件
                m_pDownloadCallback->OnDownload(downloadInfo.ulSerial, YouMeIMErrorcode_Success, downloadInfo.strSavePath);
            }
			else
            {
                 m_pDownloadCallback->OnDownload(downloadInfo.ulSerial, YouMeIMErrorcode_PTT_DownloadFail, downloadInfo.strSavePath);
            }
		}
		youmecommon::CXFile::remove_file(strTmpPath);
	}
	YouMe_LOG_Info(__XT("Leave"));
}

