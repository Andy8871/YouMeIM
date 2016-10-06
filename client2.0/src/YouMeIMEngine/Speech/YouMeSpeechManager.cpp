#include "YouMeSpeechManager.h"
#include <YouMeCommon/Log.h>
#ifdef WIN32
#include "Windows/WinSpeechManager.h"
#elif (OS_IOS || OS_IOSSIMULATOR)
#include "IOS/IOSSpeechManager.hpp"
#elif OS_ANDROID
#include "Android/AndroidSpeechManager.hpp"
#endif

CYouMeSpeechManager::CYouMeSpeechManager()
{
    m_pSpeechManager = NULL;
#ifdef WIN32
	m_pSpeechManager = new CWinSpeechManager;
#elif (OS_IOS || OS_IOSSIMULATOR)
    m_pSpeechManager = new CIOSSpeechManager;
#elif OS_ANDROID
    m_pSpeechManager = new CAndroidSpeechManager;
#else
    
#endif
}


CYouMeSpeechManager::~CYouMeSpeechManager()
{
    delete m_pSpeechManager;
}

void CYouMeSpeechManager::SetSpeechListen(ISpeechListen* pListen)
{
    if (NULL != m_pSpeechManager) {
        m_pSpeechManager->SetSpeechListen(pListen);
    }
}


int CYouMeSpeechManager::StartSpeech(XINT64 ulSerial)
{
    if (NULL != m_pSpeechManager) {
        return m_pSpeechManager->StartSpeech(ulSerial);
    }
    return  -1;
}
int CYouMeSpeechManager::StartOnlySpeech(XINT64 ulSerial)
{
	if (NULL != m_pSpeechManager) {
		return m_pSpeechManager->StartOnlySpeech(ulSerial);
	}
	return  -1;
}
void CYouMeSpeechManager::StopSpeech()
{
    if (NULL != m_pSpeechManager) {
        m_pSpeechManager->StopSpeech();
    }
}
void CYouMeSpeechManager::CancleSpeech()
{
    if (NULL != m_pSpeechManager) {
        m_pSpeechManager->CancleSpeech();
    }
}

bool CYouMeSpeechManager::Init()
{
    if (NULL != m_pSpeechManager) {
       return m_pSpeechManager->Init();
    }
    return  false;
}
void CYouMeSpeechManager::UnInit()
{
    if (NULL != m_pSpeechManager) {
        m_pSpeechManager->UnInit();
    }
}

void CYouMeSpeechManager::SetAudioCacheDir(const XString& audioCacheDir)
{
	if (m_pSpeechManager != NULL)
	{
		XString strDir = audioCacheDir;
		if (!strDir.empty() && strDir[strDir.length() - 1] != '/' && strDir[strDir.length() - 1] != '\\')
		{
			strDir.append(XPreferredSeparator);
		}
		m_pSpeechManager->SetCacheDir(strDir);
	}
}