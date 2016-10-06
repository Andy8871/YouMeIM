#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
class CConfigEntry
{
public:
	//重定向地址
	static std::string REDIRECT_ADDR;
	static XString REDIRECT_ADDR_DEFAULT;

	//重定向端口
	static std::string REDIRECT_PORT;
	static int REDIRECT_PORT_DEFAULT;

	//数据上报的参数
	static std::string DATAREPORT_ADDR;
	static XString DATAREPORT_ADDR_DEFAULT;

	static std::string DATAREPORT_PORT;
	static int DATAREPORT_PORT_DEFAULT;

	//服务器时间
	static std::string VERIFY_TIME;

	//关键词下载的http 地址
	static std::string KEYWORD_ADDRESS;
	static std::string KEYWORD_MD5;

	static std::string LOG_LEVEL;
};

