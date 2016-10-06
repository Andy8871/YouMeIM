#include "SyncTCP.h"
#include <assert.h>
#include <time.h>
#include <YouMeCommon/Log.h>
#ifndef WIN32
#include <errno.h>

#endif
namespace youmecommon {

CSyncTCP::CSyncTCP()
{
	m_client = -1;
}
CSyncTCP::~CSyncTCP()
{
	Close();
}

void CSyncTCP::Close()
{
	if (-1 != m_client)
	{
#ifdef WIN32
		shutdown(m_client, SD_BOTH);
		closesocket(m_client);
#else
		shutdown(m_client, SHUT_RDWR);
		close(m_client);
#endif
		m_client = -1;
	}
	
}

bool CSyncTCP::Init(const XString&strServerIP, int iPort,int iConnectTimeout)
{
	if (-1 != m_client)
	{
		return true;
	}
	m_client = socket(AF_INET, SOCK_STREAM, 0);
	assert(-1 != m_client);
	if (-1 == m_client)
	{
		return false;
	}
	m_strServerIP = strServerIP;
	m_iServerPort = iPort;

	int r = 1;
#ifdef WIN32
	
#elif (OS_IOS || OS_IOSSIMULATOR || OS_OSX)
	setsockopt(m_client, SOL_SOCKET, SO_NOSIGPIPE, &r, sizeof(int));
#elif OS_ANDROID
	setsockopt(m_client, SOL_SOCKET, MSG_NOSIGNAL, &r, sizeof(int));
#else
#error "nosupport"
#endif

//设置超时
#ifdef WIN32
	DWORD sendTimeOut = iConnectTimeout*1000;
	int iRet = setsockopt(m_client, SOL_SOCKET, SO_SNDTIMEO, (const char*)&sendTimeOut, sizeof(sendTimeOut));
#else
	struct timeval t;
	t.tv_sec = iConnectTimeout;	
	t.tv_usec = 0;
	int iRet = setsockopt(m_client, SOL_SOCKET, SO_SNDTIMEO, (const char*)&t, sizeof(t));
#endif // WIN32

	
	return true;
}

bool CSyncTCP::Connect(int iTimeOut)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(m_iServerPort);
	serverAddr.sin_addr.s_addr = inet_addr(XStringToUTF8(m_strServerIP).c_str());
    return 0 == ::connect(m_client, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
}


int CSyncTCP::SendData(const char* buffer, int iBufferSize)
{
	//先发送数据的长度,转换成网络字节序
	int iNetOrder = htonl(iBufferSize);
	if (sizeof(int) != SendBufferData((const char*)&iNetOrder, sizeof(int)))
	{
		return -1;
	}
	return SendBufferData(buffer, iBufferSize);
}


void CSyncTCP::UnInit()
{
	Close();
}

int CSyncTCP::SendBufferData(const char*buffer, int iBufferSize)
{
	int iSend = 0;
	//4字节的长度
	while (iSend != iBufferSize)
	{
		int iTmpSend = send(m_client, buffer + iSend, iBufferSize - iSend,0);
		if (iTmpSend <= 0)
		{
			break;
		}
		iSend += iTmpSend;
	}
	if (iSend != iBufferSize)
	{
		return iSend;
	}
	return iBufferSize;
}

int CSyncTCP::GetRecvDataLen()
{
	//4字节长度
	CXSharedArray<char> recvBuffer;
	if (4 != RecvDataByLen(4, recvBuffer))
	{
		return -1;
	}
	int iDataLen = *(int*)recvBuffer.Get();
	return ntohl(iDataLen);
}

int CSyncTCP::RecvDataByLen(int iLen, CXSharedArray<char>& recvBuffer)
{
	int iRecv = 0;
	recvBuffer.Allocate(iLen);
	while (true)
	{
		int iCurRecv = (int)recv(m_client, recvBuffer.Get() + iRecv, iLen - iRecv, 0);
		if (iCurRecv <= 0)
		{
#ifdef WIN32
			//WSAETIMEDOUT
			int iErrorcode = WSAGetLastError();
#else
			int iErrorcode = errno;
#endif
			break;
		}

		iRecv += iCurRecv;
		if (iRecv == iLen)
		{
			break;
		}
	}
	return iRecv;
}
#define MAXRECVLEN 5*1024*1024
int CSyncTCP::RecvData(CXSharedArray<char>& recvBuffer)
{
	//先接收4字节的长度
	int iDataLen = GetRecvDataLen();
	if (-1 == iDataLen || iDataLen > MAXRECVLEN)
	{
		return -1;
	}
	return RecvDataByLen(iDataLen, recvBuffer);
}

int CSyncTCP::RawSocket()
{
	return m_client;
}
}

