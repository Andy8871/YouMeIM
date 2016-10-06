// stdafx.cpp : 只包括标准包含文件的源文件
// CrossPlatformDefine.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "YouMeIMContactManager.h"
#include "YouMeIMManager.h"
void YouMeIMContactManager::SetCallback(IYouMeIMContactCallback* pCallback)
{
	m_pCallback = pCallback;
}

YouMeIMErrorcode YouMeIMContactManager::Init()
{
	if (m_bInit)
	{
		return YouMeIMErrorcode_Success;
	}
	m_pCallback = NULL;
	m_bInit = true;
	return YouMeIMErrorcode_Success;
}

void YouMeIMContactManager::UnInit()
{
	if (!m_bInit)
	{
		return;
	}
	m_pCallback = NULL;
	m_bInit = false;
}

YouMeIMErrorcode YouMeIMContactManager::GetContactInfo(const std::list<XString>& contactIDList)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	if (contactIDList.size() == 0)
	{
		return YouMeIMErrorcode_ParamInvalid;
	}
	//构建请求
	YouMeIMProtocol::YouMeIM_Command_GetContactInfo request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_GetContactInfo));
	std::list<XString>::const_iterator begin  = contactIDList.begin();
	std::list<XString>::const_iterator end = contactIDList.end();
	for (; begin != end;begin++)
	{
		std::string*pAddID =  request.mutable_youmeid()->Add();
		XString strContactID = *begin;
		*pAddID = XStringToUTF8(strContactID);
	}
	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_GetContactInfo));

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

