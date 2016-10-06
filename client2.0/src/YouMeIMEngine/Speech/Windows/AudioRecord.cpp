#include "AudioRecord.h"
#include "WinSpeechManager.h"

#define INP_BUFFER_SIZE 8192

LRESULT CALLBACK MessageOnlyWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case MM_WIM_OPEN:
		AudioRecordThread::Instance()->OnMM_WIM_OPEN(wParam, lParam);
		break;
	case MM_WIM_DATA:
		AudioRecordThread::Instance()->OnMM_WIM_DATA(wParam, lParam);
		break;
	case MM_WIM_CLOSE:
		AudioRecordThread::Instance()->OnMM_WIM_CLOSE(wParam, lParam);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

AudioRecordThread::AudioRecordThread() : m_pSpeechManager(NULL)
,m_pBuffer1(NULL)
,m_pBuffer2(NULL)
,m_pFile(NULL)
,m_bRecording(false)
,m_bCancelReord(false)
{

}

AudioRecordThread::~AudioRecordThread()
{

}

AudioRecordThread* AudioRecordThread::Instance()
{
	static AudioRecordThread instance;
	return &instance;
}

int AudioRecordThread::Init(CWinSpeechManager* speechManager)
{
	if (NULL == speechManager)
	{
		return -1;
	}

	m_pSpeechManager = speechManager;

	m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels = 1;
	m_waveFormat.nSamplesPerSec = 8000;
	m_waveFormat.wBitsPerSample = 16;
	m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nChannels * m_waveFormat.wBitsPerSample / 8;
	m_waveFormat.nBlockAlign = m_waveFormat.nChannels * m_waveFormat.wBitsPerSample / 8;
	m_waveFormat.cbSize = 0;

	HMODULE  hInstance = GetModuleHandle(NULL);
	if (hInstance == NULL)
	{
		return -1;
	}
	WNDCLASS  wndcls;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = 0;
	wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndcls.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndcls.hInstance = hInstance;
	wndcls.lpfnWndProc = MessageOnlyWinProc;
	wndcls.lpszClassName = "MessageWindow";
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndcls);
	m_hWnd = CreateWindowEx(0, "MessageWindow", "MessageWindow", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInstance, 0);
	if (m_hWnd == NULL)
	{
		return -1;
	}

	return 0;
}

int AudioRecordThread::StartRecord(std::wstring& path)
{
	if (m_bRecording || path.empty())
	{
		return -1;
	}

	m_pBuffer1 = new BYTE[INP_BUFFER_SIZE];
	m_pBuffer2 = new BYTE[INP_BUFFER_SIZE];
	if (m_pBuffer1 == NULL || m_pBuffer2 == NULL)
	{
		if (m_pBuffer1)
		{
			delete[] m_pBuffer1;
		}
		if (m_pBuffer2)
		{
			delete[] m_pBuffer2;
		}
		return -1;
	}

	if (waveInOpen(&m_hWaveInDev, WAVE_MAPPER, &m_waveFormat, (DWORD)m_hWnd, NULL, CALLBACK_WINDOW) != 0)
	{
		delete[] m_pBuffer1;
		delete[] m_pBuffer2;
		return -1;
	}
	m_bRecording = true;
	m_wstrRecordPath = path;

	m_wavehdr1.lpData = (LPSTR)m_pBuffer1;
	m_wavehdr1.dwBufferLength = INP_BUFFER_SIZE;
	m_wavehdr1.dwBytesRecorded = 0;
	m_wavehdr1.dwUser = 0;
	m_wavehdr1.dwFlags = 0;
	m_wavehdr1.dwLoops = 1;
	m_wavehdr1.lpNext = NULL;
	m_wavehdr1.reserved = 0;
	waveInPrepareHeader(m_hWaveInDev, &m_wavehdr1, sizeof(WAVEHDR));

	m_wavehdr2.lpData = (LPSTR)m_pBuffer2;
	m_wavehdr2.dwBufferLength = INP_BUFFER_SIZE;
	m_wavehdr2.dwBytesRecorded = 0;
	m_wavehdr2.dwUser = 0;
	m_wavehdr2.dwFlags = 0;
	m_wavehdr2.dwLoops = 1;
	m_wavehdr2.lpNext = NULL;
	m_wavehdr2.reserved = 0;
	waveInPrepareHeader(m_hWaveInDev, &m_wavehdr2, sizeof(WAVEHDR));

	waveInAddBuffer(m_hWaveInDev, &m_wavehdr1, sizeof(WAVEHDR));
	waveInAddBuffer(m_hWaveInDev, &m_wavehdr2, sizeof(WAVEHDR));

	waveInStart(m_hWaveInDev);

	return 0;
}

