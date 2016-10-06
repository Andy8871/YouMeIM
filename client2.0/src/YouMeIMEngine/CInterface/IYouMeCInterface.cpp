//
//  IYouMeCInterface.cpp
//  youme_voice_engine
//
//  Created by YouMe.im on 15/12/10.
//  Copyright © 2015年 tencent. All rights reserved.
//

#include "IYouMeCInterface.h"
#include <YouMeIMEngine/pb/im_common.pb.h>
#include <YouMeCommon/Log.h>
#include <memory>
#include <mutex>
#include <YouMeCommon/XAny.h>
#include <YouMeCommon/XSharedArray.h>
#include <YouMeCommon/XVariant.h>
#include <YouMeCommon/json/json.h>
#include <YouMeCommon/XSemaphore.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeIMEngine/YouMeIMMessageManager.h>
bool s_YouMeInit = false;

IYouMeIMManager* g_pIMManager = NULL;
std::mutex g_contentMutex;
youmecommon::CXSemaphore g_messageSemap;
//用一个map 保存解析器，根据不同的命令定制解析器的内容。
struct ParserContent
{
	ParserContent(YouMeIMProtocol::Command_TYPE commandType)
	{
		m_commandType = commandType;
        m_errorcode = YouMeIMErrorcode_Success;
	}
	//协议
	YouMeIMProtocol::Command_TYPE m_commandType;
	//按顺序保存值
	std::vector<youmecommon::CXAny> m_values;
	//错误码
	YouMeIMErrorcode m_errorcode;
};

