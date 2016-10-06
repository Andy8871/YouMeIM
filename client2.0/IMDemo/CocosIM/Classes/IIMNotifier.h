#pragma once
#include <string>

class IIMAddContactNotifyCallback
{
public:
	virtual void OnAddReturnNotify(const std::string& strID, const std::string& strReason) = 0;
};

class IIMReceiveInviteNotifyCallback
{
public:
	virtual void OnInviteReturnNotify(const std::string& strID, bool bAgree) = 0;
};