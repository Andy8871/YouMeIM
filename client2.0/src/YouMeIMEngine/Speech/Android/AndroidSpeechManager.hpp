//
//  IOSSpeechManager.hpp
//  YouMeIM
//
//  Created by 智璋 谢 on 16/6/11.
//
//

#ifndef AndroidSpeechManager_hpp
#define AndroidSpeechManager_hpp

#include <stdio.h>
#include <YouMeIMEngine/Speech/SpeechListenManager.h>
class CAndroidSpeechManager :public IYoueSpeechManager
{
public:
    CAndroidSpeechManager();
    virtual void SetSpeechListen(ISpeechListen* pListen) override;
    
    virtual bool Init() override;
    virtual int StartSpeech(XINT64 ulSerial) override;
	virtual int StartOnlySpeech(XINT64 ulSerial) override;
    virtual void StopSpeech() override;
    virtual void CancleSpeech() override;
    virtual void UnInit() override;
	virtual void SetCacheDir(const XString& path) override;
};



#endif /* AndroidSpeechManager_hpp */
