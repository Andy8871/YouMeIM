//
//  IOSSpeechManager.hpp
//  YouMeIM
//
//  Created by 智璋 谢 on 16/6/11.
//
//

#ifndef IOSSpeechManager_hpp
#define IOSSpeechManager_hpp

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/Speech/SpeechListenManager.h>

class CIOSSpeechManager :public IYoueSpeechManager
{
public:
    virtual void SetSpeechListen(ISpeechListen* pListen) override;
    
    virtual bool Init() override;
    virtual int StartSpeech(XINT64 ulSerial) override;
    virtual int StartOnlySpeech(XINT64 ulSerial) override;
    virtual void StopSpeech() override;
    virtual void CancleSpeech() override;
    virtual void UnInit() override;
    virtual void SetCacheDir(const XString& path) override;
private:
    
    
    
};



#endif /* IOSSpeechManager_hpp */
