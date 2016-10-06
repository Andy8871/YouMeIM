//
//  ReachabilityServiceBridge.cpp
//  YouMeVoiceEngine
//
//  Created by wanglei on 15/9/17.
//  Copyright (c) 2015年 tencent. All rights reserved.
//

#include "ReachabilityServiceBridge.h"
#import "ReachabilityService.h"

YouMeReachabilityServiceBridge *YouMeReachabilityServiceBridge::mPInstance = NULL;

YouMeReachabilityServiceBridge *YouMeReachabilityServiceBridge::getInstance ()
{
    if (mPInstance == NULL)
    {
        mPInstance = new YouMeReachabilityServiceBridge ();
    }
    return mPInstance;
}
void YouMeReachabilityServiceBridge::destroy ()
{
    delete mPInstance;
    mPInstance = NULL;
}


void YouMeReachabilityServiceBridge::init (youmecommon::INgnNetworkChangCallback *networkChangeCallbakc)
{
    YouMeReachabilityService* pService = [[YouMeReachabilityService getInstance] init];
    [pService start:networkChangeCallbakc];
}
void YouMeReachabilityServiceBridge::uninit()
{
    YouMeReachabilityService* pService = [[YouMeReachabilityService getInstance] init];
    [pService uninit];
}

YouMeReachabilityServiceBridge::YouMeReachabilityServiceBridge ()
{
}
YouMeReachabilityServiceBridge::~YouMeReachabilityServiceBridge ()
{
    [YouMeReachabilityService destroy];
}