std::list<XString> g_parserList;
bool InsertToMessageList(const std::shared_ptr<ParserContent>& content)
{
    youmecommon::Value jsonRoot;
    jsonRoot["Command"] = content->m_commandType;
    jsonRoot["Errorcode"] = content->m_errorcode;
	YouMe_LOG_Info(__XT("插入消息到队列： 目前消息个数:%d command:%d  code:%d"), g_parserList.size(),content->m_commandType, content->m_errorcode);
    switch (content->m_commandType) {
        case YouMeIMProtocol::Commnad_Chat_Voice:
        {
            jsonRoot["RequestID"] = (int)youmecommon::CXAny::XAny_Cast<XINT64>(content->m_values[0]);
            jsonRoot["Text"] = content->m_values[1].Get();
			jsonRoot["LocalPath"] = content->m_values[2].Get(); 
			jsonRoot["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[3]);
        }
        break;
        case YouMeIMProtocol::Command_Download:
        {
            jsonRoot["Serial"] = (int)youmecommon::CXAny::XAny_Cast<XINT64>(content->m_values[0]);
            jsonRoot["SavePath"] = content->m_values[1].Get();
        }
            break;
        case YouMeIMProtocol::Command_Login:
        {
            if (YouMeIMErrorcode_Success == content->m_errorcode) {
                jsonRoot["YouMeID"] = content->m_values[0].Get();
                jsonRoot["Status"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]);
            }
        }
            break;
        case YouMeIMProtocol::Command_AddContactRequest:
        case YouMeIMProtocol::Command_WasDeleted:
        case YouMeIMProtocol::Command_AddedToBlacklist:
        case YouMeIMProtocol::Command_KickoutedFromBlacklist:
        case YouMeIMProtocol::Command_DeleteContactResponse:
        case YouMeIMProtocol::Command_AddToBlacklistResponse:
        case YouMeIMProtocol::Command_KickoutFromBlacklistResponse:
        case YouMeIMProtocol::Command_ContactLogin:
        case YouMeIMProtocol::Command_ContactLogOut:
        {
            jsonRoot["YouMeID"] = content->m_values[0].Get();
            
        }
            break;
        case YouMeIMProtocol::Commnad_SendMessage:
        {
            jsonRoot["RequestID"] = (int)youmecommon::CXAny::XAny_Cast<XINT64>(content->m_values[0]);
        }
            break;
        case YouMeIMProtocol::Commnad_RecvMessage:
        {
            jsonRoot["ChatType"] = youmecommon::CXAny::XAny_Cast<YouMeIMChatType>(content->m_values[0]);
            jsonRoot["SenderID"] = content->m_values[1].Get();
            jsonRoot["ReceiveID"] = content->m_values[2].Get();
            jsonRoot["Serial"] = (int) youmecommon::CXAny::XAny_Cast<XINT64>(content->m_values[3]);
            YouMeIMMessageBodyType bodyType = youmecommon::CXAny::XAny_Cast<YouMeIMMessageBodyType>(content->m_values[4]);
            jsonRoot["MessageType"] = bodyType;
            if(bodyType == MessageBodyType_TXT)
            {
                jsonRoot["Content"] = content->m_values[5].Get();
            }
            else if(bodyType == MessageBodyType_CustomMesssage)
            {
                //对二进制数据进行base64
                std::string strBase64Output;
                youmecommon::CCryptUtil::Base64Encoder(youmecommon::CXAny::XAny_Cast<std::string>(content->m_values[5]), strBase64Output);
                jsonRoot["Content"] = strBase64Output;
                
            }
            else if(bodyType == MessageBodyType_Voice)
            {
                 jsonRoot["Text"] = content->m_values[5].Get();
				 jsonRoot["Param"] = content->m_values[6].Get();
				 jsonRoot["Duration"] = (int)youmecommon::CXAny::XAny_Cast<unsigned int>(content->m_values[7]);
            }
        }
            break;
        case YouMeIMProtocol::Command_JoinGroupRequest:
        {
            jsonRoot["GroupID"] = content->m_values[0].Get();
            
        }
            break;
        case YouMeIMProtocol::Command_JoinGroupResponse:
        {
            jsonRoot["GroupEvt"] = youmecommon::CXAny::XAny_Cast<YouMeIMGroupEvent>(content->m_values[0]);
            if (content->m_values.size() >= 4) {
                jsonRoot["GroupID"] = content->m_values[1].Get();
                jsonRoot["GroupName"] = content->m_values[2].Get();
                jsonRoot["GroupType"] = youmecommon::CXAny::XAny_Cast<YouMeIMGroupType>(content->m_values[3]);
            }
        }
            break;
        case YouMeIMProtocol::Command_AddContactResponse:
        {
            jsonRoot["EvtType"] =	youmecommon::CXAny::XAny_Cast<YouMeIMContactEvent>(content->m_values[0]);
            if (content->m_values.size() == 3) {
                jsonRoot["YouMeID"] = content->m_values[1].Get();
                jsonRoot["Status"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[2]);

            }
        }
            break;
        case YouMeIMProtocol::Command_ReceiveInvitedApplication:
        {
            jsonRoot["YouMeID"] = content->m_values[0].Get();
            jsonRoot["Status"] = youmecommon::CXAny::XAny_Cast<int>(content->m_values[1]);
            jsonRoot["Reason"] = content->m_values[2].Get();
        }
            break;
        case YouMeIMProtocol::Command_InvitedResponse:
        {
            
            jsonRoot["EvtType"] = youmecommon::CXAny::XAny_Cast<YouMeIMContactEvent>(content->m_values[0]);
            jsonRoot["YouMeID"] = content->m_values[1].Get();
        }
            break;

		case YouMeIMProtocol::Command_LogOut:
		{

		}
		break;
        case YouMeIMProtocol::Command_GetContactList:
        case YouMeIMProtocol::Command_GetContactInfo:
        case YouMeIMProtocol::Command_GetBlacklistList:
        {
            youmecommon::Value contactList(youmecommon::arrayValue);
            for (size_t i=0; i<content->m_values.size(); i++) {
                std::vector<youmecommon::CXAny> contact = youmecommon::CXAny::XAny_Cast<std::vector<youmecommon::CXAny> >(content->m_values[i]);
                youmecommon::Value contactJson;
                contactJson["YouMeID"] = contact[0].Get();
                contactJson["Status"] = youmecommon::CXAny::XAny_Cast<int>(contact[1]);
                
                
                contactList[(int)i] = contactJson;
            }
            jsonRoot["Contacts"] = contactList;
        }
            break;
        default:
            YouMe_LOG_Warning(__XT("没有处理的json 数据:%d"),content->m_commandType);
            break;
    }
    
    {
        std::lock_guard<std::mutex> lock(g_contentMutex);
        g_parserList.push_back(jsonRoot.toSimpleString());
    }
    g_messageSemap.Increment();
    return true;
}
//这里返回的是一个常量的地址，注意不能把数据从 list 中弹出，否则c# 接受的数据会出错。
const XCHAR* HeadMessage()
{
    std::lock_guard<std::mutex> lock(g_contentMutex);
    YouMe_LOG_Info(__XT("当前消息个数:%d"),g_parserList.size());
    if (g_parserList.size() == 0) {
        return NULL;
    }
    return g_parserList.begin()->c_str();
}

