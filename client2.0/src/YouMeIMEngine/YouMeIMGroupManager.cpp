// stdafx.cpp : 只包括标准包含文件的源文件
// CrossPlatformDefine.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息


#include "YouMeIMGroupManager.h"
#include <YouMeCommon/StringUtil.hpp>
#include "YouMeIMManager.h"
YouMeIMErrorcode YouMeIMGroupManager::Init()
{
	if (m_bInit)
	{
		return YouMeIMErrorcode_Success;
	}
	m_pCallback = NULL;
	m_bInit = true;
	return YouMeIMErrorcode_Success;
}

void YouMeIMGroupManager::UnInit()
{
	if (!m_bInit)
	{
		return;
	}
	m_pCallback = NULL;
	m_bInit = false;
}


bool YouMeIMGroupManager::OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize)
{
	if (NULL == m_pCallback) {
		YouMe_LOG_Warning(__XT("回调指针为空，上层是不是忘记设置了"));
	}
	bool bDeal = true;
	//有一些包是需要服务器立即回复的，这些包如果没有回复的话会回调给上层一个超时。如果已经回复了就需要删掉
	bool bCheckResopnse = false;
	switch (head.head().msgtype())
	{
	case YouMeIMProtocol::Command_DestroyGroup:
	{
		bCheckResopnse = true;
		YouMeIMProtocol::YouMeIM_Command_DestroyGroupResponse request;
		if (!request.ParseFromArray(pBuffer, iBufferSize))
		{
			//记录一下
			YouMe_LOG_Error(__XT("解析 YouMeIM_Command_DestroyGroupResponse 失败了,长度：%d"), iBufferSize);
		}
		else
		{
			if (NULL != m_pCallback)
			{
				m_pCallback->OnDestroyGroup((YouMeIMErrorcode)request.msghead().code(), UTF8TOXString(request.groupid()));
			}
		}
	}
	break;
	case YouMeIMProtocol::Command_GroupDestroyed:
	{
		YouMeIMProtocol::YouMeIM_Command_GroupDestroyed request;
		if (!request.ParseFromArray(pBuffer, iBufferSize))
		{
			//记录一下
			YouMe_LOG_Error(__XT("解析 Command_GroupDestroyed 失败了,长度：%d"), iBufferSize);
		}
		else
		{
			if (NULL != m_pCallback)
			{
				m_pCallback->OnGroupDestroyed(UTF8TOXString(request.groupid()));
			}
		}
	}
	break;
	case YouMeIMProtocol::Command_GetGroupMembers:
	{
		YouMeIMProtocol::YouMeIM_Command_GetGroupMembersResponse request;
		if (!request.ParseFromArray(pBuffer, iBufferSize))
		{
			//记录一下
			YouMe_LOG_Error(__XT("解析 YouMeIM_Command_GetGroupMembersResponse 失败了,长度：%d"), iBufferSize);
		}
		else
		{
			if (NULL != m_pCallback)
			{
				std::list<IYouMeIMGroupMemberInfo*> pValue;
				//使用一个智能指针来保存数据，自动销毁内存
				std::list<std::shared_ptr<IYouMeIMGroupMemberInfo> >tmpList;
				for (int i = 0; i < request.memberinfo_size(); i++)
				{
					const ::YouMeIMProtocol::YouMeIM_MemberInfo& memberInfo = request.memberinfo(i);
					std::shared_ptr<CYouMeIMGroupMemberInfo> pPtr(new CYouMeIMGroupMemberInfo);
					pPtr->m_strYouMeID = UTF8TOXString(memberInfo.youmeid());
					pPtr->m_status = (ContactStatus)memberInfo.status();
					pPtr->m_permission = (YouMeIMGroupPermission)memberInfo.memberpower();
					pValue.push_back(pPtr.get());
					tmpList.push_back(pPtr);
				}
				m_pCallback->OnGetGroupMembers((YouMeIMErrorcode)request.msghead().code(),UTF8TOXString(request.groupid()),&pValue);
			}
		}
	}
	break;
	case YouMeIMProtocol::Command_GetAllGroup:
	{
		bCheckResopnse = true;
		YouMeIMProtocol::YouMeIM_Command_GetAllGroupResponse request;
		if (!request.ParseFromArray(pBuffer, iBufferSize))
		{
			//记录一下
			YouMe_LOG_Error(__XT("解析 YouMeIM_Command_GetAllGroupResponse 失败了,长度：%d"), iBufferSize);
		}
		else
		{
			if (NULL != m_pCallback)
			{
				std::list<IYouMeIMGroupInfo*> pValue;
				//使用一个智能指针来保存数据，自动销毁内存
				std::list<std::shared_ptr<IYouMeIMGroupInfo> >tmpList;
				for (int i = 0; i < request.groupinfos_size(); i++)
				{
					const ::YouMeIMProtocol::YouMeIM_GroupInfo& groupInfo = request.groupinfos(i);
					std::shared_ptr<CYouMeIMGroupInfo> pPtr(new CYouMeIMGroupInfo);
					pPtr->m_strGroupID = UTF8TOXString(groupInfo.groupid());
					pPtr->m_strGroupName = UTF8TOXString(groupInfo.groupname());
					pPtr->m_groupType = (YouMeIMGroupType)groupInfo.grouptype();
					pValue.push_back(pPtr.get());
					tmpList.push_back(pPtr);
				}
				
				m_pCallback->OnGetAllGroup((YouMeIMErrorcode)request.msghead().code(),&pValue);
			}
		}
	}
	break;
	case YouMeIMProtocol::Command_CreateGroup:
	{
		bCheckResopnse = true;
		YouMeIMProtocol::YouMeIM_Command_CreateGroupResponse request;
		if (!request.ParseFromArray(pBuffer, iBufferSize))
		{
			//记录一下
			YouMe_LOG_Error(__XT("解析 YouMeIM_Command_CreateGroupResponse 失败了,长度：%d"), iBufferSize);
		}
		else
		{
			if (NULL != m_pCallback)
			{
				std::shared_ptr<CYouMeIMGroupInfo> pPtr(new CYouMeIMGroupInfo);
				if (request.has_groupinfo())
				{
					pPtr->m_strGroupID = UTF8TOXString(request.groupinfo().groupid());
					pPtr->m_groupType = (YouMeIMGroupType)request.groupinfo().grouptype();
					pPtr->m_strGroupName = UTF8TOXString(request.groupinfo().groupname());
				}
				m_pCallback->OnCreateGroup((YouMeIMErrorcode)request.msghead().code(), head.head().clientserialnum(), pPtr.get());
			}
		}
	}
	break;
	case YouMeIMProtocol::Command_LeaveGroup:
	case YouMeIMProtocol::Command_LeaveGroupResponse:
	{
		bCheckResopnse = true;
	}
	break;
	case YouMeIMProtocol::Command_JoinGroupResponse :
	{
		bCheckResopnse = true;
		YouMeIMProtocol::YouMeIM_Command_JoinGroupResponse request;
		if (!request.ParseFromArray(pBuffer, iBufferSize))
		{
			//记录一下
			YouMe_LOG_Error(__XT("解析 YouMeIM_Command_JoinGroupResponse 失败了,长度：%d"), iBufferSize);
		}
		else
		{
			if (NULL != m_pCallback)
			{
				std::shared_ptr<CYouMeIMGroupInfo> pPtr(new CYouMeIMGroupInfo);
				pPtr->m_strGroupID = UTF8TOXString(request.groupinfo().groupid());
				pPtr->m_groupType = (YouMeIMGroupType)request.groupinfo().grouptype();
				pPtr->m_strGroupName = UTF8TOXString(request.groupinfo().groupname());
				m_pCallback->OnJoinGroupResponse((YouMeIMErrorcode)request.msghead().code(), (YouMeIMGroupEvent)request.evttype(), pPtr.get());
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

YouMeIMErrorcode YouMeIMGroupManager::GetAllGroup()
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_GetAllGroup request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_GetAllGroup));
	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request,true);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_GetAllGroup));

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMGroupManager::CreateGroup(const XString&strGroupName,XINT64 *iRequestID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_CreateGroup request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_CreateGroup));
	request.set_groupname(XStringToUTF8(strGroupName));
	request.set_grouptype(YouMeIMProtocol::GroupType_Group);

	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request,true);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		if (NULL != iRequestID)
		{
			*iRequestID = request.msghead().msgserialnum();
		}
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_CreateGroup));
		pMessage->SetParam("GroupName", strGroupName);
		pMessage->SetParam("RequestID", (XINT64)request.msghead().msgserialnum());

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

