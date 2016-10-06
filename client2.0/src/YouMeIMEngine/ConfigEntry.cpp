#include "ConfigEntry.h"

std::string CConfigEntry::REDIRECT_ADDR = "NETWORK_REDIRECT_ADD";
XString CConfigEntry::REDIRECT_ADDR_DEFAULT = __XT("192.168.1.1");


std::string CConfigEntry::REDIRECT_PORT = "NETWORK_REDIRECT_PORT";
int CConfigEntry::REDIRECT_PORT_DEFAULT = 8888;


std::string CConfigEntry::VERIFY_TIME = "VERIFY_TIME";


//数据上报的参数
std::string CConfigEntry::DATAREPORT_ADDR="DATAREPORT_ADDR";
XString CConfigEntry::DATAREPORT_ADDR_DEFAULT=__XT("192.168.1.1");

std::string CConfigEntry::DATAREPORT_PORT="DATAREPORT_PORT";
int CConfigEntry::DATAREPORT_PORT_DEFAULT=190;


std::string CConfigEntry::KEYWORD_ADDRESS = "KeywordAddress"; 
std::string CConfigEntry::KEYWORD_MD5 = "KeywordMD5";

std::string CConfigEntry::LOG_LEVEL = "LOG_LEVEL";