//清空消息
void CleanMessage()
{
    std::lock_guard<std::mutex> lock(g_contentMutex);
    g_parserList.clear();
}

void PopHeadMessage()
{
    std::lock_guard<std::mutex> lock(g_contentMutex);
    if (g_parserList.size() == 0) {
        return ;
    }
    g_parserList.pop_front();
}


class InterCallback :public IYouMeIMLoginCallback,
	public IYouMeIMContactCallback,
	public IYouMeIMMessageCallback,
	public IYouMeIMGroupCallback,
    public IYouMeIMDownloadCallback
{
public:
    virtual ~InterCallback()
    {
        
    }
public:
	//所有的回掉都是把东西按顺序组合好之后塞进去，根据每种命令的类型确定数据类型
	virtual void OnLogin(YouMeIMErrorcode errorcode, IYouMeIMContactInfo* pUserInfo) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_Login));
		content->m_errorcode = errorcode;
		if ((errorcode == YouMeIMErrorcode_Success) && (NULL != pUserInfo))
		{
			content->m_values.push_back(pUserInfo->GeYouMeID());
			content->m_values.push_back(pUserInfo->GetStatus());
		}
		
        InsertToMessageList(content);
	}

	virtual void OnLogout(YouMeIMErrorcode errorcode) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_LogOut));
		content->m_errorcode = errorcode;
		 InsertToMessageList(content);
	}

	virtual void OnGetContactInfo(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_GetContactInfo));
		content->m_errorcode = errorcode;
		if ((errorcode == YouMeIMErrorcode_Success) && (NULL != pValue))
		{
			std::list<IYouMeIMContactInfo*>::const_iterator begin = pValue->begin();
			std::list<IYouMeIMContactInfo*>::const_iterator end = pValue->end();
			for (; begin != end;begin++)
			{
				std::vector<youmecommon::CXAny> contactValues;
				contactValues.push_back((*begin)->GeYouMeID());
				contactValues.push_back((*begin)->GetStatus());

				content->m_values.push_back(contactValues);
			}
		}
		 InsertToMessageList(content);
    }

	virtual void OnGetContactList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_GetContactList));
		content->m_errorcode = errorcode;
		if ((errorcode == YouMeIMErrorcode_Success) && (NULL != pValue))
		{
			std::list<IYouMeIMContactInfo*>::const_iterator begin = pValue->begin();
			std::list<IYouMeIMContactInfo*>::const_iterator end = pValue->end();
			for (; begin != end; begin++)
			{
				std::vector<youmecommon::CXAny> contactValues;
				contactValues.push_back((*begin)->GeYouMeID());
				contactValues.push_back((*begin)->GetStatus());

				content->m_values.push_back(contactValues);
			}
		}
		 InsertToMessageList(content);
	}
    void OnDownload(XINT64 ulSerial, YouMeIMErrorcode errorcode,const XString& strSavePath) override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_Download));
        content->m_errorcode = errorcode;
        content->m_values.push_back(ulSerial);
        content->m_values.push_back(strSavePath);
        InsertToMessageList(content);
    }
	virtual void OnGetBlacklistList(YouMeIMErrorcode errorcode, const std::list<IYouMeIMContactInfo*>* pValue) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_GetBlacklistList));
		content->m_errorcode = errorcode;
		if ((errorcode == YouMeIMErrorcode_Success) && (NULL != pValue))
		{
			std::list<IYouMeIMContactInfo*>::const_iterator begin = pValue->begin();
			std::list<IYouMeIMContactInfo*>::const_iterator end = pValue->end();
			for (; begin != end; begin++)
			{
				std::vector<youmecommon::CXAny> contactValues;
				contactValues.push_back((*begin)->GeYouMeID());
				contactValues.push_back((*begin)->GetStatus());

				content->m_values.push_back(contactValues);
			}
		}
		 InsertToMessageList(content);
	}

	virtual void OnAddContactRequest(YouMeIMErrorcode errorcode, const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_AddContactRequest));
		content->m_errorcode = errorcode;
        content->m_values.push_back(strYouMeID);
        InsertToMessageList(content);
	}

	virtual void OnAddContactResponse(YouMeIMContactEvent evtType, IYouMeIMContactInfo* pContactInfo) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_AddContactResponse));
		content->m_values.push_back(evtType);
		if (NULL != pContactInfo)
		{
			content->m_values.push_back(pContactInfo->GeYouMeID());
			content->m_values.push_back(pContactInfo->GetStatus());
		}
		 InsertToMessageList(content);
	}

	virtual void OnReceiveContactInvited(IYouMeIMContactInfo*pContactInfo, const XString& strReason) override
	{
		assert(NULL != pContactInfo);
		if (pContactInfo == NULL)
		{
			return;
		}
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_ReceiveInvitedApplication));
		content->m_values.push_back(pContactInfo->GeYouMeID());
		content->m_values.push_back(pContactInfo->GetStatus());
		content->m_values.push_back(strReason);
		
		 InsertToMessageList(content);
    }

	virtual void OnContactInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMContactEvent evtType, const XString&strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_InvitedResponse));
		content->m_errorcode = errorcode;
		content->m_values.push_back(evtType);
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnContactBeDeleted(const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_WasDeleted));
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnContactAddedBlacklist(const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_AddedToBlacklist));
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnKickoutedBlacklist(const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_KickoutedFromBlacklist));
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnDeleteContactResponse(YouMeIMErrorcode errorcode, const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_DeleteContactResponse));
		content->m_errorcode = errorcode;
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnAddBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_AddToBlacklistResponse));
		content->m_errorcode = errorcode;
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnKickoutBlacklistResponse(YouMeIMErrorcode errorcode, const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_KickoutFromBlacklistResponse));
		content->m_errorcode = errorcode;
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnContactOnline(const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_ContactLogin));
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}

	virtual void OnContactOffline(const XString& strYouMeID) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_ContactLogOut));
		content->m_values.push_back(strYouMeID);

		 InsertToMessageList(content);
	}
	virtual void OnSendAudioMessageStatus(XINT64 iRequestID, YouMeIMErrorcode errorcode, const XString&strText, const XString& strAudioPath, unsigned int audioTime)override
    {
        std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Commnad_Chat_Voice));
        content->m_errorcode = errorcode;
        content->m_values.push_back(iRequestID);
        content->m_values.push_back(strText);
        content->m_values.push_back(strAudioPath);
		content->m_values.push_back(audioTime);
        InsertToMessageList(content);
    }
	virtual void OnSendMessageStatus(XINT64 iRequestID, YouMeIMErrorcode errorcode) override
	{
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Commnad_SendMessage));
		content->m_errorcode = errorcode;
		content->m_values.push_back(iRequestID);
		 InsertToMessageList(content);
	}

	virtual void OnRecvMessage(IYouMeIMMessage* pMessage) override
	{
		assert(NULL != pMessage);
		if (NULL == pMessage)
		{
			return;
		}
		std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Commnad_RecvMessage));
		content->m_values.push_back(pMessage->GetChatType());
		content->m_values.push_back(pMessage->GetSenderID());
		content->m_values.push_back(pMessage->GetReceiveID());
        content->m_values.push_back(pMessage->GetMessageID());
		IYouMeIMMessageBodyBase* pbodyBase = pMessage->GetMessageBody();
        content->m_values.push_back(pbodyBase->GetMessageType());
		switch (pbodyBase->GetMessageType())
		{
		case MessageBodyType_TXT:
		{
			IYouMeIMMessageBodyText* pContent = (IYouMeIMMessageBodyText*)pbodyBase;
			content->m_values.push_back(pContent->GetMessageContent());
		}
		break;
		case MessageBodyType_CustomMesssage:
		{
			IYouMeIMMessageBodyCustom* pContent = (IYouMeIMMessageBodyCustom*)pbodyBase;
			content->m_values.push_back(pContent->GetCustomMessage());
		}
		break;
        case MessageBodyType_Voice:
        {
            IYouMeIMMessageBodyAudio* pContent = (IYouMeIMMessageBodyAudio*)pbodyBase;
            content->m_values.push_back(pContent->GetText());
			content->m_values.push_back(pContent->GetParam());
			content->m_values.push_back(pContent->GetAudioTime());
        }
            break;
		default:
			assert(false);
			break;
		}
		 InsertToMessageList(content);
	}

	virtual void OnGetAllGroup(YouMeIMErrorcode errorcode, const std::list<IYouMeIMGroupInfo*>* groupInfoList) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnCreateGroup(YouMeIMErrorcode errorcode, XINT64 iRequestID, IYouMeIMGroupInfo* pGroupInfo) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnDestroyGroup(YouMeIMErrorcode errorcode, const XString& strGroupID) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnGroupDestroyed(const XString& strGroupID) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnGetGroupMembers(YouMeIMErrorcode errorcode, const XString& strGroupID, const std::list<IYouMeIMGroupMemberInfo*>* groupMembers) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnJoinGroupRequest(YouMeIMErrorcode errorcode, const XString& strGroupID) override
	{
        std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_JoinGroupRequest));
        content->m_errorcode = errorcode;
        content->m_values.push_back(strGroupID);
        InsertToMessageList(content);

	}

	virtual void OnJoinGroupResponse(YouMeIMErrorcode errorcode, YouMeIMGroupEvent evtType, IYouMeIMGroupInfo* pGroupInfo) override
	{
        std::shared_ptr<ParserContent> content(new ParserContent(YouMeIMProtocol::Command_JoinGroupResponse));
        content->m_errorcode = errorcode;
        content->m_values.push_back(evtType);
        if (NULL != pGroupInfo) {
            content->m_values.push_back(pGroupInfo->GetGroupID());
            content->m_values.push_back(pGroupInfo->GetGroupName());
            content->m_values.push_back(pGroupInfo->GetGroupType());
        }
        InsertToMessageList(content);
	}

	virtual void OnReceiveGroupInvited(const XString&strGroupID, IYouMeIMContactInfo* pContact, const XString& strReason) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnGroupInvitedResponse(YouMeIMErrorcode errorcode, YouMeIMGroupEvent evtType, const XString&strGroupID, const XString& strYouMeID) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnOtherJoinGroup(const XString&strGroupID, IYouMeIMGroupMemberInfo* pMemberInfo) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnKickMemberResponse(YouMeIMErrorcode errorcode, const XString&strGroupID, const XString&strYouMeID) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnOtherKickedGroup(const XString&strGroupID, const XString& strYouMeID) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnLeaveGroupResponse(YouMeIMErrorcode errorcode, const XString&strGroupID) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void OnOtherLeaveGroup(const XString&strGroupID, const XString& strYouMeID) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

};
//一个全局的回掉
InterCallback* g_InterCallback = NULL;

