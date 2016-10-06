#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <memory>
struct KeywordInfo
{
	XString strKeyword;
};
class CKeywordFilter
{
public:
	CKeywordFilter();
	~CKeywordFilter();
	bool IsLoadSuccess()
	{
		return m_bIsLoadSuccess;
	}
	bool LoadFile(const XString&strSrcPath);
	//世界上最简单的处理方式
	XString GetFilterText(const XString& strSource);
private:
	XString m_strReplaceText;
	std::vector<std::shared_ptr<KeywordInfo> >m_keywordVect;
	bool m_bIsLoadSuccess;
};