bool YouMeIMContactManager::OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize)
{
    if (NULL == m_pCallback) {
        YouMe_LOG_Warning(__XT("联系人消息回调指针为空，上层是不是忘记设置了"));
    }
	bool bDeal = true;
	//有一些包是需要服务器立即回复的，这些包如果没有回复的话会回调给上层一个超时。如果已经回复了就需要删掉
	bool bCheckResopnse = false;
	switch (head.head().msgtype())
	{
		case YouMeIMProtocol::Command_WasDeleted:
		{
			YouMeIMProtocol::YouMeIM_Command_WasDeleted request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 YouMeIM_Command_WasDeleted 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					m_pCallback->OnContactBeDeleted(UTF8TOXString(request.youmeid()));
				}
			}
		}
		break;
		case YouMeIMProtocol::Command_ContactLogin:
		{
			YouMeIMProtocol::YouMeIM_Command_ContactLogin request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 Command_ContactLogin 失败了,长度：%d"), iBufferSize);
			}
			else
			{
                //判断是否是自己在线了，如果是自己在线那就是被T了
                XString strYouMeID = UTF8TOXString(request.youmeid());
                if (strYouMeID == m_pIManager->m_strYouMeID) {
                    //启动一个线程，用来调用退出
                    YouMe_LOG_Info(__XT("被踢下线了"));
                    std::thread logoutThread(&YouMeIMManager::Logout,m_pIManager);
                    logoutThread.detach();
                }
                else
                {
                    if (NULL != m_pCallback)
                    {
                        m_pCallback->OnContactOnline(UTF8TOXString(request.youmeid()));
                    }
                }
			}
		}
		break;
		case YouMeIMProtocol::Command_ContactLogOut:
		{
			YouMeIMProtocol::YouMeIM_Command_ContactLogin request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 Command_ContactLogOut 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					m_pCallback->OnContactOffline(UTF8TOXString(request.youmeid()));
				}
			}
		}
		break;
		case YouMeIMProtocol::Command_DeleteContactResponse:
		{		
			bCheckResopnse = true;
			//添加联系人的请求已经发出
			YouMeIMProtocol::YouMeIM_Command_DeleteContactResponse request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 YouMeIM_Command_DeleteContactResponse 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					m_pCallback->OnDeleteContactResponse((YouMeIMErrorcode)request.msghead().code(), UTF8TOXString(request.youmeid()));
				}
			}
		}
		break;
		case YouMeIMProtocol::Command_GetContactListResponse:
		{
			bCheckResopnse = true;
			YouMeIMProtocol::YouMeIM_Command_GetContactListResponse request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 YouMeIM_Command_GetContactListResponse 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					std::list<IYouMeIMContactInfo*> pValue;
					//使用一个智能指针来保存数据，自动销毁内存
					std::list<std::shared_ptr<CYouMeIMContactInfo> >tmpList;
					for (int i = 0; i < request.contacts_size(); i++)
					{
						const ::YouMeIMProtocol::ContactInfo& contactInfo = request.contacts(i);
						std::shared_ptr<CYouMeIMContactInfo> pPtr(new CYouMeIMContactInfo);
						pPtr->m_strYouMeID = UTF8TOXString(contactInfo.youmeid());
						pPtr->m_iStatus = contactInfo.status();
						pValue.push_back(pPtr.get());
						tmpList.push_back(pPtr);
					}
					m_pCallback->OnGetContactList((YouMeIMErrorcode)request.msghead().code(), &pValue);
				}
			}
		}
		break;
		case YouMeIMProtocol::Command_GetContactInfoResponse:
		{			
			bCheckResopnse = true;
			//添加联系人的请求已经发出
			YouMeIMProtocol::YouMeIM_Command_GetContactInfoResponse request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 YouMeIM_Command_GetContactInfoResponse 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					std::list<IYouMeIMContactInfo*> pValue;
					//使用一个智能指针来保存数据，自动销毁内存
					std::list<std::shared_ptr<CYouMeIMContactInfo> >tmpList;
					for (int i = 0; i < request.contactinfolist_size(); i++)
					{
						const ::YouMeIMProtocol::ContactInfo& contactInfo = request.contactinfolist(i);
						std::shared_ptr<CYouMeIMContactInfo> pPtr(new CYouMeIMContactInfo);
						pPtr->m_strYouMeID = UTF8TOXString(contactInfo.youmeid());
						pPtr->m_iStatus = contactInfo.status();
						pValue.push_back(pPtr.get());
						tmpList.push_back(pPtr);
					}
					m_pCallback->OnGetContactInfo((YouMeIMErrorcode)request.msghead().code(), &pValue);
				}
			}
		}
		break;
		case YouMeIMProtocol::Command_AddContactRequest:
		{
			//添加联系人的请求已经发出
			YouMeIMProtocol::YouMeIM_Command_AddContactRequest request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 Command_AddContactRequest 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					m_pCallback->OnAddContactRequest((YouMeIMErrorcode)request.msghead().code(), UTF8TOXString(request.youmeid()));
				}
			}
			break;
		}
		break;
		case YouMeIMProtocol::Command_ReceiveInvitedApplication:
		{
			//接收到了好友邀请
			YouMeIMProtocol::YouMeIM_Command_ReceiveContactInvited request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 Command_ReceiveInvitedApplication 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					std::shared_ptr<CYouMeIMContactInfo> pInfo(new CYouMeIMContactInfo);
					pInfo->m_iStatus = request.contactinfo().status();
					pInfo->m_strYouMeID = UTF8TOXString(request.contactinfo().youmeid());
					m_pCallback->OnReceiveContactInvited(pInfo.get(), UTF8TOXString(request.reason()));
				}
			}
			break;
		}
		case YouMeIMProtocol::Command_InvitedResponse:
		{
			//统一了或者拒绝了之后，服务器回的包
			YouMeIMProtocol::YouMeIM_Command_InvitedResponse request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 YouMeIM_Command_InvitedResponse 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
                    if (request.evttype() == YouMeIMProtocol::AddContactEvent_Agree)
					{
						m_pCallback->OnContactInvitedResponse((YouMeIMErrorcode)request.msghead().code(), AddContactEvent_Agree, UTF8TOXString(request.youmeid()));
					}
					else
					{
						m_pCallback->OnContactInvitedResponse((YouMeIMErrorcode)request.msghead().code(), AddContactEvent_Refuse, UTF8TOXString(request.youmeid()));
					}			
				}
			}
			break;
		}
		case  YouMeIMProtocol::Command_AddContactResponse:
		{
			//添加联系人的回复
			YouMeIMProtocol::YouMeIM_Command_AddContactResponse request;
			if (!request.ParseFromArray(pBuffer, iBufferSize))
			{
				//记录一下
				YouMe_LOG_Error(__XT("解析 Command_AddContactResponse 失败了,长度：%d"), iBufferSize);
			}
			else
			{
				if (NULL != m_pCallback)
				{
					std::shared_ptr<CYouMeIMContactInfo> pInfo(new CYouMeIMContactInfo);
					pInfo->m_iStatus = request.contactinfo().status();
					pInfo->m_strYouMeID = UTF8TOXString(request.contactinfo().youmeid());
                    if (request.evttype() == YouMeIMProtocol::AddContactEvent_Agree)
					{						
						m_pCallback->OnAddContactResponse( AddContactEvent_Agree, pInfo.get());
					}
					else
					{
						m_pCallback->OnAddContactResponse(AddContactEvent_Refuse, pInfo.get());
					}
				}
			}
		}
		break;
		default:
			bDeal = false;
			break;
	}
	if (bCheckResopnse)
	{
		//搽除之前保留的关系
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
	return bDeal;
}
bool YouMeIMContactManager::OnRecvPacketTimeOut(XINT64 iUniqueID)
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
		m_messageMap.erase(it);
	}

	switch (messageInfo.command)
	{
	case YouMeIMProtocol::Command_DeleteContact:
	{
		if (NULL != m_pCallback)
		{
			m_pCallback->OnDeleteContactResponse(YouMeIMErrorcode_TimeOut, messageInfo.GetParam("YouMeID", __XT("")));
		}
	}
	break;
	case YouMeIMProtocol::Command_GetContactList:
	{
		if (NULL != m_pCallback)
		{
			m_pCallback->OnGetContactList(YouMeIMErrorcode_TimeOut, NULL);
		}
	}
	break;
	case YouMeIMProtocol::Command_GetContactInfo:
	{
		if (NULL != m_pCallback)
		{
			m_pCallback->OnGetContactInfo(YouMeIMErrorcode_TimeOut, NULL);
		}
	}
	break;
	default:
		YouMe_LOG_Warning(__XT("没有处理的需要服务器回复的超时包:%d"), messageInfo.command);
		break;
	}
	return true;
}


