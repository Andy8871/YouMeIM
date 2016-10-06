#pragma once

#include <YouMeIMEngine/Speech/SpeechListenManager.h>

class CWinSpeechManager :public IYoueSpeechManager
{
public:
	CWinSpeechManager();
	~CWinSpeechManager();

	virtual void SetSpeechListen(ISpeechListen* pListen) override;

	virtual bool Init() override;
	virtual int StartSpeech(XINT64 ulSerial) override;
	virtual int StartOnlySpeech(XINT64 ulSerial) override;
	virtual void StopSpeech() override;
	virtual void CancleSpeech() override;
	virtual void UnInit() override;
	virtual void SetCacheDir(const XString& path) override;

	void RecordDone(const std::wstring& path);

private:
	bool AudioToText(const std::wstring& audioPath, std::string& text);

	ISpeechListen* m_pSpeechListen;
	XINT64 m_ulSerial;
	std::wstring m_wstrRecordDir;
	bool m_bAudioConvertInit;
	bool m_bSpeechOnly;
};