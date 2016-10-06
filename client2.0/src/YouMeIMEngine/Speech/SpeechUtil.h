#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>


struct WAVHeadInfo
{
	// RIFF chunk
	char riffID[4];					//"RIFF"
	unsigned int fileSize;
	char riffFormat[4];				//"WAVE"
	// format chunk
	char fmtID[4];					//"fmt "
	unsigned int fmtSize;
	unsigned short formatTag;		//编码方式
	unsigned short channels;		//声道数
	unsigned int sampleFrequency;	//采样频率
	unsigned int byteRate;			//比特率
	unsigned short blockAlign;		//块对齐大小
	unsigned short sampleBitSize;	//样本宽度
	// data chunk
	char dataID[4];					//"data"
	unsigned int dataChunkSize;
};


class SpeechUtil
{
public:
	static bool PcmToWav(const XString& srcPath, unsigned short sampleBitSize, unsigned short channels, unsigned long sampleFrequency, int format = 0, const XString desPath = __XT(""));
	static int GetWavTime(const XString& path);
	static int GetAmrnbTime(const XString& path);

private:
	static int GetAmrFrameSize(int frameHeader);
};