YouMeIMContactManager::YouMeIMContactManager(YouMeIMManager* pIManager) :
IManagerBase(pIManager)
{
	m_bInit = false;
}

YouMeIMErrorcode YouMeIMContactManager::GetContactList()
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	XINT64 ulSerial = -1;
	YouMeIMErrorcode errorcode = m_pIManager->SendCommnadPacket<YouMeIMProtocol::YouMeIM_Command_GetContactList>(YouMeIMProtocol::Command_GetContactList, ulSerial);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_GetContactList));
		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[ulSerial] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMContactManager::AddContact(const XString& strContactID, const XString& strReason)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_AddContact request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_AddContact));
	request.set_youmeid(XStringToUTF8(strContactID));
	request.set_reason(XStringToUTF8(strReason));

	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_AddContact));
		pMessage->SetParam("YouMeID", strContactID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
	
}

YouMeIMErrorcode YouMeIMContactManager::AgreeContactInvited(const XString& strContactID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_AgreeContactInvited request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_AgreeContactInvited));
	request.set_youmeid(XStringToUTF8(strContactID));

	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_AgreeContactInvited));
		pMessage->SetParam("YouMeID", strContactID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMContactManager::RefuseContactInvited(const XString& strContactID, const XString& strReason)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_RefuseContactInvited request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_RefuseContactInvited));
	request.set_youmeid(XStringToUTF8(strContactID));
	request.set_reason(XStringToUTF8(strReason));
	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_RefuseContactInvited));
		pMessage->SetParam("YouMeID", strContactID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMContactManager::DeleteContact(const XString&strContactID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_DeleteContact request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_DeleteContact));
	request.set_youmeid(XStringToUTF8(strContactID));
	
	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_DeleteContact));
		pMessage->SetParam("YouMeID", strContactID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMContactManager::KickoutBlacklist(const XString&strContactID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	return YouMeIMErrorcode_Success;
}

YouMeIMErrorcode YouMeIMContactManager::AddToBlacklist(const XString& strContactID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	return YouMeIMErrorcode_Success;
}

YouMeIMErrorcode YouMeIMContactManager::GetBlacklistList()
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}
	return YouMeIMErrorcode_Success;
}


//具体联系人的信息
XString CYouMeIMContactInfo::GeYouMeID()
{
	return m_strYouMeID;
}

int CYouMeIMContactInfo::GetStatus()
{
	return m_iStatus;
}
