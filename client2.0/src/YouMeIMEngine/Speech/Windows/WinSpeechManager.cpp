#include "WinSpeechManager.h"
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "AudioRecord.h"
#include "kdxf/qisr.h"
#include "kdxf/msp_cmn.h"
#include "kdxf/msp_errors.h"
#include <YouMeCommon/XFile.h>


#ifdef _WIN64
#pragma comment(lib,"../../lib/msc_x64.lib") //x64
#else
#pragma comment(lib,"../../lib/msc.lib") //x86
#endif

#define FRAME_LEN	640 
#define SESSON_PARAM "sub = iat, domain = iat, language = zh_ch, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8"

CWinSpeechManager::CWinSpeechManager() : m_pSpeechListen(NULL), m_bAudioConvertInit(false), m_ulSerial(0), m_bSpeechOnly(false)
{

}

CWinSpeechManager::~CWinSpeechManager()
{
	
}
void CWinSpeechManager::SetSpeechListen(ISpeechListen* pListen)
{
	m_pSpeechListen = pListen;
}

bool CWinSpeechManager::Init()
{
	if (m_bAudioConvertInit)
	{
		return true;
	}
	AudioRecordThread::Instance()->Init(this);

	int ret = MSPLogin(NULL, NULL, "appid = 577f51fe, work_dir = .");
	if (MSP_SUCCESS != ret)
	{
		MSPLogout();
		return false;
	}
	m_bAudioConvertInit = true;
	SetCacheDir(__XT(""));

	return true;
}

