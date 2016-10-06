//
//  IOSSpeechManager.cpp
//  YouMeIM
//
//  Created by 智璋 谢 on 16/6/11.
//
//

#include <YouMeIMEngine/Speech/IOS/IATSpeech.h>
#include "IOSSpeechManager.hpp"
void CIOSSpeechManager::SetSpeechListen(ISpeechListen* pListen)
{
    [[IATSpeech getInstance] SetListen:pListen];
}
int CIOSSpeechManager::StartSpeech(XINT64 ulSerial)
{
    return [[IATSpeech getInstance] StartSpeech:ulSerial];
}
int CIOSSpeechManager::StartOnlySpeech(XINT64 ulSerial)
{
    return [[IATSpeech getInstance] StartOnlySpeech:ulSerial];
}


void CIOSSpeechManager::StopSpeech()
{
    [[IATSpeech getInstance]StopSpeech];
}
void CIOSSpeechManager::CancleSpeech()
{
    [[IATSpeech getInstance]CancleSpeech];
}

bool CIOSSpeechManager::Init()
{
    [ [IATSpeech getInstance] Init];
    return  true;
}

void CIOSSpeechManager::UnInit()
{
    
}

void CIOSSpeechManager::SetCacheDir(const XString& path)
{
    NSString *strDir = [NSString stringWithUTF8String:path.c_str()];
    [[IATSpeech getInstance] SetCacheDir:strDir];
}