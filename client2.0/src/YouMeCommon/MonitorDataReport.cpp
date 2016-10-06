#include "MonitorDataReport.h"
#include <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/Log.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/Log.h>
#include <YouMeCommon/XDNSParse.h>
#include <YouMeCommon/SyncTCP.h>
#include <YouMeCommon/pb/datareport.pb.h>
#include <YouMeCommon/SDKValidate.hpp>
namespace youmecommon
{
CMonitorDataReport::CMonitorDataReport()
{
	m_bInit = false;
	m_iMaxID = 1;
	m_iPort = -1;
}



CMonitorDataReport::~CMonitorDataReport()
{
}

bool CMonitorDataReport::Init(const XString& strDomainOrIP, int iPort, const XString& strLocalDbName, const XString& strDefalutIP)
{
	YouMe_LOG_Info(__XT("Enter"));
	if (m_bInit)
	{
		return m_bInit;
	}
	std::lock_guard<std::mutex> lock(m_mutex);
	m_strReportAddr = strDomainOrIP;
	m_strDefaultIP = strDefalutIP;
	m_iPort = iPort;
	//读取数据库
	XString strDBPath = CXFile::CombinePath(CYMApplication::GetInstance().GetDocumentDir(), strLocalDbName);
	m_sqliteDb.Open(strDBPath.c_str());

	for (int i = 0; i<sizeof(s_szYoumeCreateTableSQL) / sizeof(s_szYoumeCreateTableSQL[0]); i++)
	{
		if (!m_sqliteDb.IsTableExist(s_szYoumeTableName[i].c_str()))
		{
			CSqliteOperator	sqliteOperator(m_sqliteDb);
			sqliteOperator.PrepareSQL(s_szYoumeCreateTableSQL[i].c_str());
			sqliteOperator.Execute();
		}
	}

	//从数据库中读取所有的需要上报的日志
	CSqliteOperator sqliteOperator(m_sqliteDb);
	XString strSql = __XT("select * from report");
	sqliteOperator.PrepareSQL(strSql);
	sqliteOperator.Execute();
	while (sqliteOperator.Next())
	{
		int iTmpID = 0;
		CXSharedArray<char> buffer;
		sqliteOperator >> iTmpID >> buffer;
		if (iTmpID > m_iMaxID)
		{
			m_iMaxID = iTmpID;
		}
		m_needReportMap[iTmpID] = buffer;
		m_wait.Increment();
	}

	m_bInit = true;
	//无论是否有数据，先启动一次，用来DNS 解析
	m_wait.Increment();
	//启动一个线程用来数据上报
	m_reportThread = std::thread(&CMonitorDataReport::ReportProc, this);
	YouMe_LOG_Info(__XT("Leave"));
	return true;
}

void CMonitorDataReport::UnInit()
{
	YouMe_LOG_Info(__XT("Enter"));
	m_bInit = false;
	m_wait.Increment();
	if (m_reportThread.joinable())
	{
		m_reportThread.join();
	}
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_needReportMap.clear();
	}
	YouMe_LOG_Info(__XT("Leave"));
}

bool CMonitorDataReport::Report(int iEvtType, const std::vector<CXVariant>& reportData,XINT64 ulReportTime)
{
	YouMeCommonProtocol::DataReportRequest request;
	request.set_allocated_msghead(CSDKValidate::CreateRequestHead(iEvtType));
	request.set_reporttime(ulReportTime);
	for (size_t i = 0; i < reportData.size();i++)
	{
		YouMeCommonProtocol::NameValue* pContent = request.add_contents();
		pContent->set_name("");
		pContent->set_value(XStringToUTF8(reportData[i].ToString()));
		switch (reportData[i].m_vType)
		{
		case VariantType_String:
		{
			pContent->set_type(YouMeCommonProtocol::NAME_STRING);			
		}
			break;

		case VariantType_Bool:
		case VariantType_INT16:
		case VariantType_INT32:
		{
			pContent->set_type(YouMeCommonProtocol::NAME_INT32);
		}
			break;

		case VariantType_INT64:
			{
				pContent->set_type(YouMeCommonProtocol::NAME_INT64);
			}
			break;
			
		default:
			assert(false);
			break;
		}
	}

	std::string strSerialData;
	request.SerializeToString(&strSerialData);

	CXSharedArray<char> pPtr(strSerialData.length());
	memcpy(pPtr.Get(), strSerialData.c_str(), strSerialData.length());
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_iMaxID++;
		m_needReportMap[m_iMaxID] = pPtr;
		//插入到db
		CSqliteOperator	sqliteOperator(m_sqliteDb);
		XString strSql = __XT("insert into report values(?1,?2)");
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator << m_iMaxID;
		sqliteOperator << pPtr;
		sqliteOperator.Execute();
	}
	return true;
}