int CWinSpeechManager::StartSpeech(XINT64 ulSerial)
{
	if (m_wstrRecordDir.empty())
	{
		return -1;
	}
	m_ulSerial = ulSerial;
	std::wstring path = m_wstrRecordDir;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	wchar_t wszTemp[32] = { 0 };
	swprintf_s(wszTemp, L"%d%d%d%d%d%d.wav", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	path.append(wszTemp);

	return AudioRecordThread::Instance()->StartRecord(path);
}

int CWinSpeechManager::StartOnlySpeech(XINT64 ulSerial)
{
	m_ulSerial = ulSerial;
	m_bSpeechOnly = true;
	std::wstring path;
	AudioRecordThread::Instance()->StartRecord(path);
	return 0;
}

void CWinSpeechManager::StopSpeech()
{
	AudioRecordThread::Instance()->StopRecord();
}

void CWinSpeechManager::CancleSpeech()
{
	AudioRecordThread::Instance()->StopRecord(true);
}

void CWinSpeechManager::UnInit()
{
	if (m_bAudioConvertInit)
	{
		MSPLogout();
		m_bAudioConvertInit = false;
	}
}

bool CWinSpeechManager::AudioToText(const std::wstring& audioPath, std::string& text)
{
	if (!m_bAudioConvertInit || audioPath.empty())
	{
		return false;
	}

	FILE* pFile = NULL;
	errno_t errCode = _wfopen_s(&pFile, audioPath.c_str(), __XT("rb"));
	if (errCode != 0 || NULL == pFile)
	{
		return false;
	}

	bool bRet = true;

	fseek(pFile, 0, SEEK_END);
	long lAudioFileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char* szAudioData = new char[lAudioFileSize];
	if (NULL == szAudioData)
	{
		bRet = false;
		goto iat_exit;
	}
	if (fread((void *)szAudioData, 1, lAudioFileSize, pFile) != lAudioFileSize)
	{
		bRet = false;
		goto iat_exit;
	}

	int nErrorCode = MSP_SUCCESS;
	const char*	szSessionID = QISRSessionBegin(NULL, SESSON_PARAM, &nErrorCode); //听写不需要语法，第一个参数为NULL
	if (MSP_SUCCESS != nErrorCode)
	{
		bRet = false;
		goto iat_exit;
	}

	int nAudioState = MSP_AUDIO_SAMPLE_CONTINUE;	//音频状态
	int nEpState = MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int	nRecognizeState = MSP_REC_STATUS_SUCCESS;	//识别状态
	int nRet = 0;
	long lSendSize = 0;

	while (1)
	{
		unsigned int nSize = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
		if (lAudioFileSize < 2 * nSize)
		{
			nSize = lAudioFileSize;
		}
		if (nSize <= 0)
		{
			break;
		}
		nAudioState = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == lSendSize)
		{
			nAudioState = MSP_AUDIO_SAMPLE_FIRST;
		}

		nRet = QISRAudioWrite(szSessionID, (const void *)&szAudioData[lSendSize], nSize, nAudioState, &nEpState, &nRecognizeState);
		if (MSP_SUCCESS != nRet)
		{
			bRet = false;
			goto iat_exit;
		}

		lSendSize += (long)nSize;
		lAudioFileSize -= (long)nSize;

		if (MSP_REC_STATUS_SUCCESS == nRecognizeState) //已经有部分听写结果
		{
			const char* szText = QISRGetResult(szSessionID, &nRecognizeState, 0, &nErrorCode);
			if (MSP_SUCCESS != nErrorCode)
			{
				bRet = false;
				goto iat_exit;
			}
			if (NULL != szText && strlen(szText) > 0)
			{
				text.append(szText);
			}
		}

		if (MSP_EP_AFTER_SPEECH == nEpState)
		{
			break;
		}
	}
	nErrorCode = QISRAudioWrite(szSessionID, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &nEpState, &nRecognizeState);
	if (MSP_SUCCESS != nErrorCode)
	{
		bRet = false;
		goto iat_exit;
	}

	while (MSP_REC_STATUS_COMPLETE != nRecognizeState)
	{
		const char* szText = QISRGetResult(szSessionID, &nRecognizeState, 0, &nErrorCode);
		if (MSP_SUCCESS != nErrorCode)
		{
			bRet = false;
			goto iat_exit;
		}
		if (NULL != szText && strlen(szText) > 0)
		{
			text.append(szText);
		}
		Sleep(150); //防止频繁占用CPU
	}

iat_exit:
	if (NULL != pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	if (NULL != szAudioData)
	{
		delete[] szAudioData;
		szAudioData = NULL;
	}

	char hints[1] = {0}; // 结束本次会话的原因描述 用户自定义
	QISRSessionEnd(szSessionID, hints);

	return bRet;
}

void CWinSpeechManager::RecordDone(const std::wstring& path)
{
	if (NULL == m_pSpeechListen)
	{
		return;
	}

	XString text;
	bool bRet = true;
	if (!m_bSpeechOnly)
	{
		std::string strText;
		bRet = AudioToText(path, strText);
		if (bRet)
		{
			text = UTF8TOXString(strText);
		}
	}
	m_pSpeechListen->OnSpeechResult(m_ulSerial, bRet ? 0 : -1, text, path);

	m_bSpeechOnly = false;
}

void CWinSpeechManager::SetCacheDir(const XString& path)
{
	if (path.empty())
	{
		wchar_t wszPath[MAX_PATH] = { 0 };
		if (GetTempPathW(MAX_PATH, wszPath) > 0)
		{
			std::wstring wstrTemp(wszPath);
			m_wstrRecordDir = wstrTemp.substr(0, wstrTemp.find_last_of('\\') + 1);
			m_wstrRecordDir.append(L"record\\");
			if (GetFileAttributesW(m_wstrRecordDir.c_str()) != FILE_ATTRIBUTE_DIRECTORY)
			{
				CreateDirectoryW(m_wstrRecordDir.c_str(), NULL);
			}
		}
	}
	else
	{
		if (!youmecommon::CXFile::IsFileExist(path))
		{
			if (youmecommon::CXFile::make_dir_tree(path.c_str()))
			{
				m_wstrRecordDir = path;
			}
		}
		else
		{
			m_wstrRecordDir = path;
		}
	}
}