void YouMeIMGroupManager::SetCallback(IYouMeIMGroupCallback* pCallback)
{
	m_pCallback = pCallback;
}

YouMeIMErrorcode YouMeIMGroupManager::DestroyGroup(const XString& strGroupID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_DestroyGroup request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_DestroyGroup));
	request.set_groupid(XStringToUTF8(strGroupID));

	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_DestroyGroup));
		pMessage->SetParam("GroupID", strGroupID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMGroupManager::GetGroupMembers(const XString& strGroupID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_DestroyGroup request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_GetGroupMembers));
	request.set_groupid(XStringToUTF8(strGroupID));

	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_GetGroupMembers));
		pMessage->SetParam("GroupID", strGroupID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMGroupManager::LeaveGroup(const XString&strGroupID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	YouMeIMProtocol::YouMeIM_Command_LeaveGroup request;
	request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_LeaveGroup));
	request.set_groupid(XStringToUTF8(strGroupID));
	
	YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
	if (errorcode == YouMeIMErrorcode_Success)
	{
		std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_LeaveGroup));

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
	}
	return errorcode;
}

YouMeIMErrorcode YouMeIMGroupManager::JoinGroup(const XString& strGroupID,const XString&strReason)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

    return InterJoimGroupOrChatroom(strGroupID, strReason, YouMeIMProtocol::GroupType_Group);
}

