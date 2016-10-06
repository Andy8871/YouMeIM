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
class YouMeReachabilityServiceBridge
{
    public:
    static YouMeReachabilityServiceBridge *getInstance ();
    static void destroy ();

    public:
    void init (youmecommon::INgnNetworkChangCallback *networkChangeCallback);
    void uninit();
    private:
    YouMeReachabilityServiceBridge ();
    ~YouMeReachabilityServiceBridge ();

    private:
    static YouMeReachabilityServiceBridge *mPInstance;
};

#endif /* defined(__YouMeVoiceEngine__ReachabilityServiceBridge__) */
