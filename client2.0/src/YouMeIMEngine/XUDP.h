#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeIMCommonDef.h>
//proto 协议
#include <YouMeIMEngine/pb/im_common.pb.h>
#include <YouMeIMEngine/pb/im_contact.pb.h>
#include <YouMeIMEngine/pb/im_group.pb.h>
#include <YouMeIMEngine/pb/im_message.pb.h>
#include <YouMeCommon/SyncTCP.h>
#include <mutex>
#include <YouMeCommon/XSemaphore.h>
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


#include <thread>
class YouMeIMManager;
//内部会重试，超时，确认等等
class CXUDP
{
public:
	CXUDP(YouMeIMManager* pIManager);
	~CXUDP();
	//设置接收服务器参数，
	bool Init(const XString&strServerIP, int iPort, IPacketCallback* pCallback);
	void UnInit();

	//仅仅是放进了发送队列。iUniqueSerial 外部调用者 通过静态函数生成
	bool SendData(YouMeIMProtocol::Command_TYPE commandType, const char* buffer, size_t iBufferSize, XINT64 iUniqueSerial, XINT64 ulTimeout = 10000);
		
private:
	//发包线程函数
	void SendRetryThread();
	std::thread m_sendThread;
	//接收包线程
	void UDPRecvThread();
	std::thread m_recvThread;
	bool m_bRecvThreadExit;

	//用来处理socket 可读，可写的函数
	bool DealRead();
	bool DealSend();
private:
	bool m_bInit;
	//一个用来发送，一个用来接收的socket
	youmecommon::CSyncTCP m_client;

	//用来保存发送的包
	std::map<XINT64, UDPTCPPacketInfo >m_sendPacketMap;
	std::mutex m_mutex;

	
	youmecommon::CXSemaphore m_condWait;
	bool m_bExitSend;
	
	IPacketCallback* m_pPacketCallback;

	//IM管理器
	YouMeIMManager* m_pIManager;
	//和服务器约定的4个字节的序号
	youmecommon::CXSharedArray<char> m_sessionSerial;

	//一个集合用来保存所有收到包的序号，以防止重复
	std::set<XINT64> m_recvPackSerial;

};