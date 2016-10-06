
/**************************** 条件编译选项和头文件 ****************************/
#ifndef __FORMAT_UTILITY_H_DA84050F_6692_40f3_95E1_9CFA4122D924__  
#define __FORMAT_UTILITY_H_DA84050F_6692_40f3_95E1_9CFA4122D924__
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <string>
/********************************** 常量和宏 **********************************/

/********************************** 数据类型 **********************************/

/********************************** 函数声明 **********************************/

/*********************************** 类定义 ***********************************/
template<class _char> class CStringUtilT;

typedef CStringUtilT<XCHAR> CStringUtil;

template<class _char>
class CStringUtilT
{
public:
    typedef std::basic_string<_char> _string;

    static _string      to_string(unsigned char i);
    static _string      to_string(char i);

    static _string      to_string(unsigned short i);
    static _string      to_string(short i);

    static _string      to_string(unsigned int i);
    static _string      to_string(int i);

    static _string      to_string(XUINT64 i);
    static _string      to_string(XINT64 i);


    static unsigned char        str_to_uint8(const _string& s);
    static char        str_to_sint8(const _string& s);

    static unsigned char        str_to_uint8(const _char* s);
    static char        str_to_sint8(const _char* s);

    static unsigned short       str_to_uint16(const _string& s);
    static short       str_to_sint16(const _string& s);

    static unsigned short       str_to_uint16(const _char* s);
    static short       str_to_sint16(const _char* s);

    static unsigned int       str_to_uint32(const _string& s);
    static unsigned int       str_to_uint32(const _char* s);

    static int       str_to_sint32(const _string& s);
    static int       str_to_sint32(const _char* s);

    static XUINT64       str_to_uint64(const _string& s);
    static XUINT64       str_to_uint64(const _char* s);

    static XINT64       str_to_sint64(const _string& s);
    static XINT64       str_to_sint64(const _char* s);


    static _string      to_string(bool b);
    static bool         str_to_bool(const _string& s);
    static bool         str_to_bool(const _char* s);

    static bool         is_readable(byte ch);
    static bool         is_readable(_char ch);
    static bool         IsValidChar(_char ch);
    
    static bool         is_hex(byte ch);
    static bool         is_hex(_char ch);
    static bool         is_hex_string(const byte* str, unsigned int len);
    static bool         is_hex_string(const _char* str, unsigned int len);
    static bool         is_hex_string(const _string& str);

    static int       hex_to_int(byte ch);
    static int       hex_to_int(_char ch);	

    static bool         byte_to_hex(byte data, _char* buf, unsigned int len = 2);

    static _string      bytes_to_hex_string(const byte* data, unsigned int len);
    static _string      bytes_to_hex_string(const std::string& bytes);

    static int       hex_string_to_bytes(const byte* hex_buffer, unsigned int hex_len, byte* buffer, unsigned int buffer_len);	
    static int       hex_string_to_bytes(const _char* hex_buffer, unsigned int hex_len, byte* buffer, unsigned int buffer_len);
    static int       hex_string_to_bytes(const _string& hex, byte* buffer, unsigned int buffer_len);    
    static std::string  hex_string_to_bytes(const _string& hex);

	static _string replace_text(const _string& strSource, const _string& strOld, const _string& strNew);
	static int       compare_nocase(const _char* str1, const _char* str2);

	static std::string formatString(const char* format, ...);
	static std::wstring formatString(const wchar_t* format, ...);
private:
	static _char        s_hex_data_map[];
};




/*********************************** 类实现 ***********************************/