int IM_Init(const XCHAR* pszAppKey, const XCHAR* pszSecrect)
{
	if ((NULL == pszAppKey) || (NULL == pszSecrect))
	{
		return YouMeIMErrorcode_ParamInvalid;
	}
	if (s_YouMeInit)
	{
		return YouMeIMErrorcode_Success;
	}
	g_parserList.clear();
	IYouMeIMManager::Startup();
	g_pIMManager = IYouMeIMManager::CreateInstance();
	YouMeIMErrorcode errorcode = g_pIMManager->Init(pszAppKey, pszSecrect, __XT(""));
	if (errorcode == YouMeIMErrorcode_Success)
	{
		g_InterCallback = new InterCallback;
		s_YouMeInit = true;
	}

	return errorcode;
}

void IM_Uninit()
{
	if (!s_YouMeInit)
	{
		return;
	}
    g_messageSemap.Increment();
	g_pIMManager->UnInit();
	IYouMeIMManager::DestroyInstance(g_pIMManager);
	g_pIMManager = NULL;
	IYouMeIMManager::CleanUp();
	delete g_InterCallback;
	g_InterCallback = NULL;
	g_parserList.clear();
    s_YouMeInit = false;
}

int IM_GetContactList()
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMContactManager* pContactManager = g_pIMManager->GetContactManager();
    if (NULL == pContactManager) {
        return YouMeIMErrorcode_NotLogin;
    }
	return pContactManager->GetContactList();
}
int IM_AddContact(const XCHAR* pszYouMeID,const XCHAR* pszReason)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMContactManager* pContactManager = g_pIMManager->GetContactManager();
    if (NULL == pContactManager) {
        return YouMeIMErrorcode_NotLogin;
    }
    return  pContactManager->AddContact(pszYouMeID, pszReason);
}
int IM_AgreeContactInvited(const XCHAR* pszYouMeID)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMContactManager* pContactManager = g_pIMManager->GetContactManager();
    if (NULL == pContactManager) {
        return YouMeIMErrorcode_NotLogin;
    }
    return pContactManager->AgreeContactInvited(pszYouMeID);
}
int IM_RefuseContactInvited(const XCHAR* pszYouMeID,const XCHAR* pszReason)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMContactManager* pContactManager = g_pIMManager->GetContactManager();
    if (NULL == pContactManager) {
        return YouMeIMErrorcode_NotLogin;
    }
    return pContactManager->RefuseContactInvited(pszYouMeID, pszReason);
}
int IM_DeleteContact(const XCHAR* pszYouMeID)
{
	if (NULL == pszYouMeID)
	{
		return YouMeIMErrorcode_ParamInvalid;
	}
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMContactManager* pContactManager = g_pIMManager->GetContactManager();
    if (NULL == pContactManager) {
        return YouMeIMErrorcode_NotLogin;
    }
	return pContactManager->DeleteContact(pszYouMeID);
}
int IM_GetContactOfflineMessage()
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    return pMessageManger->GetContactOfflineMessage();
}
int IM_SendTextMessage(const XCHAR* strRecvID, YouMeIMChatType chatType, const XCHAR*  strContent, int *pRequestID)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
        return YouMeIMErrorcode_NotLogin;
    }
    XINT64 ulSerial = 0;
    YouMeIMErrorcode errorcode = pMessageManger->SendTextMessage(strRecvID, chatType, strContent, &ulSerial);
    if (errorcode == YouMeIMErrorcode_Success) {
        *pRequestID = (int)ulSerial;
    }
    return errorcode;
}
int IM_SendCustomMessage(const XCHAR* strRecvID, YouMeIMChatType chatType, const char* pszCustomMessage,int iBufferLen, int *pRequestID)
{
     if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YouMeIMErrorcode_NotLogin;
    }
    XINT64 ulSerial = 0;
    std::string strCustomMsg;
    strCustomMsg.append(pszCustomMessage,iBufferLen);
    YouMeIMErrorcode errorcode = pMessageManger->SendCustomMessage(strRecvID, chatType, strCustomMsg, &ulSerial);
    if (errorcode == YouMeIMErrorcode_Success) {
        *pRequestID = (int)ulSerial;
    }
    return errorcode;
}
int IM_Logout()
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    CleanMessage();
    g_messageSemap.Increment();
	return g_pIMManager->Logout();
}

