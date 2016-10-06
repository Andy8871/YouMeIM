//
//  ReachabilityServiceBridge.h
//  YouMeVoiceEngine
//
//  Created by wanglei on 15/9/17.
//  Copyright (c) 2015年 tencent. All rights reserved.
//

#ifndef __YouMeVoiceEngine__ReachabilityServiceBridge__
#define __YouMeVoiceEngine__ReachabilityServiceBridge__

#include "NetworkService.h"
class YouMeCommonReachabilityServiceBridge
{
    public:
    static YouMeCommonReachabilityServiceBridge *getInstance ();
    static void destroy ();

    public:
    void init (youmecommon::INgnNetworkChangCallback *networkChangeCallback);
    void uninit();
    private:
    YouMeCommonReachabilityServiceBridge ();
    ~YouMeCommonReachabilityServiceBridge ();

    private:
    static YouMeCommonReachabilityServiceBridge *mPInstance;
};

#endif /* defined(__YouMeVoiceEngine__ReachabilityServiceBridge__) */