template<class _char>
_char CStringUtilT<_char>::s_hex_data_map[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

template<class _char>
std::basic_string<_char> CStringUtilT<_char>::to_string(unsigned char i)
{
    return to_string((unsigned int)i);
}

template <class _char>
std::basic_string<_char> CStringUtilT<_char>::replace_text(const _string& strSource, const _string& strOld, const _string& strNew)
{
	size_t pos = 0;
	_string tempStr = strSource;
	typename _string::size_type newStrLen = strNew.length();
	typename _string::size_type oldStrLen = strOld.length();
	while (true)
	{
		pos = tempStr.find(strOld, pos);
		if (pos == std::wstring::npos) break;

		tempStr.replace(pos, oldStrLen, strNew);
		pos += newStrLen;
	}

	return tempStr;
}


template<class _char>
std::basic_string<_char> CStringUtilT<_char>::to_string(char i)
{
    return to_string((int)i);
}

template<class _char>
std::basic_string<_char> CStringUtilT<_char>::to_string(unsigned short i)
{
    return to_string((unsigned int)i);
}

template<class _char>
std::basic_string<_char> CStringUtilT<_char>::to_string(short i)
{
    return to_string((int)i);
}

template<class _char>
unsigned char CStringUtilT<_char>::str_to_uint8(const std::basic_string<_char>& s)
{
    return (unsigned char)str_to_uint32(s);	
}

template<class _char>
char CStringUtilT<_char>::str_to_sint8(const std::basic_string<_char>& s)
{
    return (char)str_to_sint32(s);
}

template<class _char>
unsigned char CStringUtilT<_char>::str_to_uint8(const _char* s)
{
    return (unsigned char)str_to_uint32(s);	
}

template<class _char>
char CStringUtilT<_char>::str_to_sint8(const _char* s)
{
    return (char)str_to_sint32(s);
}

template<class _char>
unsigned short CStringUtilT<_char>::str_to_uint16(const std::basic_string<_char>& s)
{
    return (unsigned short)str_to_uint32(s);
}

template<class _char>
short CStringUtilT<_char>::str_to_sint16(const std::basic_string<_char>& s)
{
    return (short)str_to_sint32(s);
}

template<class _char>
unsigned short CStringUtilT<_char>::str_to_uint16(const _char* s)
{
    return (unsigned short)str_to_uint32(s);
}

template<class _char>
short CStringUtilT<_char>::str_to_sint16(const _char* s)
{
    return (short)str_to_sint32(s);
}

template<class _char>
unsigned int CStringUtilT<_char>::str_to_uint32(const std::basic_string<_char>& s)
{
    return str_to_uint32(s.c_str());
}


template<class _char>
int CStringUtilT<_char>::str_to_sint32(const std::basic_string<_char>& s)
{
    return str_to_sint32(s.c_str());
}


template<class _char>
XUINT64 CStringUtilT<_char>::str_to_uint64(const std::basic_string<_char>& s)
{
    return str_to_uint64(s.c_str());
}


template<class _char>
XINT64 CStringUtilT<_char>::str_to_sint64(const std::basic_string<_char>& s)
{
    return str_to_sint64(s.c_str());
}


template<class _char>
std::basic_string<_char> CStringUtilT<_char>::to_string(bool b)
{
    _char t[] = { '1', '\0' };
    _char f[] = { '0', '\0' };
    return b ? t : f;
}

template<class _char>
bool CStringUtilT<_char>::str_to_bool(const std::basic_string<_char>& s)
{
    return str_to_bool( s.c_str() );
}


/*******************************************************************************
* 函数名称	: format_utility_t<_char>::is_readable
* 功能描述	: 判断一个字符是否可读，即ASCII码属于[32,126]
* 参　　数	: byte ch				要判断的字符
* 返 回 值	: bool					true表示可读，false表示不可读
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月11日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::is_readable(byte ch)
{
    return (ch > 31 && ch < 127);
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::is_readable
* 功能描述	: 判断一个字符是否可读，即ASCII码属于[32,126]
* 参　　数	: _char ch				要判断的字符
* 返 回 值	: bool					true表示可读，false表示不可读
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年4月8日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::is_readable(_char ch)
{
    return (ch > 31 && ch < 127);
}

template<class _char>
bool CStringUtilT<_char>::IsValidChar(_char c)
{
    if (((c >= '0') && (c <='9'))||
        ((c >= 'A') && (c <= 'Z'))||
        ((c >= 'a') && (c <= 'z'))||
        (c == '_'))
    {
        return true;
    }
    return  false;
}


/*******************************************************************************
* 函数名称	: format_utility_t<_char>::is_hex
* 功能描述	: 判断一个字符是否为16进制字符
* 参　　数	: byte ch			要处理的字符
* 返 回 值	: bool				true表示是16进制字符，false表示不是16进制字符
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月9日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::is_hex(byte ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return true;
    }
    if(ch >= 'a' && ch <= 'f')
    {
        return true;
    }
    if(ch >= 'A' && ch <= 'F')
    {
        return true;
    }

    return false;
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::is_hex
* 功能描述	: 判断一个字符是否为16进制字符
* 参　　数	: _char ch			要处理的字符
* 返 回 值	: bool				true表示是16进制字符，false表示不是16进制字符
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年4月8日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::is_hex(_char ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return true;
    }
    if(ch >= 'a' && ch <= 'f')
    {
        return true;
    }
    if(ch >= 'A' && ch <= 'F')
    {
        return true;
    }

    return false;
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::is_hex_string
* 功能描述	: 判断一个字符串是否为16进制字符串
* 参　　数	: const byte* str		要判断的字符串
* 参　　数	: uint32 len			字符串长度
* 返 回 值	: bool					true表示为16进制字符串，false表示不是16进制字符串
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月11日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::is_hex_string(const byte* str, unsigned int len)
{
    if(NULL == str || 0 == len)
    {
        return false;
    }

    for(unsigned int i = 0; i < len; i ++)
    {
        if(!is_hex(str[i]))
        {
            return false;
        }
    }

    return true;
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::is_hex_string
* 功能描述	: 判断一个字符串是否为16进制字符串
* 参　　数	: const _char* str		要判断的字符串
* 参　　数	: uint32 len			字符串长度
* 返 回 值	: bool					true表示为16进制字符串，false表示不是16进制字符串
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年4月8日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::is_hex_string(const _char* str, unsigned int len)
{
    if(NULL == str || 0 == len)
    {
        return false;
    }

    for(unsigned int i = 0; i < len; i ++)
    {
        if(!is_hex(str[i]))
        {
            return false;
        }
    }

    return true;
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::is_hex_string
* 功能描述	: 判断一个字符串是否为16进制字符串
* 参　　数	: const std::string& str		要判断的字符串
* 返 回 值	: bool					true表示为16进制字符串，false表示不是16进制字符串
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月11日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::is_hex_string(const std::basic_string<_char>& str)
{
    if(str.empty())
    {
        return false;
    }

    return is_hex_string(str.c_str(), str.length());
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::hex_2_int
* 功能描述	: 将十六制字符表示为十进制整数,如'A'转换为10
* 参　　数	: byte ch				16进制字符
* 返 回 值	: sint32				对应的值，非16进制字符串返回为-1
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月9日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
int CStringUtilT<_char>::hex_to_int(byte ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    else if(ch >= 'A' && ch <= 'F')
    {
        return ch - 'A' + 10;
    }
    else if(ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    else
    {
        return -1;
    }
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::hex_to_int
* 功能描述	: 将十六制字符表示为十进制整数,如'A'转换为10
* 参　　数	: _char ch					16进制字符
* 返 回 值	: sint32					对应的值，非16进制字符串返回为-1
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年4月8日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
int CStringUtilT<_char>::hex_to_int(_char ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    else if(ch >= 'A' && ch <= 'F')
    {
        return ch - 'A' + 10;
    }
    else if(ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    else
    {
        return -1;
    }
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::byte_to_hex
* 功能描述	: 将一个字符转换为16进制
* 参　　数	: byte data				要被处理的字符
* 参　　数	: _char* buf			保存处理后数据的缓存指针
* 参　　数	: uint32 len			保存处理后数据的缓存大小
* 返 回 值	: bool					true表示正确处理，false表示出错
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月9日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
bool CStringUtilT<_char>::byte_to_hex(byte data, _char* buf, unsigned int len)
{
    if(NULL == buf || len < 2)
    {
        return false;
    }

    unsigned char h1 = (data & 0xF0) >> 4;
    unsigned char h2 = data & 0xF;

    buf[0] = s_hex_data_map[h1];
    buf[1] = s_hex_data_map[h2];

    return true;
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::bytes_to_hex_string
* 功能描述	: 将一串字符转换成16进制
* 参　　数	: const byte* data		要被处理的字符串
* 参　　数	: uint32 len			要被处理的字符串
* 返 回 值	: std::string				处理后的数据
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月9日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
std::basic_string<_char> CStringUtilT<_char>::bytes_to_hex_string(const byte* data, unsigned int len)
{
    if(NULL == data || 0 == len)
    {
        return std::basic_string<_char>();
    }

    std::basic_string<_char> str(len * 2, '*');
    _char* buff = (_char*)str.c_str();
    for(unsigned int i = 0; i < len; i++)
    {
        byte_to_hex(*(data + i), (buff + i * 2));
    }

    return str;
}

/*******************************************************************************
* 函数名称  : format_utility_t<_char>::bytes_to_hex_string
* 功能描述  : 将一段字节数据转换为十六进制字符串
* 参　　数  : const std::string& bytes  字节数据
* 返 回 值  : std::string                   十六进制字符串
* 作　　者  : 王卫华 杨晓虎
* 设计日期  : 2009年4月2日
* 修改日期         修改人         修改内容
*******************************************************************************/
template<class _char>
std::basic_string<_char> CStringUtilT<_char>::bytes_to_hex_string(const std::string& bytes)
{
    return bytes_to_hex_string( (const byte*)bytes.c_str(), (unsigned int)bytes.size() );
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::hex_text2bytes
* 功能描述	: 将16进制的数据还原为字符串
* 参　　数	: const _char* hex_buffer		需要处理的16进制数据
* 参　　数	: uint32 hex_len				16进制数据长度
* 参　　数	: byte* buffer					保存处理后数据的缓存指针
* 参　　数	: uint32 buffer_len				保存处理后数据的缓存大小
* 返 回 值	: sint32						处理后字符串长度，-1表示出错
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月11日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
int CStringUtilT<_char>::hex_string_to_bytes(const byte* hex_buffer, unsigned int hex_len, byte* buffer, unsigned int buffer_len)
{
    if(NULL == hex_buffer || NULL == buffer)
    {
        return -1;
    }
    if(hex_len % 2 != 0)
    {
        return -1;
    }

    unsigned int nbytes = hex_len / 2;
    if(buffer_len < nbytes)
    {
        return -1;
    }	

    for(unsigned int i = 0; i < nbytes; i++)
    {
        byte c1 = hex_buffer[i * 2];
        int h1 = hex_to_int(c1);
        if(h1 < 0)
        {
            return -1;
        }

        byte c2 = hex_buffer[i * 2 + 1];
        int h2 = hex_to_int(c2);
        if(h2 < 0)
        {
            return -1;
        }

        byte b = (byte)((h1 << 4) | h2);
        buffer[i] = b;			
    }	

    return nbytes;
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::hex_string_to_bytes
* 功能描述	: 将16进制的数据还原为字符串
* 参　　数	: const _char* hex_buffer		需要处理的16进制数据
* 参　　数	: uint32 hex_len				16进制数据长度
* 参　　数	: byte* buffer					保存处理后数据的缓存指针
* 参　　数	: uint32 buffer_len				保存处理后数据的缓存大小
* 返 回 值	: sint32						处理后字符串长度，-1表示出错
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年4月8日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
int CStringUtilT<_char>::hex_string_to_bytes(const _char* hex_buffer, unsigned int hex_len, byte* buffer, unsigned int buffer_len)
{	
    if(hex_len % 2 != 0 || NULL == buffer)
    {
        return -1;
    }

    unsigned int nbytes = hex_len / 2;
    if(buffer_len < nbytes)
    {
        return -1;
    }	

    for(unsigned int i = 0; i < nbytes; i++)
    {
        _char c1 = hex_buffer[i * 2];
        int h1 = hex_to_int(c1);
        if(h1 < 0)
        {
            return -1;
        }

        _char c2 = hex_buffer[i * 2 + 1];
        int h2 = hex_to_int(c2);
        if(h2 < 0)
        {
            return -1;
        }

        byte b = (byte)((h1 << 4) | h2);
        buffer[i] = b;			
    }	

    return nbytes;
}

/*******************************************************************************
* 函数名称	: format_utility_t<_char>::hex_string_to_bytes
* 功能描述	: 将16进制的字符串还原为字节数据
* 参　　数	: const std::string& hex		需要处理的16进制数据
* 参　　数	: byte* buffer				保存处理后数据的缓存指针
* 参　　数	: sint32 buffer_len			保存处理后数据的缓存大小
* 返 回 值	: sint32					处理后字符串长度，-1表示出错
* 作　　者	: 王卫华 杨晓虎
* 设计日期	: 2009年3月9日
* 修改日期		   修改人		  修改内容
*******************************************************************************/
template<class _char>
int CStringUtilT<_char>::hex_string_to_bytes(const std::basic_string<_char>& hex, byte* buffer, unsigned int buffer_len)
{
    return hex_string_to_bytes( hex.c_str(), (unsigned int)hex.size(), buffer, buffer_len );
}

/*******************************************************************************
* 函数名称  : format_utility_t<_char>::hex_string_to_bytes
* 功能描述  : 将十六进制的字符串还原为字节数据
* 参　　数  : const std::string& hex        十六进制字符串
* 返 回 值  : std::string               存储字节数据的字符串，错误情况下返回空字符串
* 作　　者  : 王卫华 杨晓虎
* 设计日期  : 2009年4月2日
* 修改日期         修改人         修改内容
*******************************************************************************/
template<class _char>
std::string CStringUtilT<_char>::hex_string_to_bytes(const std::basic_string<_char>& hex)
{
    std::string ret;
    ret.resize( hex.size() / 2 );
    int len = hex_string_to_bytes( hex.c_str(), (unsigned int)hex.size(), (byte*)ret.c_str(), (unsigned int)ret.size() );
    if ( len == -1 )
    {
        ret.erase();
    }
    else
    {
        ret.resize( len );
    }
    return ret;
}


#endif // end of __FORMAT_UTILITY_H_DA84050F_6692_40f3_95E1_9CFA4122D924__
