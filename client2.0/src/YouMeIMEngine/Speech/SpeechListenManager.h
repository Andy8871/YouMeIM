#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
class ISpeechListen
{
public:
	virtual void OnSpeechInit(int iErrorcode) = 0;
	virtual void OnSpeechResult(XINT64 ulSerial,int iErrorcode, const XString& strResult, const XString& strWavPath) = 0;
};

class IYoueSpeechManager
{
public:
	virtual ~IYoueSpeechManager(){}
	//设置回调
	virtual void SetSpeechListen(ISpeechListen* pListen) = 0;
    virtual bool Init() = 0;
    virtual int StartSpeech(XINT64 ulSerial) = 0;
	virtual int StartOnlySpeech(XINT64 ulSerial) = 0;
    virtual void StopSpeech() = 0;
    virtual void CancleSpeech() = 0;
    virtual void UnInit() = 0;
	virtual void SetCacheDir(const XString& path) = 0;
};
