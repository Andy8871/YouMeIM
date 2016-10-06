#pragma once
#ifndef _SQLITE_QUERY_
#define _SQLITE_QUERY_
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <assert.h>
#include <string>
#if defined (WIN32) || defined(OS_ANDROID)
#include <YouMeCommon/Sqlite3/sqlite3.h>
#else
    #include <sqlite3.h>
#endif
#include "XSharedArray.h"
struct sqlite3_stmt;
struct sqlite3;
namespace youmecommon
{

class CSqliteDb
{
public:
	CSqliteDb(const XCHAR* dbPath );
	CSqliteDb();
	void Open(const XCHAR* dbPath);
	void Close();
	~CSqliteDb();

	operator const sqlite3*() const throw()
	{
		return _sqlite_db;
	}
	operator sqlite3*() throw()
	{
		return _sqlite_db;
	}
	sqlite3* GetDBPtr() const 
	{
		return _sqlite_db;
	}
	bool IsTableExist(const XString& tb_name);
	int _error_code;
	sqlite3* _sqlite_db;
};

class CSqliteOperator
{
public:

	CSqliteOperator();
	CSqliteOperator(CSqliteDb& sqlite_db);
	~CSqliteOperator(void);

    sqlite3_stmt* PrepareSQL(const XString& sql);

	CSqliteOperator& operator<<(unsigned char b);
	CSqliteOperator& operator<<(short s);
	CSqliteOperator& operator<<(int i);
	CSqliteOperator& operator<<(float f);
	CSqliteOperator& operator<<(double d);
	CSqliteOperator& operator<<(long long i64);
    CSqliteOperator& operator<<(CXSharedArray<char>& var);

	CSqliteOperator& operator<<(const XString& text);
	CSqliteOperator& operator>>(XString& text);
	CSqliteOperator& operator>>(unsigned char& b);
	CSqliteOperator& operator>>(short& s);
	CSqliteOperator& operator>>(int& i);
	CSqliteOperator& operator>>(float& f);
	CSqliteOperator& operator>>(double& d);
	CSqliteOperator& operator>>(long long& i64);
    CSqliteOperator& operator>>(CXSharedArray<char>& var);
    
	void Reset();
	bool Next();
	bool Execute();
	CSqliteDb* SqliteDbPtr() { return _sqlite_db_ptr; }
	
	
	int Errcode()  { return sqlite3_errcode(*_sqlite_db_ptr); }
	const char* Errmsg()  { return sqlite3_errmsg(*_sqlite_db_ptr); }

protected:
	void _Finalize();
	int  _Step();
private:
	int				_cur_bind;
	sqlite3_stmt*	_stmt_ptr;
	CSqliteDb*		_sqlite_db_ptr;
protected:
	
};

class AutoTransaction
{
public:
	AutoTransaction(sqlite3* db_ptr,bool exclusive = false);
	~AutoTransaction();
	void Commit();
private:
	bool m_commited;
	sqlite3* m_db_ptr;
};

class CSqliteMutex
{
	/**
	SQLITE_MUTEX_FAST
	SQLITE_MUTEX_RECURSIVE
	SQLITE_MUTEX_STATIC_MASTER
	SQLITE_MUTEX_STATIC_MEM
	SQLITE_MUTEX_STATIC_MEM2
	SQLITE_MUTEX_STATIC_PRNG
	SQLITE_MUTEX_STATIC_LRU
	SQLITE_MUTEX_STATIC_LRU2
	**/
public:
	CSqliteMutex(int mutex_type = SQLITE_MUTEX_FAST)
		: _mutex(0)
	{
		_mutex = sqlite3_mutex_alloc(mutex_type);
		assert(NULL != _mutex);
	}
	virtual ~CSqliteMutex()
	{
		if (_mutex)
			sqlite3_mutex_free(_mutex);
	}
	CSqliteMutex( const CSqliteMutex& mx );
	CSqliteMutex& operator=( const CSqliteMutex& mx );

	void lock()
	{
		if (_mutex)
			sqlite3_mutex_enter(_mutex);
	}
	void unlock()
	{
		if (_mutex)
			sqlite3_mutex_leave(_mutex);
	}

	operator sqlite3_mutex*() const
	{
		return _mutex;
	}
private:
	sqlite3_mutex*	_mutex;
};
}
#endif




