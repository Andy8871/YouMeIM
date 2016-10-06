#pragma once
#include <assert.h>
#include <string>
#include <typeinfo>
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeCommon/Log.h>
namespace youmecommon {

class CXAny
{
public:
	CXAny()
		: content(0)
	{
	}
	//带参数的模板构造，该死的为了自适应参数类型
	template<typename ValueType>
	CXAny(const ValueType & value)
		: content(new holder<ValueType>(value))
	{
	}

	CXAny(const CXAny & other)
		: content(other.content ? other.content->clone() : 0)
	{
	}

	~CXAny()
	{
		delete content;
	}
public:

	template<typename ValueType>
	static inline ValueType XAny_Cast(const CXAny & operand)
	{
		if (operand.type() != typeid(ValueType))
		{
            YouMe_LOG_Warning(__XT("anycast 类型不一致"));
            assert(false);
            return ValueType();
		}
		return static_cast<CXAny::holder<ValueType> *>(operand.content)->held;
	}

	template<typename ValueType>
	static inline ValueType * XAny_Cast(const CXAny * operand)
	{
		if (NULL == operand)
		{
			return NULL;
		}
		if (operand->type() != typeid(ValueType))
		{
            YouMe_LOG_Warning(__XT("anycast 类型不一致"));
			return NULL;
		}
		return &static_cast<CXAny::holder<ValueType> *>(operand->content)->held;
	}
	//提供一些get 函数
	template <typename T>
	T Get()
	{
		return XAny_Cast<T>(*this);
	}


	template <typename T>
	T* GetP()
	{
		return XAny_Cast<T>(this);
	}

	//对string 做特化处理
    XString Get()
	{
		XString * pPtr = GetP<XString>();
		if (NULL != pPtr)
		{
			return *pPtr;
		}
		return __XT("");
	}
	//判断是否是一个空类型
	bool empty() const
	{
		return !content;
	}
	//获取数据的类型
	const std::type_info & type() const
	{
		return content ? content->type() : typeid(void);
	}

	CXAny & swap(CXAny & rhs)
	{
		std::swap(content, rhs.content);
		return *this;
	}

	template<typename ValueType>
	CXAny & operator=(const ValueType & rhs)
	{
		CXAny(rhs).swap(*this);
		return *this;
	}

	CXAny & operator=(CXAny rhs)
	{
		rhs.swap(*this);
		return *this;
	}

	//下面这段是boost 中的代码，
	class placeholder
	{
	public: // structors

		virtual ~placeholder()
		{
		}

	public: // queries
		virtual const std::type_info & type() const = 0;
		virtual placeholder * clone() const = 0;
	};

	template<typename ValueType>
	class holder : public placeholder
	{
	public: // structors

		holder(const ValueType & value)
			: held(value)
		{
		}

		holder(ValueType&& value)
			: held(static_cast<ValueType&&>(value))
		{
		}

	public: // queries

		virtual const std::type_info & type() const
		{
			return typeid(ValueType);
		}

		virtual placeholder * clone() const
		{
			return new holder(held);
		}

	public: // representation

		ValueType held;

	private: // intentionally left unimplemented
		holder & operator=(const holder &);
	};
public:
	placeholder * content;
};
}
