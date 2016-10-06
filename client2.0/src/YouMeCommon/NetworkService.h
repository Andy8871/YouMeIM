//
//  NgnNetworkService.h
//  cocos2d-x sdk
//
//  Created by wanglei on 15/8/7.
//  Copyright (c) 2015年 tencent. All rights reserved.
//

#ifndef __cocos2d_x_sdk__NgnNetworkService__
#define __cocos2d_x_sdk__NgnNetworkService__

#include <string>
#include <vector>
#include <mutex>
namespace youmecommon
{
//网络类型
enum NetworkType
{
	NetworkType_Unknow = 0,
	NetworkType_3G = 1, //移动网络
	NetworkType_Wifi = 2
};

class INgnNetworkChangCallback
{
public:
	virtual void onNetWorkChanged(NetworkType type) = 0;
};

/**
 * Network service.
 */
class CNetworkService :public INgnNetworkChangCallback
{
public:
	static CNetworkService* Instance();
	static void DestroyInstance();
	virtual ~CNetworkService();

private:
	CNetworkService();
    NetworkType mNetWorkType = NetworkType_Wifi;
    std::vector<INgnNetworkChangCallback *> mPCallback;

public:
    virtual bool start ();
    virtual bool stop ();

    virtual NetworkType getNetworkType ();
	void onNetWorkChanged(NetworkType type);
    void registerCallback (INgnNetworkChangCallback *pCallback);
	void cancleCallback(INgnNetworkChangCallback *pCallbac);
	std::mutex m_lock;
	bool m_bIsStart=false;
};

}
#endif /* defined(__cocos2d_x_sdk__NgnNetworkService__) */
