//
//  ReachabilityServiceBridge.cpp
//  YouMeVoiceEngine
//
//  Created by wanglei on 15/9/17.
//  Copyright (c) 2015年 tencent. All rights reserved.
//

#include "ReachabilityServiceBridge.h"
#import "ReachabilityService.h"

YouMeCommonReachabilityServiceBridge *YouMeCommonReachabilityServiceBridge::mPInstance = NULL;

YouMeCommonReachabilityServiceBridge *YouMeCommonReachabilityServiceBridge::getInstance ()
{
    if (mPInstance == NULL)
    {
        mPInstance = new YouMeCommonReachabilityServiceBridge ();
    }
    return mPInstance;
}
void YouMeCommonReachabilityServiceBridge::destroy ()
{
    delete mPInstance;
    mPInstance = NULL;
}


void YouMeCommonReachabilityServiceBridge::init (youmecommon::INgnNetworkChangCallback *networkChangeCallbakc)
{
    YouMeCommonReachabilityService* pService = [[YouMeCommonReachabilityService getInstance] init];
    [pService start:networkChangeCallbakc];
}
void YouMeCommonReachabilityServiceBridge::uninit()
{
    YouMeCommonReachabilityService* pService = [[YouMeCommonReachabilityService getInstance] init];
    [pService uninit];
}

YouMeCommonReachabilityServiceBridge::YouMeCommonReachabilityServiceBridge ()
{
}
YouMeCommonReachabilityServiceBridge::~YouMeCommonReachabilityServiceBridge ()
{
    [YouMeCommonReachabilityService destroy];
}
