#include "TimeUtil.h"
#ifdef WIN32
#include <windows.h>
#include <WinSock2.h>
#else
#include <sys/time.h>
#endif
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

namespace youmecommon {

CTimeUtil::CTimeUtil()
{
}


CTimeUtil::~CTimeUtil()
{
}

XINT64 CTimeUtil::GetTimeOfDay_MS()
{
    timeval tv;
#ifdef WIN32
    FILETIME ft;
    XINT64 tmpres = 0;
    GetSystemTimeAsFileTime(&ft);
    
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
    
    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tv.tv_sec = (long)(tmpres / 1000000UL);
    tv.tv_usec = (long)(tmpres % 1000000UL);
    
#else
    gettimeofday(&tv, NULL);
#endif
	return (((XINT64)tv.tv_sec)*(XINT64)1000) + (((XINT64)tv.tv_usec) / (XINT64)1000);
}
}
