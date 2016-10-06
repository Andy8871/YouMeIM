//
//  Application.h
//  YouMeVoiceEngine
//
//  Created by wanglei on 15/9/17.
//  Copyright (c) 2015年 tencent. All rights reserved.
//
#include <map>
#include "../IYouMePacketInfo.h"

class YouMeApplication_OSX : public IYouMeSystemProvider
{
public:
    virtual XString getBrand ();
    virtual XString getSystemVersion ();
    virtual XString getCpuArchive ();
    virtual XString getPackageName ();
    virtual XString getUUID ();
    virtual XString getModel ();
    virtual XString getCpuShip ();
    virtual XString getDocumentPath ();
    
private:
    XString getBundleValueByName (const XString &name);

private:
    std::map<XString,XString> m_deviceNameMap;
};
