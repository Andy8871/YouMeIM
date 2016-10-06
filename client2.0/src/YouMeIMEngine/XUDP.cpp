#include "XUDP.h"
#include <assert.h>
#include <time.h>
#include <YouMeCommon/Log.h>
#include <YouMeIMEngine/YouMeIMManager.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <YouMeCommon/TimeUtil.h>

CXUDP::CXUDP(YouMeIMManager* pIManager)
{
	m_bInit = false;
	m_bExitSend = false;
	m_bRecvThreadExit = false;
	m_pIManager = pIManager;
}
CXUDP::~CXUDP()
{
    YouMe_LOG_Info(__XT("Enter"));
	UnInit();
    YouMe_LOG_Info(__XT("Leave"));
}


void CXUDP::UDPRecvThread()
{
	YouMe_LOG_Info(__XT("Enter"));
	fd_set  read_set;
	while (true)
	{
		//类unix 系统中，第一个参数必须是client+1
		if (m_bExitSend)
		{
			break;
		}	
		struct timeval  tv;
		tv.tv_sec = 0;
		tv.tv_usec = 200 * 1000;

		FD_ZERO(&read_set);
		FD_SET(m_client.RawSocket(), &read_set);
		//select 参数处理


		// http://linux.die.net/man/2/select
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms740141
		// Return Value
		// On success, select() and pselect() return the number of file descriptors contained in the three returned descriptor sets (that is, the total number of bits that are set in readfds, writefds, exceptfds) which may be zero if the timeout expires before anything interesting happens. On error, -1 is returned, and errno is set appropriately; the sets and timeout become undefined, so do not rely on their contents after an error.

		int ret = select(m_client.RawSocket() + 1, &read_set,NULL, NULL, &tv);
		if (-1 == ret)
		{
			//socket 错误直接退出，可以通过关闭socket 来达到退出目的
			break;
		}
		//判断一下有没有因为服务器没有回业务包超时的
		{
			XINT64 nowUnixTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
			std::lock_guard<std::mutex> lock(m_mutex);
			std::map<XINT64, UDPTCPPacketInfo>::iterator begin = m_sendPacketMap.begin();
			for (; begin != m_sendPacketMap.end();)
			{
				if (begin->second.ulSendTime == 0)
				{
					begin++;
					continue;//还没有发送
				}
				//判断是否已经发送并且超时
				if ((nowUnixTime - begin->second.ulSendTime) >= begin->second.ulTimeout)
				{
					//超时了
					if (NULL != m_pPacketCallback)
					{
						m_pPacketCallback->OnRecvPacketTimeOut(begin->first);
					}
					begin = m_sendPacketMap.erase(begin);
				}
				else
				{
					begin++;
				}
			}
		}
		if (0 == ret)
		{
			//超时了，continue一下
			continue;
		}
		//其他情况表示已经触发了可用的条件
		if (FD_ISSET(m_client.RawSocket(), &read_set))
		{
			if (!DealRead())
            {
				//通知一遍所有的消息，超时，并且清空
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					std::map<XINT64, UDPTCPPacketInfo>::iterator begin = m_sendPacketMap.begin();
					for (; begin != m_sendPacketMap.end(); begin++)
					{
						//超时了
						if (NULL != m_pPacketCallback)
						{
							m_pPacketCallback->OnRecvPacketTimeOut(begin->first);
						}
					}
					m_sendPacketMap.clear();
				}
				if (!m_bExitSend)
				{
					//启动一次重连
					YouMe_LOG_Info(__XT("tcp recv 失败了，触发重连"));
					m_pIManager->ReconectProc();
				}
                YouMe_LOG_Info(__XT("接收数据失败了，退出接受线程"));
                break;
            }
		}
	
	}
	m_bRecvThreadExit = true;
	YouMe_LOG_Info(__XT("Leave"));
}

