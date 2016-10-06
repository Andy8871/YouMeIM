#include "SyncUDP.h"
#include <assert.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#else
#define  MSG_DONTWAIT 0
#endif
#include <YouMeCommon/Log.h>

namespace youmecommon {
    
//internet MTU
#define INTERNET_MTU 1460
CSyncUDP::CSyncUDP()
{
	m_client = -1;
}
CSyncUDP::~CSyncUDP()
{
	Close();
}

void CSyncUDP::Close()
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

bool CSyncUDP::Init(const XString&strServerIP, int iPort)
{
	if (-1 != m_client)
	{
		return true;
	}
	m_client = socket(AF_INET, SOCK_DGRAM, 0);
	assert(-1 != m_client);
	if (-1 == m_client)
	{
		YouMe_LOG_Error(__XT("创建client socket 失败了"));
		return false;
	}
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(iPort);

	m_serverAddr.sin_addr.s_addr = inet_addr(XStringToUTF8(strServerIP).c_str());
#ifdef WIN32
	//设置非阻塞模式
	u_long mode = 1;
	ioctlsocket(m_client,FIONBIO,&mode);
#else
    int r = 1;
    #if (defined OS_OSX) || (defined OS_IOS) || (defined OS_IOSSIMULATOR)
        setsockopt(m_client, SOL_SOCKET, SO_NOSIGPIPE, &r, sizeof(int));
    #else
        setsockopt(m_client, SOL_SOCKET, MSG_NOSIGNAL, &r, sizeof(int));
    #endif
#endif
	return true;
}

int CSyncUDP::SendData(const char* buffer, int iBufferSize)
{
	return sendto(m_client, buffer,iBufferSize, 0, (const sockaddr*)&m_serverAddr, sizeof(sockaddr_in));
}


void CSyncUDP::UnInit()
{
	Close();
}

//select 500ms 一次
int CSyncUDP::RecvData(CXSharedArray<char>& recvBuffer,int iTimeout,CXCondWait* pCondiWait)
{
	sockaddr_storage addr;
	socklen_t addrSize = sizeof(addr);
	recvBuffer.Allocate(INTERNET_MTU);
    int iExt = iTimeout % 500;
    int iMaxTryCount = iTimeout / 500;
    if (iExt != 0) {
        iMaxTryCount++;
    }
    if (0 == iMaxTryCount)
    {
        iMaxTryCount = 1;
    }
    int iCurTryCount = 0;
    fd_set  read_set;
    while (iCurTryCount < iMaxTryCount)
    {
        if (NULL != pCondiWait)
        {
            if (WaitResult_Timeout != pCondiWait->WaitTime(0))
            {
                //收到通知了，需要退出接受
                break;
            }
        }
        struct timeval  tv;
        tv.tv_sec = 0;
        tv.tv_usec = 500*1000;
        FD_ZERO(&read_set);
        FD_SET(m_client, &read_set);
        //select 参数处理
        
        /*
         http://linux.die.net/man/2/select
         https://msdn.microsoft.com/en-us/library/windows/desktop/ms740141
         Return Value
         On success, select() and pselect() return the number of file descriptors contained in the three returned descriptor sets (that is, the total number of bits that are set in readfds, writefds, exceptfds) which may be zero if the timeout expires before anything interesting happens. On error, -1 is returned, and errno is set appropriately; the sets and timeout become undefined, so do not rely on their contents after an error.
         */
        int ret = select(m_client+1, &read_set, NULL, NULL, &tv);
        if (-1 == ret)
        {
            //socket 错误直接退出，可以通过关闭socket 来达到退出目的
            break;
        }
        if (0 == ret)
        {
            //超时了，continue一下
            iCurTryCount++;
            continue;
        }
        //其他情况表示已经触发了可用的条件
        if (FD_ISSET(m_client, &read_set)){
            return recvfrom(m_client, recvBuffer.Get(), INTERNET_MTU, MSG_DONTWAIT, (struct sockaddr *)&addr, &addrSize);
        }
    }
    return -1;
}

}
