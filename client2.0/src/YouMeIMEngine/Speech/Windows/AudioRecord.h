#pragma once
#include <windows.h>
#include <Mmsystem.h>
#include <string>


class CWinSpeechManager;


struct WAVHeadInfo
{
	// RIFF chunk
	char riffID[4];					//"RIFF"
	unsigned int fileSize;
	char riffFormat[4];				//"WAVE"
	// format chunk
	char fmtID[4];					//"fmt "
	unsigned int fmtSize;
	unsigned short formatTag;		//���뷽ʽ
	unsigned short channels;		//������
	unsigned int sampleFrequency;	//����Ƶ��
	unsigned int byteRate;			//������
	unsigned short blockAlign;		//������С
	unsigned short sampleBitSize;	//�������
	// data chunk
	char dataID[4];					//"data"
	unsigned int dataChunkSize;
};

class AudioRecordThread
{
public:
	~AudioRecordThread();

	static AudioRecordThread* Instance();
	int Init(CWinSpeechManager* speechManager);
	int StartRecord(std::wstring& path);
	void StopRecord(bool cancel = false);

	void OnMM_WIM_OPEN(WPARAM wParam, LPARAM lParam);
	void OnMM_WIM_DATA(WPARAM wParam, LPARAM lParam);
	void OnMM_WIM_CLOSE(WPARAM wParam, LPARAM lParam);

private:
	AudioRecordThread();
	AudioRecordThread(const AudioRecordThread&);
	AudioRecordThread& operator= (const AudioRecordThread&);

	HWAVEIN m_hWaveInDev;
	WAVEFORMATEX m_waveFormat;
	WAVEHDR m_wavehdr1;
	WAVEHDR m_wavehdr2;
	BYTE* m_pBuffer1;
	BYTE* m_pBuffer2;
	FILE* m_pFile;
	bool m_bRecording;
	bool m_bCancelReord;
	HWND m_hWnd;
	std::wstring m_wstrRecordPath;
	CWinSpeechManager* m_pSpeechManager;
};