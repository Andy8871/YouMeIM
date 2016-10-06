
/**************************** 条件编译选项和头文件 ****************************/
#ifdef WIN32

#pragma warning(disable:4267)
#include <tchar.h>

#endif // end of WIN32

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "StringUtil.hpp"


/********************************** 宏、常量 **********************************/

/********************************** 数据类型 **********************************/
#if defined(ANDROID) || defined(__ANDROID__)
int wcscasecmp(const wchar_t *s1, const wchar_t *s2)
{
    while (towlower(*s1) == towlower(*s2))
    {
        if (0 == *s1)
        {
            return 0;
        }
        s1++;
        s2++;
    }
    
    return(towlower(*s1) - towlower(*s2));
}
#endif
template<>
std::string CStringUtilT<char>::to_string(unsigned int i)
{
    char out[16];
#if defined(WIN32)
    _ultoa(i, out, 10);
#else
    sprintf(out, "%u", i);
#endif
    return out;
}

template<>
std::wstring CStringUtilT<wchar_t>::to_string(unsigned int i)
{
    wchar_t out[16];
#if defined(WIN32)
    _ultow(i, out, 10);
#else
    swprintf(out, sizeof(out) / sizeof(wchar_t), L"%u", i);
#endif
    return out;
}

template<>
std::string CStringUtilT<char>::to_string(int i)
{
    char out[16];
#if defined(WIN32)
    _ltoa(i, out, 10);
#else
    sprintf(out, "%d", i);
#endif	
    return out; 
}

template<>
std::wstring CStringUtilT<wchar_t>::to_string(int i)
{
    wchar_t out[16];
#if defined(WIN32)
    _ltow(i, out, 10);
#else
    swprintf(out, sizeof(out) / sizeof(wchar_t), L"%d", i);
#endif	
    return out; 
}

template<>
std::string CStringUtilT<char>::to_string(XUINT64 i)
{
    char out[32];
#if defined(WIN32)
    _ui64toa(i, out, 10);
#else
    sprintf(out, "%llu", (long long unsigned int)i);
#endif
    return out;
}

template<>
std::wstring CStringUtilT<wchar_t>::to_string(XUINT64 i)
{
    wchar_t out[32];
#if defined(WIN32)
    _ui64tow(i, out, 10);
#else
    swprintf(out, sizeof(out) / sizeof(wchar_t), L"%llu", (long long unsigned int)i);
#endif
    return out;
}

template<>
std::string CStringUtilT<char>::to_string(XINT64 i)
{
    char out[32];
#if defined(WIN32)
    _i64toa(i, out, 10);
#else
    sprintf(out, "%lld", (long long int)i);
#endif
    return out;
}

template<>
std::wstring CStringUtilT<wchar_t>::to_string(XINT64 i)
{
    wchar_t out[32];
#if defined(WIN32)
    _i64tow(i, out, 10);
#else
    swprintf(out, sizeof(out) / sizeof(wchar_t), L"%lld", (long long int)i);
#endif
    return out;
}


template<>
unsigned int CStringUtilT<char>::str_to_uint32(const char* s)
{
#if defined(WIN32)
    return atol(s);
#else
    unsigned int out = 0;
    sscanf(s, "%u", &out);
    return out;
#endif
}

template<>
unsigned int CStringUtilT<wchar_t>::str_to_uint32(const wchar_t* s)
{
#if defined(WIN32)
    return _wtol(s);
#else
    unsigned int out = 0;
    swscanf(s, L"%u", &out);
    return out;
#endif
}

template<>
int CStringUtilT<char>::str_to_sint32(const char* s)
{
#if defined(WIN32)
    return atol(s);
#else
    return atoi(s);
#endif
}

template<>
int CStringUtilT<wchar_t>::str_to_sint32(const wchar_t* s)
{
#if defined(WIN32)
    return _wtol(s);
#else
    int out = 0;
    swscanf(s, L"%i", &out);
    return out;
#endif
}

template<>
int CStringUtilT<char>::compare_nocase(const char* str1, const char* str2)
{
#if defined(WIN32)
	return _stricmp(str1, str2);
#else
	return strcasecmp(str1, str2);
#endif
}

