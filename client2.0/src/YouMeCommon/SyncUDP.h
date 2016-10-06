#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "XSharedArray.h"
#include <string>
#include <map>
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#endif
#include "XCondWait.h"
//同步TCP 连接，使用的时候需要避免TCP 长链接,不要多线程调用，否则后果很严重
namespace youmecommon {
   
class CSyncUDP
{
public:
	CSyncUDP();
	~CSyncUDP();
	//创建socket，只有真正发送数据的时候才会去connect fuwuqi，单位毫秒
	bool Init(const XString &strServerIP, int iPort);
	void UnInit();

	//直接就发送了，同步的，-1 发生错误, 
	int SendData(const char* buffer, int iBufferSize);
	int RecvData(CXSharedArray<char>& recvBuffer,int iTimeout,CXCondWait* pCondiWait);
private:
	void Close();
	int m_client;
	sockaddr_in m_serverAddr;
};
}