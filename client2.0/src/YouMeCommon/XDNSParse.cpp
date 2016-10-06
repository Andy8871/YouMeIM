#include "XDNSParse.h"
#include <time.h>
#include <YouMeCommon/Log.h>
#ifndef WIN32
#include <netdb.h>
#include <sys/select.h>
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif // !WIN32

#include <thread>

namespace youmecommon {


CXDNSParse* CXDNSParse::Instance()
{
	static CXDNSParse instance;
	return &instance;
}

bool CXDNSParse::IsIPAddress(const XString& strValue)
{
    int iDot1 =0;
    int iDot2 =0;
    int iDot3 =0;
    int iDot4 = 0;
#ifdef WIN32 
    swscanf(strValue.c_str(), __XT("%d.%d.%d.%d"),&iDot1,&iDot2,&iDot3,&iDot4);
#else
    sscanf(strValue.c_str(), "%d.%d.%d.%d",&iDot1,&iDot2,&iDot3,&iDot4);
#endif
        if ((iDot1 > 255)
        || (iDot2 >255)
        || (iDot3 >255)
        || (iDot4 > 255)
        || (iDot1 <=0)
        || (iDot2 <=0)
        || (iDot3 <=0)
        || (iDot4 <=0))
    {
        return false;
    }
    return true;
}
bool CXDNSParse::ParseDomain2(const XString& strDomain, std::vector<XString>& ipList, int iTimeOut)
{
	/*std::thread domainThread(&CXDNSParse::DomainParseThread, this, strDomain);
	domainThread.detach();
	m_domainParseCondWait.Reset();
	m_domainParseCondWait.WaitTime(iTimeOut);
	if (m_ipList.size() > 0)
	{
		ipList = m_ipList;
		return true;
	}
	return false;*/
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    struct addrinfo *result = NULL;
    std::string strUTF8Domain = XStringToUTF8(strDomain);
    int iRet = getaddrinfo(strUTF8Domain.c_str(), NULL, &hints, &result);
    if (iRet != 0) {
        return false;
    }
    
    struct addrinfo* pAddrInfo = NULL;
    for (pAddrInfo = result; pAddrInfo != NULL; pAddrInfo = pAddrInfo->ai_next)
    {
        switch (pAddrInfo->ai_family)
        {
            case AF_INET:
            {
                struct sockaddr_in* pSockaddrIpv4 = (struct sockaddr_in*) pAddrInfo->ai_addr;
                char szAddr[16];
                memset(szAddr, 0, 16);
                inet_ntop(AF_INET, &pSockaddrIpv4->sin_addr, szAddr, 16);
                if (strlen(szAddr) > 0)
                {
                    std::string strTemp = std::string(szAddr);
                    XString strIp = UTF8TOXString(strTemp);
                    ipList.push_back(strIp);
                }
            }
                break;
            case AF_INET6:
            {
                char szAddr[64];
                memset(szAddr, 0, 64);
                struct sockaddr_in6* pSockaddrIpv6 = (struct sockaddr_in6*) pAddrInfo->ai_addr;
                inet_ntop(AF_INET6, &pSockaddrIpv6->sin6_addr, szAddr, 64);
                if (strlen(szAddr) > 0)
                {
                    std::string strTemp = std::string(szAddr);
                    XString strIp = UTF8TOXString(strTemp);
                    ipList.push_back(strIp);
                }
            }
                break;
            default:
                break;
        }
    }
    
    freeaddrinfo(result);
    return true;
}
    
void CXDNSParse::DomainParseThread(const XString& domain)
{
	m_ipList.clear();
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	struct addrinfo *result = NULL;
	std::string strUTF8Domain = XStringToUTF8(domain);
	int iRet = getaddrinfo(strUTF8Domain.c_str(), NULL, &hints, &result);
	if (iRet != 0) {
		m_domainParseCondWait.SetSignal();
		return;
	}

	struct addrinfo* pAddrInfo = NULL;
	for (pAddrInfo = result; pAddrInfo != NULL; pAddrInfo = pAddrInfo->ai_next)
	{
		switch (pAddrInfo->ai_family)
		{
		case AF_INET:
		{
			struct sockaddr_in* pSockaddrIpv4 = (struct sockaddr_in*) pAddrInfo->ai_addr;
			char szAddr[16];
			memset(szAddr, 0, 16);
			inet_ntop(AF_INET, &pSockaddrIpv4->sin_addr, szAddr, 16);
			if (strlen(szAddr) > 0)
			{
                std::string strTemp = std::string(szAddr);
                XString strIp = UTF8TOXString(strTemp);
				m_ipList.push_back(strIp);
			}
		}
		break;
		case AF_INET6:
		{
			char szAddr[64];
			memset(szAddr, 0, 64);
			struct sockaddr_in6* pSockaddrIpv6 = (struct sockaddr_in6*) pAddrInfo->ai_addr;
			inet_ntop(AF_INET6, &pSockaddrIpv6->sin6_addr, szAddr, 64);
			if (strlen(szAddr) > 0)
			{
                std::string strTemp = std::string(szAddr);
                XString strIp = UTF8TOXString(strTemp);
				m_ipList.push_back(strIp);
			}
		}
		break;
		default:
			break;
		}
	}

	freeaddrinfo(result);

	m_domainParseCondWait.SetSignal();
}

}