void AudioRecordThread::StopRecord(bool cancel)
{
	waveInReset(m_hWaveInDev);
	m_bRecording = false;
	m_bCancelReord = cancel;
}

void AudioRecordThread::OnMM_WIM_OPEN(WPARAM wParam, LPARAM lParam)
{
	if (m_wstrRecordPath.empty())
	{
		StopRecord();
		return;
	}
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	errno_t errCode = _wfopen_s(&m_pFile, m_wstrRecordPath.c_str(), L"wb+");
	if (errCode != 0 || NULL == m_pFile)
	{
		return;
	}
	BYTE byteFill = 0X0;
	size_t stWrite = fwrite(&byteFill, sizeof(BYTE), sizeof(WAVHeadInfo), m_pFile);
	if (stWrite < sizeof(WAVHeadInfo))
	{
		fclose(m_pFile);
		m_pFile = NULL;
		StopRecord();
		return;
	}
}

void AudioRecordThread::OnMM_WIM_DATA(WPARAM wParam, LPARAM lParam)
{
	if (m_pFile == NULL)
	{
		return;
	}

	size_t stWrite = fwrite(((PWAVEHDR)lParam)->lpData, sizeof(BYTE), ((PWAVEHDR)lParam)->dwBytesRecorded, m_pFile);
	if (!m_bRecording)
	{
		waveInClose(m_hWaveInDev);
		return;
	}
	waveInAddBuffer(m_hWaveInDev, (PWAVEHDR)lParam, sizeof(WAVEHDR));
}

void AudioRecordThread::OnMM_WIM_CLOSE(WPARAM wParam, LPARAM lParam)
{
	if (m_bCancelReord)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		waveInUnprepareHeader(m_hWaveInDev, &m_wavehdr1, sizeof(WAVEHDR));
		waveInUnprepareHeader(m_hWaveInDev, &m_wavehdr2, sizeof(WAVEHDR));
		delete[] m_pBuffer1;
		delete[] m_pBuffer2;

		m_bCancelReord = false;
		return;
	}

	fseek(m_pFile, 0, SEEK_END);
	long nFileSize = ftell(m_pFile);

	WAVHeadInfo headInfo;
	headInfo.riffID[0] = 'R';
	headInfo.riffID[1] = 'I';
	headInfo.riffID[2] = 'F';
	headInfo.riffID[3] = 'F';
	headInfo.fileSize = nFileSize - 8;
	headInfo.riffFormat[0] = 'W';
	headInfo.riffFormat[1] = 'A';
	headInfo.riffFormat[2] = 'V';
	headInfo.riffFormat[3] = 'E';
	headInfo.fmtID[0] = 'f';
	headInfo.fmtID[1] = 'm';
	headInfo.fmtID[2] = 't';
	headInfo.fmtID[3] = 0X20;
	headInfo.fmtSize = 16;
	headInfo.formatTag = 0x0001;
	headInfo.channels = m_waveFormat.nChannels;
	headInfo.sampleFrequency = m_waveFormat.nSamplesPerSec;
	headInfo.sampleBitSize = m_waveFormat.wBitsPerSample;
	headInfo.byteRate = m_waveFormat.nAvgBytesPerSec;
	headInfo.blockAlign = m_waveFormat.nBlockAlign;
	headInfo.dataID[0] = 'd';
	headInfo.dataID[1] = 'a';
	headInfo.dataID[2] = 't';
	headInfo.dataID[3] = 'a';
	headInfo.dataChunkSize = nFileSize - sizeof(WAVHeadInfo);

	fseek(m_pFile, 0, SEEK_SET);
	size_t stWrite = fwrite(&headInfo, sizeof(headInfo), 1, m_pFile);

	fclose(m_pFile);
	m_pFile = NULL;
	waveInUnprepareHeader(m_hWaveInDev, &m_wavehdr1, sizeof(WAVEHDR));
	waveInUnprepareHeader(m_hWaveInDev, &m_wavehdr2, sizeof(WAVEHDR));
	delete[] m_pBuffer1;
	delete[] m_pBuffer2;

	if (m_pSpeechManager != NULL)
	{
		m_pSpeechManager->RecordDone(m_wstrRecordPath);
	}

	return;
}
