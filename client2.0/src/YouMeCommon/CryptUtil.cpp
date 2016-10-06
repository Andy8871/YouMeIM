#include "CryptUtil.h"
#include <YouMeCommon/crypto/md5/md5.h>
#include <YouMeCommon/XFile.h>
#include <XSharedArray.h>
#include <YouMeCommon/StringUtil.hpp>
namespace youmecommon
{
CCryptUtil::CCryptUtil(void)
{
}

CCryptUtil::~CCryptUtil(void)
{

}
static const char *g_pCodes =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static const unsigned char g_pMap[256] =
{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255,
	255, 254, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
	19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
	37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
};


bool CCryptUtil::Base64Decoder( const std::string& input ,CXSharedArray<char>& pBuffer )
{
	return Base64Decoder((const byte*)input.c_str(), input.length(),pBuffer);
}
static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}
bool CCryptUtil::Base64Decoder( const byte* buffer,int iLength ,CXSharedArray<char>& pBuffer )
{
	static const std::string::size_type np = std::string::npos;
	static const std::string::size_type DecodeTable[] = {
		// 0   1   2   3   4   5   6   7   8   9 
		np, np, np, np, np, np, np, np, np, np,  //   0 -   9
		np, np, np, np, np, np, np, np, np, np,  //  10 -  19
		np, np, np, np, np, np, np, np, np, np,  //  20 -  29
		np, np, np, np, np, np, np, np, np, np,  //  30 -  39
		np, np, np, 62, np, np, np, 63, 52, 53,  //  40 -  49
		54, 55, 56, 57, 58, 59, 60, 61, np, np,  //  50 -  59
		np, np, np, np, np, 0, 1, 2, 3, 4,  //  60 -  69
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14,  //  70 -  79
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  //  80 -  89
		25, np, np, np, np, np, np, 26, 27, 28,  //  90 -  99
		29, 30, 31, 32, 33, 34, 35, 36, 37, 38,  // 100 - 109
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48,  // 110 - 119
		49, 50, 51, np, np, np, np, np, np, np,  // 120 - 129
		np, np, np, np, np, np, np, np, np, np,  // 130 - 139
		np, np, np, np, np, np, np, np, np, np,  // 140 - 149
		np, np, np, np, np, np, np, np, np, np,  // 150 - 159
		np, np, np, np, np, np, np, np, np, np,  // 160 - 169
		np, np, np, np, np, np, np, np, np, np,  // 170 - 179
		np, np, np, np, np, np, np, np, np, np,  // 180 - 189
		np, np, np, np, np, np, np, np, np, np,  // 190 - 199
		np, np, np, np, np, np, np, np, np, np,  // 200 - 209
		np, np, np, np, np, np, np, np, np, np,  // 210 - 219
		np, np, np, np, np, np, np, np, np, np,  // 220 - 229
		np, np, np, np, np, np, np, np, np, np,  // 230 - 239
		np, np, np, np, np, np, np, np, np, np,  // 240 - 249
		np, np, np, np, np, np                   // 250 - 256
	};
	static const char cFillChar = '=';

	std::string::size_type nLength = iLength;
	std::string            sResult;

	sResult.reserve(nLength);

	for (std::string::size_type nPos = 0; nPos < nLength; nPos++) {
		unsigned char c, c1;

		c = (char)DecodeTable[(unsigned char)buffer[nPos]];
		nPos++;
		c1 = (char)DecodeTable[(unsigned char)buffer[nPos]];
		c = (c << 2) | ((c1 >> 4) & 0x3);
		sResult.append(1, c);

		if (++nPos < nLength) {
			c = buffer[nPos];
			if (cFillChar == c)
				break;

			c = (char)DecodeTable[(unsigned char)buffer[nPos]];
			c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
			sResult.append(1, c1);
		}

		if (++nPos < nLength) {
			c1 = buffer[nPos];
			if (cFillChar == c1)
				break;

			c1 = (char)DecodeTable[(unsigned char)buffer[nPos]];
			c = ((c << 6) & 0xc0) | c1;
			sResult.append(1, c);
		}
	}
	pBuffer.Allocate(sResult.length());
	memcpy(pBuffer.Get(), sResult.c_str(), sResult.length());
	return true;
}

void CCryptUtil::Base64Encoder( const std::string& input ,std::string& output )
{
	Base64Encoder((const byte*)input.c_str(), input.length(),output);
}

void CCryptUtil::Base64Encoder(const byte* buffer, int iLength, std::string& sResult)
{
	static const std::string sBase64Table(
		// 0000000000111111111122222222223333333333444444444455555555556666
		// 0123456789012345678901234567890123456789012345678901234567890123
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
		);
	static const char cFillChar = '=';
	std::string::size_type   nLength = iLength;

	// Allocate memory for the converted string
	sResult.reserve(nLength * 8 / 6 + 1);

	for (std::string::size_type nPos = 0; nPos < nLength; nPos++) {
		char cCode;

		// Encode the first 6 bits
		cCode = (buffer[nPos] >> 2) & 0x3f;
		sResult.append(1, sBase64Table[cCode]);

		// Encode the remaining 2 bits with the next 4 bits (if present)
		cCode = (buffer[nPos] << 4) & 0x3f;
		if (++nPos < nLength)
			cCode |= (buffer[nPos] >> 4) & 0x0f;
		sResult.append(1, sBase64Table[cCode]);

		if (nPos < nLength) {
			cCode = (buffer[nPos] << 2) & 0x3f;
			if (++nPos < nLength)
				cCode |= (buffer[nPos] >> 6) & 0x03;

			sResult.append(1, sBase64Table[cCode]);
		}
		else {
			++nPos;
			sResult.append(1, cFillChar);
		}

		if (nPos < nLength) {
			cCode = buffer[nPos] & 0x3f;
			sResult.append(1, sBase64Table[cCode]);
		}
		else {
			sResult.append(1, cFillChar);
		}
	}
}

XString CCryptUtil::MD5(const std::string& strPlainText)
{
    return MD5((const byte*)strPlainText.c_str(), strPlainText.length());
}

XString CCryptUtil::MD5(const byte* pBuffer, int iSize)
{
    unsigned char digest[16] = {0};
    memset(digest, 0x00, sizeof(digest));
    
    youmecommon::MD5_CTX ctx;
    youmecommon::MD5_Init(&ctx);
    youmecommon::MD5_Update(&ctx, pBuffer, iSize);
    youmecommon::MD5_Final(digest, &ctx);
	return CStringUtil::bytes_to_hex_string(digest, 16);
}

XString CCryptUtil::MD5File(const XString& strFilePath)
{
	CXFile file;
	if (0 != file.LoadFile(strFilePath, CXFile::Mode_OpenExist))
	{
		return __XT("");
	}

	char buffer[1204] = { 0 };
	unsigned char digest[16] = { 0 };
	memset(digest, 0x00, sizeof(digest));
	youmecommon::MD5_CTX ctx;
	youmecommon::MD5_Init(&ctx);
	while (true)
	{
		int iReadLen = file.Read((byte*)buffer, 1024);
		if (iReadLen <=0)
		{
			break;
		}
		youmecommon::MD5_Update(&ctx, buffer, iReadLen);
	}
	
	youmecommon::MD5_Final(digest, &ctx);
	return CStringUtil::bytes_to_hex_string(digest, 16);

}

}