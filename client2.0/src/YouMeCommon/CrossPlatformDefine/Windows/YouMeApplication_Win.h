//
//  Application.h
//  YouMeVoiceEngine
//
//  Created by wanglei on 15/9/17.
//  Copyright (c) 2015年 tencent. All rights reserved.
//
#include <string>
#include "../IYouMePacketInfo.h"

class YouMeApplication_Win : public IYouMeSystemProvider
{
public:
	virtual XString getBrand();
	virtual XString getSystemVersion();
	virtual XString getCpuArchive();
	virtual XString getPackageName();
	virtual XString getUUID();
	virtual XString getModel();
	virtual XString getCpuShip();
	virtual XString getDocumentPath();

public:
    YouMeApplication_Win ();
    ~YouMeApplication_Win ();
};
