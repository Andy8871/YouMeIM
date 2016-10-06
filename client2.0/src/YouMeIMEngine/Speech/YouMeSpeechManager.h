#pragma once
#include "SpeechListenManager.h"

//内部使用的一个语音类，区分不同平台下的操作
class CYouMeSpeechManager
{
public:
	CYouMeSpeechManager();
	~CYouMeSpeechManager();
    
    void SetSpeechListen(ISpeechListen* pListen);
    //初始化和反初始化
    bool Init();
    void UnInit();
    
    //操作语音的接口，语音的特殊性，同一时刻只能开启一下，不能同时开多个。内部会有判断
    int StartSpeech(XINT64 ulSerial);
    void StopSpeech();
    void CancleSpeech();
    //发送纯语音消息，没有转文本
	int StartOnlySpeech(XINT64 ulSerial);
	void SetAudioCacheDir(const XString& audioCacheDir);
	
private:
    IYoueSpeechManager* m_pSpeechManager;
};

