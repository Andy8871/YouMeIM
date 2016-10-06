// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "pb/im_group.pb.h"
#include "YouMeIMCommonDef.h"
#include <memory>
#include <map>

class CYouMeIMGroupMemberInfo :public IYouMeIMGroupMemberInfo
{
	friend class  YouMeIMGroupManager;
public:
	virtual XString GetYouMeID() override;
	virtual ContactStatus GetStatus() override;
	virtual YouMeIMGroupPermission GetMemberPermission() override;

private:
	XString m_strYouMeID;	
	ContactStatus m_status;
	YouMeIMGroupPermission m_permission;
};



class CYouMeIMGroupInfo :public IYouMeIMGroupInfo
{
	friend class YouMeIMGroupManager;
public:
	virtual XString GetGroupID() override;
	virtual XString GetGroupName() override;
	virtual YouMeIMGroupType GetGroupType() override;
private:
	YouMeIMGroupType m_groupType;
	//数字的需要转换成字符串
	XString m_strGroupID;
	XString m_strGroupName;
};





class YouMeIMGroupManager :public IYouMeIMGroupManager,
	public IManagerBase
{
	friend class YouMeIMManager;

public:
	YouMeIMGroupManager(YouMeIMManager* pIManager);
	YouMeIMErrorcode Init();
	void UnInit();
	void SetCallback(IYouMeIMGroupCallback* pCallback);
	//群接口
	YouMeIMErrorcode GetAllGroup()override;
	YouMeIMErrorcode CreateGroup(const XString&strGroupName, XINT64 *iRequestID)override;
	YouMeIMErrorcode DestroyGroup(const XString& strGroupID)override;
	YouMeIMErrorcode GetGroupMembers(const XString& strGroupID)override;
	YouMeIMErrorcode JoinGroup(const XString& strGroupID, const XString&strReason)override;
	YouMeIMErrorcode AgreeGroupInvited(const XString& strGroupID, const XString&strUserID)override;
	YouMeIMErrorcode RefuseGroupInvited(const XString& strGroupID, const XString&strUserID, const XString& strReason)override;
	YouMeIMErrorcode KickoutMember(const XString& strGroupID, const XString&strUserID)override;
	YouMeIMErrorcode LeaveGroup(const XString&strGroupID)override;
	//聊天室接口
	YouMeIMErrorcode JoinChatRoom(const XString& strChatRoomID) override;
	YouMeIMErrorcode LeaveChatRoom(const XString& strChatRoomID) override;

	//重新加入聊天室
	void ReJoinChatRoom();
private:
	//加入群或者聊天室的封装
    YouMeIMErrorcode InterJoimGroupOrChatroom(const XString& strID,const XString&strReason, YouMeIMProtocol::YouMeIM_GroupType);
    
	//下面两个函数用来处理上一个管理器没有处理的包
	virtual bool OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize) override;
	virtual bool OnRecvPacketTimeOut(XINT64 iUniqueID) override;

	IYouMeIMGroupCallback* m_pCallback;
	bool m_bInit;

	//保存一个 聊天室ID 集合，自动重连的时候需要再次进入
	std::set<XString> m_chatRoomSet;
};
