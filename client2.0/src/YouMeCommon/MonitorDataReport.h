#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeCommon/XVariant.h>
#include <YouMeCommon/SqliteOperator.h>
#include <YouMeCommon/XSemaphore.h>
#include <mutex>
#include <thread>
#include <map>

/************************************************************************/
/*    保存了上报的东西                                                                     */
/************************************************************************/
namespace youmecommon
{
const XString s_szYoumeTableName[] = {
	__XT("report")
}; //一对多的配置



const XString s_szYoumeCreateTableSQL[] = {
	__XT("create table report (id int,value  blob);")
};
//数据上报一半都是一个应用程序只需要需要一个，为了避免不必要的多次初始化或者反初始化，建议new 一个全局的来用
class CMonitorDataReport
{
public:
	CMonitorDataReport();
	~CMonitorDataReport();

	//初始化和反初始化, 参数三只需要给一个db 的文件名，不需要全路径
	bool Init(const XString& strDomainOrIP, int iPort, const XString& strLocalDbName, const XString& strDefalutIP);
	void UnInit();
	//开始上报，上报统一protobuf 协议的二进制数据
	bool Report(int iEvtType, const std::vector<CXVariant>& reportData, XINT64 ulReportTime);
	
private:
	//上报线程
	void ReportProc();
	bool m_bInit;
	//服务器相关参数
	XString m_strDefaultIP;
	XString m_strReportAddr;
	int m_iPort;

	//上报线程
	CXSemaphore m_wait;
	std::mutex m_mutex;
	std::thread m_reportThread;

	//本地db
	CSqliteDb m_sqliteDb;
	int m_iMaxID;
	//保存从数据库读出的数据
	std::map<int, CXSharedArray<char> >m_needReportMap;
	//用来保存域名DNS 解析的ip或者默认的ip
	std::vector<XString> m_cachReportIP;
};
}