int IM_Login(const XCHAR* pszYouMeID, const XCHAR* pszPasswd, const XCHAR* pszOldPasswd)
{
	if ((NULL == pszYouMeID) || (NULL == pszPasswd) || (NULL == pszOldPasswd))
	{
		return YouMeIMErrorcode_ParamInvalid;
	}
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    g_pIMManager->SetLoginCallback(g_InterCallback);
    g_pIMManager->SetContactCallback(g_InterCallback);
    g_pIMManager->SetMessageCallback(g_InterCallback);
    g_pIMManager->SetGroupCallback(g_InterCallback);
    g_pIMManager->SetDownloadCallback(g_InterCallback);
    
    YouMeIMErrorcode errorcode = g_pIMManager->Login(pszYouMeID, pszPasswd, pszOldPasswd);
    if (errorcode == YouMeIMErrorcode_Success) {
        CleanMessage();
    }
    return errorcode;
}

 int IM_JoinChatRoom(const XCHAR*pszID)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMGroupManager* pGroupManger = g_pIMManager->GetGroupManager();
    if (NULL == pGroupManger) {
		return YouMeIMErrorcode_NotLogin;
    }
   return pGroupManger->JoinChatRoom(pszID);
}
 int IM_LeaveChatRoom(const XCHAR*pszID)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMGroupManager* pGroupManger = g_pIMManager->GetGroupManager();
    if (NULL == pGroupManger) {
		return YouMeIMErrorcode_NotLogin;
    }
    return pGroupManger->LeaveChatRoom(pszID);
}