template<>
int CStringUtilT<wchar_t>::compare_nocase(const wchar_t* str1, const wchar_t* str2)
{
#if defined(WIN32)
	return _wcsicmp(str1, str2);
#else
	return wcscasecmp(str1, str2);
#endif
}

template<>
XUINT64 CStringUtilT<char>::str_to_uint64(const char* s)
{
#if defined(WIN32)
    return _atoi64(s);
#else	
    long long unsigned int out = 0;
    sscanf(s, "%llu", &out);
	return (XUINT64)out;
#endif
}

template<>
XUINT64 CStringUtilT<wchar_t>::str_to_uint64(const wchar_t* s)
{
#if defined(WIN32)
    return _wtoi64(s);
#else	
    long long unsigned int out = 0;
    swscanf(s, L"%llu", &out);
    return (XUINT64)out;
#endif
}

template<>
XINT64 CStringUtilT<char>::str_to_sint64(const char* s)
{
#if defined(WIN32)
    return _atoi64(s);
#else
    return (XINT64)atoll(s);
#endif
}

template<>
XINT64 CStringUtilT<wchar_t>::str_to_sint64(const wchar_t* s)
{
#if defined(WIN32)
    return _wtoi64(s);
#else
    long long int out = 0;
    swscanf(s, L"%llu", &out);
    return (XINT64)out;    
#endif
}


template<>
bool CStringUtilT<char>::str_to_bool(const char* s)
{
#if defined(WIN32)
    return ( 0 == _stricmp(s, "true") || 0 == _stricmp(s, "1") );
#else
    return (0 == strcasecmp(s, "true") || 0 == strcasecmp(s, "1"));
#endif
}



template<>
bool CStringUtilT<wchar_t>::str_to_bool(const wchar_t* s)
{
#if defined(WIN32)
    return ( 0 == _wcsicmp(s, L"true") || 0 == _wcsicmp(s, L"1") );
#else
    return ( 0 == wcscasecmp(s, L"true") || 0 == wcscasecmp(s, L"1") );
#endif
}

template<>
std::string CStringUtilT<char>::formatString(const char* format, ...)
{
	int nSize = 0;
	size_t nBuffsize = 128;
	char* pszData = (char*)malloc(nBuffsize);
	memset(pszData, 0, nBuffsize);
	while (true)
	{
		va_list args;
		va_start(args, format);
#ifdef WIN32
		nSize = _vsnprintf_s(pszData, nBuffsize, _TRUNCATE, format, args);
#else
		nSize = vsnprintf(pszData, nBuffsize, format, args);
#endif
		if (nSize == -1 || nSize > nBuffsize)
		{
			nBuffsize *= 2;
			if (nBuffsize > 65535)
			{
				break;
			}
			pszData = (char*)realloc(pszData, nBuffsize);
		}
		else
		{
			va_end(args);
			std::string strResult(pszData);
			free(pszData);
			return strResult;
		}
		va_end(args);
	}
	free(pszData);
	return std::string();
}

template<>
std::wstring CStringUtilT<wchar_t>::formatString(const wchar_t* format, ...)
{
	int nSize = 0;
	size_t nBuffsize = 128;
	wchar_t* pszData = (wchar_t*)malloc(nBuffsize * sizeof(wchar_t));
	memset(pszData, 0, nBuffsize * sizeof(wchar_t));

	while (true)
	{
		va_list args;
		va_start(args, format);

#ifdef WIN32
		nSize = _vsnwprintf_s(pszData, nBuffsize, _TRUNCATE, format, args);
#else
		nSize = vswprintf(pszData, nBuffsize, format, args);
#endif
		if (nSize > 0)
		{
			va_end(args);
			std::wstring strResult(pszData);
			free(pszData);
			return strResult;
		}
		else
		{
			nBuffsize *= 2;
			if (nBuffsize > 65535)
			{
				break;
			}
			pszData = (wchar_t*)realloc(pszData, nBuffsize * sizeof(wchar_t));
		}
		va_end(args);
	}

	free(pszData);
	return std::wstring();
}