//需要判断wifi 和 3G 网卡，绑定ip 地址优先wifi
bool CXUDP::Init(const XString&strServerIP, int iPort, IPacketCallback* pCallback)
{
	YouMe_LOG_Info(__XT("Enter m_binit:%d"),(int)m_bInit);
	if (m_bInit)
	{
		return m_bInit;
	}
	m_pPacketCallback = pCallback;
	m_bExitSend = false;
	//启动一个线程用来发送，发送的第一次用来创建socket
	m_client.Init(strServerIP, iPort);
	m_sendThread = std::thread(&CXUDP::SendRetryThread, this);  
	m_bInit = true;
	YouMe_LOG_Info(__XT("Leave"));
	return true;
}

bool CXUDP::SendData(YouMeIMProtocol::Command_TYPE commandType, const char* buffer, size_t iBufferSize, XINT64 iUniqueSerial, XINT64 ulTimeout)
{
	if (m_bRecvThreadExit)
	{
		YouMe_LOG_Warning(__XT("接收数据线程已经退出了但是消息来了:%d"),commandType);
		return false;
	}
	//把数据加入到发送队列，需要校验数据是否发送成功	
	UDPTCPPacketInfo info;
	info.commandType = commandType;
	info.pPacket = youmecommon::CXSharedArray<char>((int)iBufferSize+5);
	memcpy(info.pPacket.Get()+5, buffer, iBufferSize);
	//清空前5个字节
	memset(info.pPacket.Get(), 0, 5);
	info.ulTimeout = ulTimeout;
	if (commandType != YouMeIMProtocol::Command_Login)
	{
		//不是登陆的都需要加密
		info.pPacket.Get()[0] = (char)1; //表示要加密
		//填入序号
		memcpy(info.pPacket.Get() + 1, m_sessionSerial.Get(), m_sessionSerial.GetBufferLen());
		//加密数据
		m_pIManager->EncryDataXOR(info.pPacket.Get() + 5, (int)iBufferSize);
	}

	//先加入到 map 中，然后发送
	 {
		 std::lock_guard<std::mutex> lock(m_mutex);
		 m_sendPacketMap[iUniqueSerial] = info;
	 }
	m_condWait.Increment();
	return true;
}

bool CXUDP::DealRead()
{
    /*
     http://linux.die.net/man/2/recvfrom
     All three routines return the length of the message on successful completion. If a message is too long to fit in the supplied buffer, excess bytes may be discarded depending on the type of socket the message is received from
     */
	youmecommon::CXSharedArray<char> recvBuffer;
	int iRecvLen = m_client.RecvData(recvBuffer);
	if (iRecvLen <= 0)
	{
#ifdef WIN32
		int iErrorcode = WSAGetLastError();
#else
		int iErrorcode = errno;
#endif
		
		YouMe_LOG_Error(__XT("读取tcp 包失败了：%d errcode:%d"), iRecvLen, iErrorcode);
		return false;
	}
	//先判断数据是否已经加密过了
	if (0 != recvBuffer.Get()[0])
	{
		//已经加密了得，需要解密一下
		m_pIManager->EncryDataXOR(recvBuffer.Get() + 5, iRecvLen - 5);
	}
	//使用proto buf 解析数据
	YouMeIMProtocol::ServerReqHead head;
	if (!head.ParseFromArray(recvBuffer.Get() + 5, iRecvLen - 5))
	{
		assert(false);
		YouMe_LOG_Error(__XT("解析Proto 头失败了"));
		//包还是收到了，只是解析不对，返回成功
		return true;
	}
	
	XINT64 ulPackSerial = head.head().msgserialnum();
	if (m_recvPackSerial.find(ulPackSerial) != m_recvPackSerial.end())
	{
		YouMe_LOG_Warning(__XT("收到了相同序号的包，服务器序号:%lld type:%d client:%lld"), head.head().msgserialnum(), head.head().msgtype(), head.head().clientserialnum());
		return true;
	}
	m_recvPackSerial.insert(ulPackSerial);
	if (head.head().msgtype() == YouMeIMProtocol::Command_LoginResponse)
	{
		//读取后面4字节
		m_sessionSerial.Allocate(4);
		//拷贝4字节的序号
		memcpy(m_sessionSerial.Get(), recvBuffer.Get() + 1, 4);
		//尼玛的耦合好严重啊
		YouMeIMProtocol::YouMeIM_Command_LoginResponse loginResponse;
		m_pIManager->ParseRSASecret(recvBuffer.Get() + 5, iRecvLen - 5, loginResponse);
	}
		
	{
		//判断业务包是否是需要确认的，然后删掉
		std::lock_guard<std::mutex> lock(m_mutex);
		std::map<XINT64, UDPTCPPacketInfo>::iterator it = m_sendPacketMap.find(head.head().clientserialnum());
		if (it != m_sendPacketMap.end())
		{
			m_sendPacketMap.erase(it);
		}
	}
	//回调给上层
	if (NULL != m_pPacketCallback)
	{
		m_pPacketCallback->OnRecvPacket(head, (const byte*)recvBuffer.Get() + 5, iRecvLen - 5);
	}
	return true;
}

