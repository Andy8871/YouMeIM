#include "./SqliteOperator.h"
namespace youmecommon
{
//////////////////////////////////////////////////////////////////////////
CSqliteOperator::CSqliteOperator(CSqliteDb& sqlite_db )
	: _cur_bind(0),_stmt_ptr(0),_sqlite_db_ptr(0)
{
	_sqlite_db_ptr = &sqlite_db;
}

CSqliteOperator::CSqliteOperator()
	: _cur_bind(0),_stmt_ptr(0),_sqlite_db_ptr(0)
{
}

CSqliteOperator::~CSqliteOperator(void)
{
	_Finalize();
}

void CSqliteOperator::Reset()
{
	_cur_bind = 0;
	if(_stmt_ptr)
		sqlite3_reset(_stmt_ptr);
}

bool CSqliteOperator::Next()
{
	return SQLITE_ROW == _Step();
}

void CSqliteOperator::_Finalize()
{
	_cur_bind  = 0;
	if(_stmt_ptr)
	{
		int res = sqlite3_finalize(_stmt_ptr);
		assert(res == SQLITE_OK);
		_stmt_ptr = NULL;
	}
}
CSqliteOperator& CSqliteOperator::operator<<(CXSharedArray<char>& var)
{
    if(_stmt_ptr)
    {
        sqlite3_bind_blob(_stmt_ptr,++_cur_bind,var.Get(), var.GetBufferLen(),SQLITE_TRANSIENT);
    }
    return *this;
}
CSqliteOperator& CSqliteOperator::operator<<( int i )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_int(_stmt_ptr,++_cur_bind,i);
	}
	return *this;
}
CSqliteOperator& CSqliteOperator::operator<<( unsigned char b )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_int(_stmt_ptr,++_cur_bind,b);
	}
	return *this;
}
CSqliteOperator& CSqliteOperator::operator<<( short b )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_int(_stmt_ptr,++_cur_bind,b);
	}
	return *this;
}
CSqliteOperator& CSqliteOperator::operator<<( float f )
{
	double d = f;
	*this<<d;
	return *this;
}

CSqliteOperator& CSqliteOperator::operator<<( double d )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_double(_stmt_ptr,++_cur_bind,d);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator<<( long long i64 )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_int64(_stmt_ptr,++_cur_bind,i64);
	}
	return *this;
}


CSqliteOperator& CSqliteOperator::operator<<(const XString& text)
{
	if(_stmt_ptr)
	{
#ifdef WIN32
	sqlite3_bind_text16(_stmt_ptr,++_cur_bind,text.c_str(),(int)text.length() * sizeof(wchar_t),SQLITE_TRANSIENT);
#else
	sqlite3_bind_text(_stmt_ptr,++_cur_bind,text.c_str(),(int)text.length(),SQLITE_TRANSIENT);
#endif
		
	}
	return *this;
}


CSqliteOperator& CSqliteOperator::operator>>(CXSharedArray<char>& var)
{
    if(_stmt_ptr)
    {
        int data_length = sqlite3_column_bytes(_stmt_ptr,_cur_bind);
        var.Allocate(data_length);
        memcpy(var.Get(), sqlite3_column_blob(_stmt_ptr,_cur_bind++), data_length);
    }
    return *this;
}


