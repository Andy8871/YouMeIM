#pragma once
#include <CrossPlatformDefine/PlatformDef.h>
#include <string>
#include <vector>
#include <XCondWait.h>
//同步的DNS 解析
namespace youmecommon {

class CXDNSParse
{
public:
	static CXDNSParse* Instance();

	//超时单位 毫秒
	bool ParseDomain2(const XString& strDomain, std::vector<XString>& ipList, int iTimeOut = 2000);
    bool IsIPAddress(const XString& strValue);

private:
	CXDNSParse(){};
	CXDNSParse(const CXCondWait&);
	CXDNSParse& operator= (const CXDNSParse&);

	void DomainParseThread(const XString& domain);

	CXCondWait m_domainParseCondWait;
	std::vector<XString> m_ipList;
};
}