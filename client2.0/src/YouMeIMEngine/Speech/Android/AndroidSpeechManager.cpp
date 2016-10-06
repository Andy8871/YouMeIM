//
//  IOSSpeechManager.cpp
//  YouMeIM
//
//  Created by 智璋 谢 on 16/6/11.
//
//

#include "AndroidSpeechManager.hpp"
extern int YouMe_StartListening(XINT64 );
extern int YouMe_StartListeningOnlyAudio(XINT64 );
extern void YouMe_StopListening();
extern void YouMe_CancleListening();
extern void YouMe_SetAndroidSpeechListen(ISpeechListen* pListen);
extern void YouMe_PlayAudio(const char* pszPath);
extern void YouMe_SetCacheDir(const char* pszPath);
CAndroidSpeechManager::CAndroidSpeechManager()
{
    
}
void CAndroidSpeechManager::SetSpeechListen(ISpeechListen* pListen)
{
    YouMe_SetAndroidSpeechListen(pListen);
}
int CAndroidSpeechManager::StartSpeech(XINT64 ulSerial)
{
    return YouMe_StartListening(ulSerial);
}

int CAndroidSpeechManager::StartOnlySpeech(XINT64 ulSerial)
{
	return YouMe_StartListeningOnlyAudio(ulSerial);
}
void CAndroidSpeechManager::StopSpeech()
{
    YouMe_StopListening();
}
void CAndroidSpeechManager::CancleSpeech()
{
    YouMe_CancleListening();
}

bool CAndroidSpeechManager::Init()
{
    return  true;
}
void CAndroidSpeechManager::UnInit()
{
    
}

void CAndroidSpeechManager::SetCacheDir(const XString& path)
{
	YouMe_SetCacheDir(path.c_str());
}