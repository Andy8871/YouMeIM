#include "profiledb.h"
#include <Sqlite3/sqlite3.h>
#include <XFile.h>
#include <YMApplication.hpp>
CProfileDB::CProfileDB()
{
	//读取数据库
	XString strDBPath = youmecommon::CXFile::CombinePath(CYMApplication::GetInstance().GetDocumentDir(), __XT("youmeprofile.db"));
	m_sqliteDb.Open(strDBPath.c_str());

	for (int i = 0; i < sizeof(szProfileCreateTableSQL) / sizeof(szProfileCreateTableSQL[0]); i++)
	{
		if (!m_sqliteDb.IsTableExist(s_szProfileTableName[i].c_str()))
		{
			youmecommon::CSqliteOperator	sqliteOperator(m_sqliteDb);
			sqliteOperator.PrepareSQL(szProfileCreateTableSQL[i].c_str());
			sqliteOperator.Execute();
		}
	}
}

CProfileDB& CProfileDB::Instance()
{
	static CProfileDB s_CProfileDB;
	return s_CProfileDB;
}

void CProfileDB::setSetting(const std::string& strKey, const XString& strValue)
{
	XString strNowValue;
	bool bExist = getSetting(strKey, strNowValue);
	youmecommon::CSqliteOperator	sqliteOperator(m_sqliteDb);
	if (!bExist)
	{
		//插入
		XString strSql = __XT("insert into settings values(?1,?2)");
		sqliteOperator.PrepareSQL(strSql);
		sqliteOperator << UTF8TOXString(strKey);
		sqliteOperator << strValue;
	}
	else
	{
		XString strSql = __XT("update settings set value=?1 where key=?2");
		sqliteOperator.PrepareSQL(strSql);		
		sqliteOperator << strValue;
		sqliteOperator << UTF8TOXString(strKey);
	}
	sqliteOperator.Execute();
}

bool CProfileDB::getSetting(const std::string& strKey,XString& strValue)
{
	youmecommon::CSqliteOperator sqliteOperator(m_sqliteDb);
	XString strSql = __XT("select value from settings where key=?1");
	sqliteOperator.PrepareSQL(strSql);
	sqliteOperator << UTF8TOXString(strKey);
	sqliteOperator.Execute();
	while (sqliteOperator.Next())
	{
		sqliteOperator >> strValue;
		return true;
	}

	return false;
}