YouMeIMErrorcode YouMeIMGroupManager::KickoutMember(const XString& strGroupID, const XString&strUserID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	return YouMeIMErrorcode_Fail;
}

YouMeIMErrorcode YouMeIMGroupManager::AgreeGroupInvited(const XString& strGroupID, const XString&strUserID)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	return YouMeIMErrorcode_Fail;
}

YouMeIMErrorcode YouMeIMGroupManager::RefuseGroupInvited(const XString& strGroupID, const XString&strUserID, const XString& strReason)
{
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}

	return YouMeIMErrorcode_Fail;
}

YouMeIMGroupManager::YouMeIMGroupManager(YouMeIMManager* pIManager):
IManagerBase(pIManager)
{
	m_bInit = false;
}

bool YouMeIMGroupManager::OnRecvPacketTimeOut(XINT64 iUniqueID)
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

	if (NULL == m_pCallback)
	{
		return true;
	}
	switch (messageInfo.command)
	{
	case YouMeIMProtocol::Command_JoinGroup:
	{		
		std::shared_ptr<CYouMeIMGroupInfo> pPtr(new CYouMeIMGroupInfo);
		pPtr->m_strGroupID = messageInfo.GetParam("GroupID", __XT(""));
		m_pCallback->OnJoinGroupResponse(YouMeIMErrorcode_TimeOut, YouMeIMGroupEvent_Refuse, pPtr.get());
	}
	break;
	case YouMeIMProtocol::Command_GetGroupMembers:
	{
		m_pCallback->OnGetGroupMembers(YouMeIMErrorcode_TimeOut, messageInfo.GetParam("GroupID", __XT("")), NULL);
	}
	break;
	case YouMeIMProtocol::Command_CreateGroup:
	{
		m_pCallback->OnCreateGroup(YouMeIMErrorcode_TimeOut, messageInfo.GetParam("RequestID", (XINT64)0), NULL);
	}
	break;
	case YouMeIMProtocol::Command_GetAllGroup:
	{
		m_pCallback->OnGetAllGroup(YouMeIMErrorcode_TimeOut, NULL);
	}
	break;
	case YouMeIMProtocol::Command_DestroyGroup:
	{
		m_pCallback->OnDestroyGroup(YouMeIMErrorcode_TimeOut, messageInfo.GetParam("GroupID", __XT("")));
	}
	break;
	default:
		YouMe_LOG_Warning(__XT("没有处理的需要服务器回复的超时包:%d"), messageInfo.command);
		break;
	}
	return true;
}

