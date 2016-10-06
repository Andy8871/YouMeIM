//
//  NgnNetworkService.cpp
//  cocos2d-x sdk
//
//  Created by wanglei on 15/8/7.
//  Copyright (c) 2015 tencent. All rights reserved.
//

#include "NetworkService.h"
#ifdef WIN32

#elif (OS_IOS || OS_IOSSIMULATOR)
#include "CrossPlatformDefine/IOS/ReachabilityServiceBridge.h"
#elif OS_OSX
#include "CrossPlatformDefine/OSX/ReachabilityServiceBridge.h"
#endif
namespace youmecommon
{
CNetworkService* g_Instance = NULL;
CNetworkService::CNetworkService ()
{
}

CNetworkService::~CNetworkService ()
{

}

bool CNetworkService::start ()
{
	if (m_bIsStart)
	{
		return true;
	}
#ifdef WIN32
    
#elif (OS_IOS || OS_IOSSIMULATOR)
    YouMeCommonReachabilityServiceBridge::getInstance ()->init (this);
#elif OS_OSX
    YouMeReachabilityServiceBridge::getInstance ()->init (this);
#endif
	m_bIsStart = true;
    return true;
}

bool CNetworkService::stop ()
{
	if (!m_bIsStart)
	{
		return true;
	}
#ifdef WIN32
    
#elif (OS_IOS || OS_IOSSIMULATOR)
    YouMeCommonReachabilityServiceBridge::getInstance()->uninit();
    YouMeCommonReachabilityServiceBridge::destroy ();
#elif OS_OSX
    YouMeReachabilityServiceBridge::getInstance()->uninit();
    YouMeReachabilityServiceBridge::destroy ();
#endif
	m_bIsStart = false;
    return true;
}

NetworkType CNetworkService::getNetworkType ()
{
    return mNetWorkType;
}

void CNetworkService::onNetWorkChanged (NetworkType type)
{
	std::lock_guard<std::mutex> lock(m_lock);
    mNetWorkType = type;
	for (size_t i = 0; i < mPCallback.size();i++)
	{
		mPCallback[i]->onNetWorkChanged(type);
	}
}

void CNetworkService::registerCallback (INgnNetworkChangCallback *pCallback)
{
	std::lock_guard<std::mutex> lock(m_lock);
   if (pCallback != NULL)
   {
	   mPCallback.push_back(pCallback);
   }
}

CNetworkService* CNetworkService::Instance()
{
	if (NULL == g_Instance)
	{
		g_Instance = new CNetworkService;
	}
	return g_Instance;
}

void CNetworkService::DestroyInstance()
{
	delete g_Instance;
	g_Instance = NULL;
}

void CNetworkService::cancleCallback(INgnNetworkChangCallback *pCallbac)
{
	std::lock_guard<std::mutex> lock(m_lock);
	std::vector<INgnNetworkChangCallback *>::iterator begin = mPCallback .begin();
	std::vector<INgnNetworkChangCallback *>::iterator end = mPCallback.end();
	for (; begin != end;begin++)
	{
		if (pCallbac == *begin)
		{
			mPCallback.erase(begin);
			break;
		}
	}
}
}