const XCHAR* IM_GetMessage()
{
    g_messageSemap.Decrement();
    return HeadMessage();
}

void IM_PopMessage()
{
    PopHeadMessage();
}

int IM_SendAudioMessage(const XCHAR* pszRecvID, YouMeIMChatType chatType, int *pRequestID)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YouMeIMErrorcode_NotLogin;
    }
    XINT64 ulSerial = 0;
	YouMeIMErrorcode errorcode = pMessageManger->SendAudioMessage(pszRecvID, chatType, &ulSerial);
    if (errorcode == YouMeIMErrorcode_Success) {
        *pRequestID = (int)ulSerial;
    }
    return errorcode;

}


int IM_SendOnlyAudioMessage(const XCHAR* pszRecvID, YouMeIMChatType chatType,int *pRequestID)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
        return YouMeIMErrorcode_NotLogin;
    }
    XINT64 ulSerial = 0;
    YouMeIMErrorcode errorcode = pMessageManger->SendOnlyAudioMessage(pszRecvID, chatType, &ulSerial);
    if (errorcode == YouMeIMErrorcode_Success) {
        *pRequestID = (int)ulSerial;
    }
    return errorcode;
    
}

YOUMEDLL_API int IM_DownloadAudioFileSync(int iSerial, const XCHAR* pszSavePath)
{
	if (NULL == g_pIMManager) {
		return YouMeIMErrorcode_EngineNotInit;
	}
	YouMeIMMessageManager* pMessageManger = (YouMeIMMessageManager*)g_pIMManager->GetMessageManager();
	if (NULL == pMessageManger) {
		return YouMeIMErrorcode_NotLogin;
	}
	return pMessageManger->DownloadAudioSync(iSerial, pszSavePath);
}

