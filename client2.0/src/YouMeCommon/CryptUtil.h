#pragma once
#include <CrossPlatformDefine/PlatformDef.h>
#include <YouMeCommon/XSharedArray.cpp>
namespace youmecommon
{
class CCryptUtil
{
public:
	CCryptUtil(void);
	~CCryptUtil(void);

	/************************************************************************/
	/*                                   BASE64                             */
	/************************************************************************/
	static bool Base64Decoder( const std::string& input ,CXSharedArray<char>& pBuffer);
	static bool Base64Decoder( const byte* buffer,int iLength ,CXSharedArray<char>& pBuffer);
	static void Base64Encoder( const std::string& input ,std::string& output);
	static void Base64Encoder( const byte* buffer,int iLength ,std::string& output);


	/************************************************************************/
	/*                                      MD5                             */
	/************************************************************************/
	static XString MD5(const std::string& strPlainText);
	static XString MD5(const byte* pBuffer, int iSize);

	//文件MD5
	static XString MD5File(const XString& strFilePath);
};
}