CSqliteOperator& CSqliteOperator::operator>>(int& i)
{
	if(_stmt_ptr)
	{
		i = sqlite3_column_int(_stmt_ptr,_cur_bind++);
	}
	return *this;
}
CSqliteOperator& CSqliteOperator::operator>>(unsigned char& b)
{
	if(_stmt_ptr)
	{
		b = sqlite3_column_int(_stmt_ptr,_cur_bind++);
	}
	return *this;
}
CSqliteOperator& CSqliteOperator::operator>>(short& b)
{
	if(_stmt_ptr)
	{
		b = sqlite3_column_int(_stmt_ptr,_cur_bind++);
	}
	return *this;
}
CSqliteOperator& CSqliteOperator::operator>>(float& f)
{
	if(_stmt_ptr)
	{
		double d;
		d = sqlite3_column_double(_stmt_ptr,_cur_bind++);
		f = (float)d;
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(double& d)
{
	if(_stmt_ptr)
	{
		d = sqlite3_column_double(_stmt_ptr,_cur_bind++);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(long long& i64)
{
	if(_stmt_ptr)
	{
		i64 = sqlite3_column_int64(_stmt_ptr,_cur_bind++);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(XString& text)
{
	if (_stmt_ptr)
	{
		XCHAR* t = NULL;
#ifdef WIN32
		t = (XCHAR*)sqlite3_column_text16(_stmt_ptr, _cur_bind++);
#else
		t = (XCHAR*)sqlite3_column_text(_stmt_ptr, _cur_bind++);
#endif	
		if (t)
		{
			text.assign(t);
		}
	}
	return *this;
}


//CSqliteOperator& CSqliteOperator::operator>>(CVariant& blob)
//{
//	if(_stmt_ptr)
//	{
//		int data_length = sqlite3_column_bytes(_stmt_ptr,_cur_bind);
//		blob.SetBuffer((byte*)sqlite3_column_blob(_stmt_ptr,_cur_bind++),data_length);
//	}
//	return *this;
//}

sqlite3_stmt* CSqliteOperator::PrepareSQL( const XString& sql)
{
	_Finalize();//Reset();
	sqlite3_stmt* statement = NULL;
	if(_sqlite_db_ptr)
	{
		#ifdef WIN32
			sqlite3_prepare16_v2(static_cast<sqlite3*>(*_sqlite_db_ptr),sql.c_str(),(int)sql.length() * 2, &statement,NULL);
		#else
			sqlite3_prepare_v2(static_cast<sqlite3*>(*_sqlite_db_ptr),sql.c_str(),(int)sql.length(), &statement,NULL);
		#endif
	}
	_stmt_ptr = statement;
	return statement;
}

bool CSqliteOperator::Execute()
{
	int r = _Step();
	if (r == SQLITE_BUSY)
	{
		r = _Step();//考虑再给一次执行机会
	}
	//ATLASSERT(r == SQLITE_DONE);
	Reset();
	return r == SQLITE_DONE;
}

int CSqliteOperator::_Step()
{
	if( !_stmt_ptr )
		return SQLITE_ERROR;
	_cur_bind = 0;
	return  sqlite3_step(_stmt_ptr);
}

//////////////////////////////////////////////////////////////////////////
CSqliteDb::CSqliteDb()
{
	_sqlite_db = NULL;
	_error_code = 0;
}

void CSqliteDb::Open(const XCHAR* dbPath)
{
#ifdef WIN32
	_error_code = sqlite3_open16(dbPath, &_sqlite_db);
#else
	_error_code = sqlite3_open(dbPath, &_sqlite_db);
#endif
}

void CSqliteDb::Close()
{
	if (NULL != _sqlite_db)
	{
		sqlite3_close(_sqlite_db);
		_sqlite_db = NULL;
	}
}

CSqliteDb::CSqliteDb( const XCHAR* dbPath )
{
	_error_code = 0;
#ifdef WIN32
	_error_code = sqlite3_open16(dbPath, &_sqlite_db);
#else
	_error_code = sqlite3_open(dbPath, &_sqlite_db);
#endif
}


CSqliteDb::~CSqliteDb()
{
	Close();	
}

bool CSqliteDb::IsTableExist(const XString& tb_name)
{
	CSqliteOperator query(*this);
	query.PrepareSQL(__XT("select tbl_name from sqlite_master where tbl_name = ? "));
	query << tb_name;
	return query.Next();
}
//////////////////////////////////////////////////////////////////////////

AutoTransaction::AutoTransaction(sqlite3* db_ptr,bool exclusive)
	: m_commited(false), m_db_ptr(db_ptr)
{
	std::string  exclusive_term = exclusive ? "EXCLUSIVE " : "";
	std::string  query = "BEGIN " + exclusive_term + " TRANSACTION;";
	int exe_res;
    do 
	{ 
		exe_res = sqlite3_exec((m_db_ptr), query.c_str(), 0, 0, 0); 
	}while (exe_res == SQLITE_BUSY);

    if (SQLITE_OK != exe_res)
    {
        m_commited = true;
    }

}

AutoTransaction::~AutoTransaction()
{
	if (!m_commited)
        sqlite3_exec((m_db_ptr), "ROLLBACK TRANSACTION;", 0, 0, 0);

}

void AutoTransaction::Commit()
{
	if (!m_commited)
    {
        for (int exe_res;;)
        {
            exe_res = sqlite3_exec((m_db_ptr), "END TRANSACTION;", 0, 0, 0);//or "COMMIT TRANSACTION"
            if (exe_res != SQLITE_BUSY)
                break;
		}
		m_commited = true;
	}
}
}