int IM_DownloadAudioFile(int iSerial,const XCHAR* pszSavePath)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YouMeIMErrorcode_NotLogin;
    }
    return pMessageManger->DownloadAudio(iSerial,pszSavePath);
}

int IM_StopAudioMessage(const XCHAR* pszParam)
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YouMeIMErrorcode_NotLogin;
    }
    
	return (int)pMessageManger->StopAudioMessage(pszParam);
}
 int IM_CancleAudioMessage()
{
    if (NULL == g_pIMManager) {
        return YouMeIMErrorcode_EngineNotInit;
    }
    IYouMeIMMessageManager* pMessageManger = g_pIMManager->GetMessageManager();
    if (NULL == pMessageManger) {
		return YouMeIMErrorcode_NotLogin;
    }
    return (int)pMessageManger->CancleAudioMessage();

}

int IM_GetSDKVer()
{
    return IYouMeIMManager::GetSDKVersion();
}
void IM_SetMode(int iMode)
{
    IYouMeIMManager::SetMode(iMode);
}
YOUMEDLL_API XCHAR* IM_GetFilterText(const XCHAR* pszSource)
{
	XString strResult = IYouMeIMManager::FilterKeyword(pszSource);
	XCHAR* pResult = new XCHAR[strResult.length()+1];
	XStrCpy(pResult,strResult.c_str());
	return pResult;
}

YOUMEDLL_API void IM_DestroyFilterText(XCHAR* pszSource)
{
	delete[] pszSource;
}

YOUMEDLL_API void IM_SetServerZone(ServerZone zone)
{
	IYouMeIMManager::SetServerZone(zone);
}

YOUMEDLL_API void IM_SetAudioCacheDir(const XCHAR* pszAudioCacheDIR)
{
	IYouMeIMManager::SetAudioCacheDir(pszAudioCacheDIR);
}
