#include "XVariant.h"
#include <YouMeCommon/StringUtil.hpp>
namespace youmecommon {

CXVariant::CXVariant(void)
{
	m_vType = VariantType_Unknow;
	m_iUnUsed = 0;
}

CXVariant::CXVariant( const XCHAR* value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_String;
	m_any = std::shared_ptr<XString>(new XString(value));
}

CXVariant::CXVariant( byte value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_Byte;
	m_any = value;
}

CXVariant::CXVariant( short value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_INT16;
	m_any = value;
}

CXVariant::CXVariant( int value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_INT32;
	m_any = value;
}

CXVariant::CXVariant( XINT64 value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_INT64;
	m_any = value;
}

CXVariant::CXVariant( float value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_Float;
	m_any = value;
}

CXVariant::CXVariant( double value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_Double;
	m_any = value;
}

CXVariant::CXVariant( bool value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_Bool;
	m_any = value;
}

CXVariant::CXVariant(const std::map<std::string,CXVariant >& value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_Dict;
	m_any = value;
}

CXVariant::CXVariant( const std::vector<CXVariant >& value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_Array;
	m_any = value;
}

CXVariant::CXVariant( const XString& value )
{
	m_iUnUsed = 0;
	m_vType = VariantType_String;
	m_any = std::shared_ptr<XString>(new XString(value));
}

CXVariant::~CXVariant(void)
{
	Clear();
}


void CXVariant::Clear()
{
	if (m_vType == VariantType_Unknow)
	{
		return;
	}
    youmecommon::CXAny().swap(m_any);
	m_vType = VariantType_Unknow;
}

void CXVariant::SetBuffer( const byte* value,int iSize )
{
	Clear();
	m_iUnUsed = iSize;
	m_vType = VariantType_Buffer;
	CXSharedArray<byte> pPtr = CXSharedArray<byte>(iSize);
	memcpy(pPtr.Get(),value,iSize);
	m_any = pPtr;
}

void CXVariant::Detach( CXVariant* value )
{
	value->Clear();
	//直接使用内存拷贝
	m_any.swap(value->m_any);
	value->m_vType = m_vType;
	value->m_iUnUsed = m_iUnUsed;
	m_vType = VariantType_Unknow;
}

void CXVariant::Add( const std::string& key,const CXVariant & value )
{
	if (value.m_vType ==  VariantType_Unknow)
	{
		return ;
	}
	if ((m_vType != VariantType_Dict) && (m_vType != VariantType_Unknow))
	{
		assert(false);
		return ;
	}
	if (m_vType == VariantType_Unknow)
	{
		m_vType = VariantType_Dict;
		m_any = std::map<std::string,CXVariant>();
	}
	std::map<std::string, CXVariant>*pPtr = youmecommon::CXAny::XAny_Cast<std::map<std::string, CXVariant> >(&m_any);
	pPtr->insert(std::make_pair(key,value));
}

void CXVariant::Add( const CXVariant& value )
{
	if (value.m_vType ==  VariantType_Unknow)
	{
		return ;
	}
	//添加一个value，只能是数组类型。
	if ((m_vType != VariantType_Array) && (m_vType != VariantType_Unknow))
	{
		assert(false);
		return ;
	}
	if (m_vType == VariantType_Unknow)
	{
		m_vType = VariantType_Array;
		m_any = std::vector<CXVariant>();
	}
	std::vector<CXVariant> *pPtr = youmecommon::CXAny::XAny_Cast<std::vector<CXVariant> >(&m_any);
	pPtr->push_back(value);
}
XINT64 CXVariant::ToInt()const
{
    switch (m_vType)
    {
        case VariantType_INT16:
        {
            return Get<short>();
        }
        case VariantType_INT32:
        {
           return Get<int>();
        }
        case VariantType_INT64:
        {
            return Get<XINT64>();
        }
		case  VariantType_Bool:
		{
			return Get<bool>();
		}
        case VariantType_Byte:
        {
            return Get<byte>();
        }
        default:
        {
            assert(0);
        }
        return 0;
    }
}

XString CXVariant::ToString() const
{
	switch (m_vType)
	{
		case VariantType_INT16:
		{
			return CStringUtil::to_string(Get<short>());
		}
		case VariantType_INT32:
		{
			return CStringUtil::to_string(Get<int>());
		}
		case VariantType_INT64:
		{
			return CStringUtil::to_string(Get<XINT64>());
		}
		case  VariantType_Bool:
		{
			return CStringUtil::to_string(Get<bool>());
		}
		case VariantType_String:
		{
			return Get();
		}
		default:
		{
			assert(0);
		}
	}
	return __XT("");
}
}