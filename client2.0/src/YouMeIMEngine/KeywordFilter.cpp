#include "KeywordFilter.h"
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/tinyxml/tinyxml.h>

CKeywordFilter::CKeywordFilter()
{
	m_bIsLoadSuccess = false;
}


CKeywordFilter::~CKeywordFilter()
{
}

bool CKeywordFilter::LoadFile(const XString&strSrcPath)
{
	m_keywordVect.clear();
	TiXmlDocument mXMLDoc;
	bool bOpen = mXMLDoc.LoadFile(strSrcPath.c_str());
	if (!bOpen)
	{
		return false;
	}
	TiXmlElement* pRoot = mXMLDoc.RootElement();
	if (NULL == pRoot)
	{
		return false;
	}
	m_strReplaceText = pRoot->Attribute("replace");
	if (m_strReplaceText == __XT(""))
	{
		m_strReplaceText = __XT("**");
	}
	TiXmlElement* pChild = pRoot->FirstChildElement("item");
	while (NULL != pChild)
	{		
		std::shared_ptr<KeywordInfo> pPtr(new KeywordInfo);
		pPtr->strKeyword = pChild->Attribute("text");

		m_keywordVect.push_back(pPtr);
		pChild = pChild->NextSiblingElement();
	}
	m_bIsLoadSuccess = true;
	return true;
}

XString CKeywordFilter::GetFilterText(const XString& strSource)
{
	XString strTmpSource = strSource;
	for (size_t i = 0; i < m_keywordVect.size();i++)
	{
		strTmpSource = CStringUtil::replace_text(strTmpSource, m_keywordVect[i]->strKeyword, m_strReplaceText);
	}
	return strTmpSource;
}
