#include "cocos2d.h"
#include "utils.h"
#include "cocostudio/CocoStudio.h"
#include "YouMeMsgBox.h"

USING_NS_CC;

#ifdef WIN32
std::string AnsiToUtf8(std::string strAnsi)
{
	std::string ret;
	if (strAnsi.length() > 0)
	{
		int nWideStrLength = MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
		WCHAR* pwszBuf = (WCHAR*)malloc((nWideStrLength + 1)*sizeof(WCHAR));
		memset(pwszBuf, 0, (nWideStrLength + 1)*sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, pwszBuf, (nWideStrLength + 1)*sizeof(WCHAR));

		int nUtf8Length = WideCharToMultiByte(CP_UTF8, 0, pwszBuf, -1, NULL, 0, NULL, FALSE);
		char* pszUtf8Buf = (char*)malloc((nUtf8Length + 1)*sizeof(char));
		memset(pszUtf8Buf, 0, (nUtf8Length + 1)*sizeof(char));

		WideCharToMultiByte(CP_UTF8, 0, pwszBuf, -1, pszUtf8Buf, (nUtf8Length + 1)*sizeof(char), NULL, FALSE);
		ret = pszUtf8Buf;

		free(pszUtf8Buf);
		free(pwszBuf);
	}
	return ret;
}

std::string Utf8ToAnsi(std::string strUTF8)
{
	std::string ret;
	if (strUTF8.length() > 0)
	{
		int nWideStrLength = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
		WCHAR* pwszBuf = (WCHAR*)malloc((nWideStrLength + 1)*sizeof(WCHAR));
		memset(pwszBuf, 0, (nWideStrLength + 1)*sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, pwszBuf, (nWideStrLength + 1)*sizeof(WCHAR));

		int nAnsiStrLength = WideCharToMultiByte(CP_ACP, 0, pwszBuf, -1, NULL, 0, NULL, FALSE);
		char* pszAnsiBuf = (char*)malloc((nAnsiStrLength + 1)*sizeof(char));
		memset(pszAnsiBuf, 0, (nAnsiStrLength + 1)*sizeof(char));

		WideCharToMultiByte(CP_ACP, 0, pwszBuf, -1, pszAnsiBuf, (nAnsiStrLength + 1)*sizeof(char), NULL, FALSE);
		ret = pszAnsiBuf;

		free(pszAnsiBuf);
		free(pwszBuf);
	}

	return ret;
}

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

#endif

void PopTip(const std::string& strTip, int nType)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto layer = YouMeMsgBox::create();
	layer->InitPara(strTip, nType);
	layer->ignoreAnchorPointForPosition(false);
	layer->setContentSize(Size(400, 180));
	layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	layer->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	Scene* pCurSene = Director::getInstance()->getRunningScene();
	pCurSene->addChild(layer);
}