#pragma once

#ifdef WIN32
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
std::string AnsiToUtf8(std::string strAnsi);
std::string Utf8ToAnsi(std::string strUTF8);
#endif

void PopTip(const std::string& strTip, int nType);