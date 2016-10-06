#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <memory>
#include <YouMeCommon/XAny.h>
#include <YouMeCommon/XSharedArray.h>
namespace youmecommon {

enum VariantType
{
	VariantType_Unknow,
	VariantType_String,
	VariantType_Byte,
	VariantType_INT16,
	VariantType_INT32,
	VariantType_INT64,
	VariantType_Float,
	VariantType_Double,
	VariantType_Bool,
	VariantType_Array,
	VariantType_Dict,
	VariantType_Buffer,
};
class CXVariant
{
public:
	CXVariant(void);
	~CXVariant(void);
	void Clear();
	//各种构造函数
	CXVariant (const XCHAR* value);
	CXVariant (const XString& value);
	CXVariant(byte value);
	CXVariant(short value);
	CXVariant(int value);
	CXVariant(XINT64 value);
	CXVariant(float value);
	CXVariant(double value);
	
	//主要是为了避免 char 和 wchar 的混用，强制不能隐式转换成bool 类型
	explicit CXVariant(bool value);
	CXVariant(const std::map<std::string,CXVariant >& value);
	CXVariant(const std::vector<CXVariant >& value);
	void SetBuffer(const byte* value,int iSize);
	//针对变体的操作
	void Add( const std::string& key,const CXVariant& value);
	void Add( const CXVariant& value);

    //返回的数据里面各种int和int64 混合，为了方便提供一个函数
    XINT64 ToInt() const;
	XString ToString() const;
	//提供一些get 函数
	template <typename T>
	T Get() const
	{
        return ::youmecommon::CXAny::XAny_Cast<T>(m_any);
	}


	template <typename T>
	T* GetP() const
	{
        return ::youmecommon::CXAny::XAny_Cast<T>(&m_any);
	}

	//对string 做特化处理
	XString Get() const
	{
		std::shared_ptr<XString>* pPtr = ::youmecommon::CXAny::XAny_Cast<std::shared_ptr<XString> >(&m_any);
		if (NULL != pPtr)
		{
			return *(pPtr->get());
		}
		return __XT("");
	}

	

	//把自身的内容给对方，直接使用swap，否则可能会释放两次
	void Detach(CXVariant* value);
	//数据类型
	VariantType m_vType;
	//保留的变量，用来补充各种数据类型的，比如缓存的大小
	int m_iUnUsed;
	//使用any 来代表任意值
	::youmecommon::CXAny m_any;
};
}