YouMeIMErrorcode YouMeIMGroupManager::JoinChatRoom(const XString& strChatRoomID)
{
	YouMe_LOG_Info(__XT("Enter :%s"),strChatRoomID.c_str());
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}
	m_chatRoomSet.insert(strChatRoomID);
	return InterJoimGroupOrChatroom(strChatRoomID, __XT(""), YouMeIMProtocol::GroupType_ChatRoom);
}

YouMeIMErrorcode YouMeIMGroupManager::LeaveChatRoom(const XString& strChatRoomID)
{
	YouMe_LOG_Info(__XT("Enter :%s"), strChatRoomID.c_str());
	if (m_pIManager == NULL || m_pIManager->GetLoginStatus() != IMManangerLoginStatus_Success)
	{
		return YouMeIMErrorcode_NotLogin;
	}
	std::set<XString>::iterator it = m_chatRoomSet.find(strChatRoomID);
	if (it != m_chatRoomSet.end())
	{
		m_chatRoomSet.erase(it);
	}
	return LeaveGroup(strChatRoomID);
}



void YouMeIMGroupManager::ReJoinChatRoom()
{
	std::set<XString>::iterator begin = m_chatRoomSet.begin();
	std::set<XString>::iterator end = m_chatRoomSet.end();
	for (; begin != end;begin++)
	{
		YouMe_LOG_Info(__XT("重新进入聊天室:%s"), begin->c_str());
		InterJoimGroupOrChatroom(*begin, __XT(""), YouMeIMProtocol::GroupType_ChatRoom);
	}
}

XString CYouMeIMGroupInfo::GetGroupID()
{
	return m_strGroupID;
}


YouMeIMGroupType CYouMeIMGroupInfo::GetGroupType()
{
	return m_groupType;
}

XString CYouMeIMGroupInfo::GetGroupName()
{
	return m_strGroupName;
}

XString CYouMeIMGroupMemberInfo::GetYouMeID()
{
	return m_strYouMeID;
}

ContactStatus CYouMeIMGroupMemberInfo::GetStatus()
{
	return m_status;
}
YouMeIMGroupPermission CYouMeIMGroupMemberInfo::GetMemberPermission()
{
	return m_permission;
}

YouMeIMErrorcode YouMeIMGroupManager::InterJoimGroupOrChatroom(const XString& strID,const XString&strReason, YouMeIMProtocol::YouMeIM_GroupType groupType)
{
    YouMeIMProtocol::YouMeIM_Command_JoinGroup request;
    request.set_allocated_msghead(m_pIManager->CreatePacketHead(YouMeIMProtocol::Command_JoinGroup));
    request.set_groupid(XStringToUTF8(strID));
    request.set_reason(XStringToUTF8(strReason));
    request.set_grouptype(groupType);
    YouMeIMErrorcode errorcode = m_pIManager->SendProtoPacket(request);
    if (errorcode == YouMeIMErrorcode_Success)
    {
        std::shared_ptr<MessageInfo> pMessage(new MessageInfo(YouMeIMProtocol::Command_JoinGroup));
		pMessage->SetParam("GroupID", strID);

		{
			std::lock_guard<std::mutex> lock(m_messageCacheMutex);
			m_messageMap[request.msghead().msgserialnum()] = pMessage;
		}
    }
    return errorcode;
}