void CXUDP::UnInit()
{
	YouMe_LOG_Info(__XT("Enter m_bInit:%d"),(int)m_bInit);
	if (!m_bInit)
	{
		return;
	}
	m_bInit = false;
	m_bExitSend = true;
	m_condWait.Increment();
	m_pPacketCallback = NULL;
	
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_sendPacketMap.clear();
	}
	
	if (m_sendThread.joinable())
	{
		m_sendThread.join();
	}
	YouMe_LOG_Info(__XT("发送线程退出"));
	if (m_recvThread.joinable())
	{
		m_recvThread.join();
	}
	m_client.UnInit();
	YouMe_LOG_Info(__XT("Leave"));
}



void CXUDP::SendRetryThread()
{	
	YouMe_LOG_Info(__XT("Enter"));
	if ( !m_client.Connect(5000))
	{
		YouMe_LOG_Error(__XT("连接服务器失败了"));
		return;
	}
	YouMe_LOG_Info(__XT("启动线程用来接收"));
	m_recvThread = std::thread(&CXUDP::UDPRecvThread, this);
	fd_set  send_set;
	while (true)
	{
		//类unix 系统中，第一个参数必须是client+1
		if (m_bExitSend)
		{
			break;
		}
		struct timeval  tv;
		tv.tv_sec = 0;
		tv.tv_usec = 200 * 1000;

		FD_ZERO(&send_set);
		FD_SET(m_client.RawSocket(), &send_set);
		//select 参数处理


		// http://linux.die.net/man/2/select
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms740141
		// Return Value
		// On success, select() and pselect() return the number of file descriptors contained in the three returned descriptor sets (that is, the total number of bits that are set in readfds, writefds, exceptfds) which may be zero if the timeout expires before anything interesting happens. On error, -1 is returned, and errno is set appropriately; the sets and timeout become undefined, so do not rely on their contents after an error.

		int ret = select(m_client.RawSocket() + 1,NULL, &send_set, NULL, &tv);
		if (-1 == ret)
		{
			//socket 错误直接退出，可以通过关闭socket 来达到退出目的
			break;
		}
		
		if (0 == ret)
		{
			//超时了，continue一下
			continue;
		}
		if (FD_ISSET(m_client.RawSocket(), &send_set))
		{
			DealSend();
		}
	}
	YouMe_LOG_Info(__XT("Leave"));
}

bool CXUDP::DealSend()
{
	//等待1000 毫秒，当然，如果有需要发送的数据，上层会直接给个信号过来，然后就立即处理发送请求
	m_condWait.Decrement();
	if (m_bExitSend)
	{
		return true;
	}

	XINT64 nowUnixTime = youmecommon::CTimeUtil::GetTimeOfDay_MS();
	youmecommon::CXSharedArray<char> pPacket;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		std::map<XINT64, UDPTCPPacketInfo >::iterator begin = m_sendPacketMap.begin();
		for (; begin != m_sendPacketMap.end();begin++)
		{
			if (begin->second.ulSendTime == 0)
			{
				begin->second.ulSendTime = nowUnixTime;
				pPacket = begin->second.pPacket;
				break;
			}
		}
	}
	if (pPacket.Get() == NULL)
	{
		return false;
	}

	m_client.SendData((const char*)pPacket.Get(), pPacket.GetBufferLen());
	return true;
}
