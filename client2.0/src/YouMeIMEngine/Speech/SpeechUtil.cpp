#include "SpeechUtil.h"
#include "YouMeCommon/XFile.h"
#include <exception>


bool SpeechUtil::PcmToWav(const XString& srcPath, unsigned short sampleBitSize, unsigned short channels, unsigned long sampleFrequency, int format, const XString desPath)
{
	if (srcPath.empty())
	{
		return false;
	}
	XString strDesPath = desPath;
	if (strDesPath.empty())
	{
		XString::size_type stPos = srcPath.find_last_of(__XT("."));
		if (stPos == XString::npos)
		{
			return false;
		}
		strDesPath = srcPath.substr(0, stPos + 1);
		strDesPath.append(__XT("wav"));
	}

	FILE* pFileSrc = NULL;
	FILE* pFileDes = NULL;
#ifdef WIN32
	errno_t errCode = _wfopen_s(&pFileSrc, srcPath.c_str(), __XT("rb"));
	if (errCode != 0 || NULL == pFileSrc)
	{
		return false;
	}
	errCode = _wfopen_s(&pFileDes, strDesPath.c_str(), __XT("wb+"));
	if (errCode != 0 || NULL == pFileDes)
	{
		fclose(pFileSrc);
		return false;
	}
#else
	std::string strSrcTmp = XStringToUTF8(srcPath);
	pFileSrc = fopen(strSrcTmp.c_str(), __XT("rb"));
	if (NULL == pFileSrc)
	{
		return false;
	}
	std::string strDesTmp = XStringToUTF8(strDesPath);
	pFileDes = fopen(strDesTmp.c_str(), __XT("wb+"));
	if (NULL == pFileDes)
	{
		fclose(pFileSrc);
		return false;
	}
#endif
	
	fseek(pFileSrc, 0, SEEK_END);
	long srcFileSize = ftell(pFileSrc);
	if (0 == srcFileSize)
	{
		return false;
	}

	WAVHeadInfo headInfo;
	headInfo.riffID[0] = 'R';
	headInfo.riffID[1] = 'I';
	headInfo.riffID[2] = 'F';
	headInfo.riffID[3] = 'F';
	headInfo.fileSize = srcFileSize + sizeof(WAVHeadInfo) - 8;
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
	headInfo.channels = channels;
	headInfo.sampleFrequency = sampleFrequency;
	headInfo.sampleBitSize = sampleBitSize;
	headInfo.byteRate = headInfo.sampleFrequency * headInfo.channels * headInfo.sampleBitSize / 8;
	headInfo.blockAlign = headInfo.channels * headInfo.sampleBitSize / 8;
	headInfo.dataID[0] = 'd';
	headInfo.dataID[1] = 'a';
	headInfo.dataID[2] = 't';
	headInfo.dataID[3] = 'a';
	headInfo.dataChunkSize = srcFileSize;
	size_t stWrite = fwrite(&headInfo, 1, sizeof(headInfo), pFileDes);
	if (stWrite < sizeof(headInfo))
	{
		fclose(pFileSrc);
		fclose(pFileDes);
		return false;
	}
    if (format == 1)
    {
        fseek(pFileSrc, 4096, SEEK_SET);
    }
    else
    {
        fseek(pFileSrc, 0, SEEK_SET);
    }
	
	while (!feof(pFileSrc))
	{
		char szBuf[4096];
		size_t stRead = fread(szBuf, 1, 4096, pFileSrc);
		if (stRead > 0)
		{
			fwrite(szBuf, 1, stRead, pFileDes);
		}
	}

	fclose(pFileSrc);
	fclose(pFileDes);

	return true;
}

int SpeechUtil::GetWavTime(const XString& path)
{
	FILE* pFile = NULL;
#ifdef WIN32
	errno_t errCode = _wfopen_s(&pFile, path.c_str(), __XT("rb"));
	if (errCode != 0 || NULL == pFile)
	{
		return false;
	}
#else
	pFile = fopen(path.c_str(), __XT("rb"));
	if (NULL == pFile)
	{
		return false;
	}
#endif
	fseek(pFile, 0X1C, SEEK_SET);
	unsigned int byteRate = 0;
	size_t stRead = fread(&byteRate, sizeof(unsigned int), 1, pFile);
	if (byteRate == 0)
	{
		fclose(pFile);
		return 0;
	}
	fseek(pFile, 0X28, SEEK_SET);
	unsigned int dataChunkSize = 0;
	stRead = fread(&dataChunkSize, sizeof(unsigned int), 1, pFile);
	if (dataChunkSize <= sizeof(unsigned int))
	{
		fclose(pFile);
		return 0;
	}
	fclose(pFile);
	
	int nTime = dataChunkSize / byteRate;
	return nTime == 0 ? 1 : nTime;
}

int SpeechUtil::GetAmrnbTime(const XString& path)
{
	FILE* pFile = NULL;
#ifdef WIN32
	errno_t errCode = _wfopen_s(&pFile, path.c_str(), __XT("rb"));
	if (errCode != 0 || NULL == pFile)
	{
		return 0;
	}
#else
	pFile = fopen(path.c_str(), __XT("rb"));
	if (NULL == pFile)
	{
		return 0;
	}
#endif
	fseek(pFile, 0, SEEK_END);
	long fileSize = ftell(pFile);
	if (fileSize <= 6)
	{
		fclose(pFile);
		return 0;
	}
	fseek(pFile, 0, SEEK_SET);

	int headSize = 0;
	unsigned char header[16] = { 0 };
	size_t read = fread(header, 1, sizeof(header), pFile);
	if (read != sizeof(header))
	{
		fclose(pFile);
		return 0;
	}
	if (memcmp(header, "#!AMR\n", 6) == 0)
	{
		headSize = 6;
	}
	else if (memcmp(header, "#!AMR_MC1.0\n", 12) == 0)
	{
		headSize = 16;
	}
	else if (memcmp(header, "#!AMR-WB\n", 9) == 0)
	{
		headSize = 9;
	}
	else if (memcmp(header, "#!AMR-WB_MC1.0\n", 15) == 0)
	{
		headSize = 19;
	}

	if (headSize == 0)
	{
		fclose(pFile);
		return 0;
	}
	int frameHeader = 0;
	fseek(pFile, headSize, SEEK_SET);
	read = fread(&frameHeader, 1, 1, pFile);
	if (read != 1)
	{
		fclose(pFile);
		return 0;
	}
	int frameSize = GetAmrFrameSize(frameHeader);
	fclose(pFile);

	if (frameSize == 0 || headSize == 0)
	{
		return 0;
	}
	int nTime = (fileSize - headSize + 1) / frameSize / 50;
	return nTime == 0 ? 1 : nTime;
}

int SpeechUtil::GetAmrFrameSize(int frameHeader)
{
	int frameSize = 0;
	switch (frameHeader)
	{
	case 0X04:
		frameSize = 13;
		break;
	case 0X0C:
		frameSize = 14;
		break;
	case 0X14:
		frameSize = 16;
		break;
	case 0X1C:
		frameSize = 18;
		break;
	case 0X24:
		frameSize = 20;
		break;
	case 0X2C:
		frameSize = 21;
		break;
	case 0X34:
		frameSize = 27;
		break;
	case 0X3C:
		frameSize = 32;
		break;
	default:
		break;
	}
	return frameSize;
}