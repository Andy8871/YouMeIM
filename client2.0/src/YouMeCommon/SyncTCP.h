#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include "XSharedArray.h"
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <string>
//同步TCP 连接，使用的时候需要避免TCP 长链接,不要多线程调用，否则后果很严重
namespace youmecommon {

class CSyncTCP
{
public:
	CSyncTCP();
	~CSyncTCP();
	//创建socket，只有真正发送数据的时候才会去connect ，单位毫秒
	bool Init(const XString&strServerIP, int iPort, int iConnectTimeout = 10);
	void UnInit();

	//直接就发送了，同步的，-1 发生错误,
    bool Connect(int iTimeOut);//单位毫秒
	int SendData(const char* buffer, int iBufferSize);
	int RecvData(CXSharedArray<char>& recvBuffer);
    //返回实际发送的个数，如果成功
    int SendBufferData(const char*buffer, int iBufferSize);
    int RecvDataByLen(int iLen, CXSharedArray<char>& recvBuffer);
	int RawSocket();
private:

	
	int GetRecvDataLen();
	void Close();
	int m_client;
	XString m_strServerIP;
	int m_iServerPort;
};
}