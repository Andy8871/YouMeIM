// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include <YouMeCommon/CrossPlatformDefine/IYouMePacketInfo.h>
#include "YouMeIMCommonDef.h"
#include <memory>
#include <map>


class CYouMeIMContactInfo :public IYouMeIMContactInfo
{
	friend class YouMeIMContactManager;
	friend class YouMeIMManager;
public:
	virtual XString GeYouMeID() override;
	//获取在线状态
	virtual int GetStatus() override;
private:
	XString m_strYouMeID;
	int m_iStatus;
};



class YouMeIMContactManager :public IYouMeIMContactManager,
	public IManagerBase
{
	friend class YouMeIMManager;
public:
	YouMeIMContactManager(YouMeIMManager* pIManager);
	YouMeIMErrorcode Init() ;
	void UnInit() ;
    //单实例
	//设置好友回掉
	void SetCallback(IYouMeIMContactCallback* pCallback);
	//获取好友列表
	YouMeIMErrorcode GetContactList()override;
	//获取黑名单列表
	YouMeIMErrorcode GetBlacklistList()override;
	//添加好友
	YouMeIMErrorcode AddContact(const XString& strContactID, const XString& strReason)override;
	//同意好友请求
	YouMeIMErrorcode AgreeContactInvited(const XString& strContactID)override;
	//拒绝好友请求
	YouMeIMErrorcode RefuseContactInvited(const XString& strContactID, const XString& strReason)override;
	//删除好友
	YouMeIMErrorcode DeleteContact(const XString&strContactID)override;
	//添加到黑名单
	YouMeIMErrorcode AddToBlacklist(const XString& strContactID)override;
	//踢出黑名单
	YouMeIMErrorcode KickoutBlacklist(const XString&strContactID)override;
	//获取联系人信息
	YouMeIMErrorcode GetContactInfo(const std::list<XString>& contactIDList)override;
private:
	//下面两个函数用来处理上一个管理器没有处理的包
	virtual bool OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize) override;
	virtual bool OnRecvPacketTimeOut(XINT64 iUniqueID) override;
	
	IYouMeIMContactCallback* m_pCallback;
	bool m_bInit;
};