void CMonitorDataReport::ReportProc()
{
	YouMe_LOG_Info(__XT("Enter"));
	while (m_wait.Decrement())
	{
		if (!m_bInit)
		{
			//退出了
			YouMe_LOG_Info(__XT("上层反初始化了，退出吧"));
			break;
		}
		//先解析一下DNS 
		if (m_cachReportIP.size() == 0)
		{
			CXDNSParse::Instance()->ParseDomain2(m_strReportAddr, m_cachReportIP);
			if (m_cachReportIP.size() == 0)
			{
				m_cachReportIP.push_back(m_strDefaultIP);
			}
		}

		//取出一个来上报
		CXSharedArray<char> pReportBuffer;
		int iReportID = -1;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_needReportMap.empty())
			{
				continue;
			}
			//把取出来的复制到另一个缓存，然后从map 中删除
			std::map<int, CXSharedArray<char> >::iterator begin = m_needReportMap.begin();
			pReportBuffer = begin->second;
			iReportID = begin->first;
			m_needReportMap.erase(begin);
		}
		//如果是空的就不要上报
		if (pReportBuffer.GetBufferLen() == 0)
		{
			assert(false);
			continue;
		}
		//开始上报,如果多个ip的话只要有一个成功了就行
		for (size_t i = 0; i < m_cachReportIP.size(); i++)
		{
			CSyncTCP tcpReport;
			if (!tcpReport.Init(m_cachReportIP[i], m_iPort))
			{
				continue;
			}
			if (!tcpReport.Connect(3000))
			{
				YouMe_LOG_Info(__XT("连接数据上报服务器失败了:%s"),m_cachReportIP[i].c_str());
				continue;
			}
			//开始发送数据，TCP 的包头4字节为内容长度
			int iSendLen = tcpReport.SendData(pReportBuffer.Get(), pReportBuffer.GetBufferLen());
			if (iSendLen != pReportBuffer.GetBufferLen())
			{
				YouMe_LOG_Info(__XT("发送数据失败:%s"), m_cachReportIP[i].c_str());
				continue;
			}
			//接收服务器返回的数据
			CXSharedArray<char> recvBuf;
			int iRecvLen = tcpReport.RecvData(recvBuf);
			if (iRecvLen <= 0)
			{
				YouMe_LOG_Info(__XT("从服务器接收的数据长度错误了:%s"), m_cachReportIP[i].c_str());
				continue;
			}
			YouMeCommonProtocol::DataReportResponse response;
			if (!response.ParseFromArray(recvBuf.Get(),iRecvLen))
			{
				YouMe_LOG_Info(__XT("服务器返回的proto 数据解析失败:%s  %d"), m_cachReportIP[i].c_str(),iRecvLen);
				continue;
			}
			if (response.serverhead().code() == 0)
			{
				//上报成功，删掉数据库的
				//成功，从数据库删除记录
				std::lock_guard<std::mutex> lock(m_mutex);
				CSqliteOperator	sqliteOperator(m_sqliteDb);
				XString strSql = __XT("delete from report where id=?1");
				sqliteOperator.PrepareSQL(strSql);
				sqliteOperator << iReportID;
				sqliteOperator.Execute();
				break;
			}
		}
	}
	YouMe_LOG_Info(__XT("Leave"));
}
}
