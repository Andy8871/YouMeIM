#include "PlatformDef.h"

#ifdef WIN32
#include <Windows.h>
std::string Unicode_to_Local(const wchar_t* in, int len)
{
	std::string  out;
	UINT CP_GBK = GetOEMCP();
	int bufferlen = (int)::WideCharToMultiByte(CP_GBK, 0, in, len, NULL, 0, NULL, NULL);
	char* pBuffer = new char[bufferlen + 4];
	if (NULL == pBuffer)
	{
		return "";
	}
	int out_len = ::WideCharToMultiByte(CP_GBK, 0, in, len, pBuffer, bufferlen + 2, NULL, NULL);
	pBuffer[bufferlen] = '\0';
	out.assign(pBuffer, out_len);
	delete[] pBuffer;
	return out;
}

std::string Unicode_to_UTF8(const wchar_t* in, int len)
{
	std::string out;
	size_t out_len = len * 3 + 1;
	char* pBuf = new char[out_len];
	if (NULL == pBuf)
	{
		return "";
	}
	char* pResult = pBuf;
	memset(pBuf, 0, out_len);


	out_len = ::WideCharToMultiByte(CP_UTF8, 0, in, len, pBuf, len * 3, NULL, NULL);
	out.assign(pResult, out_len);
	delete[] pResult;
	pResult = NULL;
	return out;
}

XString UTF8_to_Unicode(const char* in, int len)
{
	wchar_t* pBuf = new wchar_t[len + 1];
	if (NULL == pBuf)
	{
		return __XT("");
	}
	size_t out_len = (len + 1) * sizeof(wchar_t);
	memset(pBuf, 0, (len + 1) * sizeof(wchar_t));
	wchar_t* pResult = pBuf;


	out_len = ::MultiByteToWideChar(CP_UTF8, 0, in, len, pBuf, len * sizeof(wchar_t));
	XString out;
	out.assign(pResult, out_len);


	delete[] pResult;
	pResult = NULL;
	return out;
}

XString Local_to_Unicode(const char* in, int len)
{
	UINT CP_GBK = GetOEMCP();
	XString out;
	int wbufferlen = (int)::MultiByteToWideChar(CP_GBK, MB_PRECOMPOSED, in, len, NULL, 0);
	wchar_t* pwbuffer = new wchar_t[wbufferlen + 4];
	if (NULL == pwbuffer)
	{
		return out;
	}
	wbufferlen = (int)::MultiByteToWideChar(CP_GBK, MB_PRECOMPOSED, in, len, pwbuffer, wbufferlen + 2);
	pwbuffer[wbufferlen] = '\0';
	out.assign(pwbuffer, wbufferlen);
	delete[] pwbuffer;
	return out;
}

#endif // WIN32