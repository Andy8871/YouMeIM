#include "Log.h"
#ifdef WIN32
#include <tchar.h>
#endif
#include <time.h>
#define TSK_LOG_BUFSIZE 1024
#include <assert.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/TimeUtil.h>
#if OS_ANDROID
#include <android/log.h>
#endif
youmecommon::CXFile g_logFile;
YouMe_LOG_LEVEL g_logLevel= LOG_LEVEL_DEBUG;
const XCHAR *LogLevelToString(int dLevel)
{
	switch (dLevel)
	{
	case LOG_LEVEL_ERROR:
		return __XT("ERROR");

	case LOG_LEVEL_WARNING:
		return __XT("WARNING");

	case LOG_LEVEL_FATAL:
		return __XT("FATAL");

	case LOG_LEVEL_DEBUG:
		return __XT("DEBUG");

	case LOG_LEVEL_INFO:
		return __XT("INFO");


	default:
		return __XT("UNDEFINED");
	}
}
bool YouMe_LOG_Init(const XString& strLogPath)
{
    if (g_logFile.IsOpen()) {
        g_logFile.Close();
    }
    g_logFile.LoadFile(strLogPath, youmecommon::CXFile::Mode_Open_ALWAYS);
    bool bSuccess = g_logFile.IsOpen();
    if ( bSuccess)
    {
        g_logFile.Seek(0, SEEK_END);
    }
    return bSuccess;
}
void YouMe_Log_Uninit()
{
    g_logFile.Close();
}
void YouMe_LOG_SetLevel(YouMe_LOG_LEVEL level)
{
    g_logLevel = level;
}

#if OS_ANDROID
int LogLevelToAndroidLevel (int dLevel)
{
    switch (dLevel)
    {
        case LOG_LEVEL_ERROR:
            return ANDROID_LOG_ERROR;
            
        case LOG_LEVEL_WARNING:
            return ANDROID_LOG_WARN;
            
        case LOG_LEVEL_FATAL:
            return ANDROID_LOG_FATAL;
            
        case LOG_LEVEL_DEBUG:
            return ANDROID_LOG_DEBUG;
            
        case LOG_LEVEL_INFO:
            return ANDROID_LOG_INFO;
            
            
        default:
            return ANDROID_LOG_INFO;
    }
}
#endif
void YouMe_LOG_imp(const XCHAR *pszFun, const XCHAR *pszFile, int dLine, int dLevel, const XCHAR *pszFormat, ...)
{
    if (dLevel > g_logLevel) {
        return;
    }
	time_t timep = time(NULL);
	struct tm *p_tm = localtime(&timep);

	XCHAR buffer[TSK_LOG_BUFSIZE + 1] = { 0 };
#ifdef WIN32
	int dSize = _snwprintf(buffer, TSK_LOG_BUFSIZE, __XT("%04d/%02d/%02d %02d:%02d:%02d.%03d 线程id:%d %s "), p_tm->tm_year + 1900,
		p_tm->tm_mon+1,
		p_tm->tm_mday,
		p_tm->tm_hour,
		p_tm->tm_min,
		p_tm->tm_sec,
		(int)(youmecommon::CTimeUtil::GetTimeOfDay_MS() % 1000),
		GetCurrentThreadId(),
		LogLevelToString(dLevel));
#else
    int dSize = snprintf(buffer, TSK_LOG_BUFSIZE, __XT("%04d/%02d/%02d %02d:%02d:%02d.%03d 线程id：%lu  %s: "),  p_tm->tm_year+1900,
                         p_tm->tm_mon+1,
                         p_tm->tm_mday,
                         p_tm->tm_hour,
                         p_tm->tm_min,
                         p_tm->tm_sec,
						 (int)(youmecommon::CTimeUtil::GetTimeOfDay_MS() % 1000),
                         (unsigned long)pthread_self(),
                         LogLevelToString(dLevel));


    
#endif // WIN32

	if (dSize < TSK_LOG_BUFSIZE)
	{
		va_list args;
		va_start(args, pszFormat);
#ifdef WIN32

		dSize += _vsnwprintf(buffer + dSize, TSK_LOG_BUFSIZE - dSize, pszFormat, args);
#else
        dSize += vsnprintf(buffer + dSize, TSK_LOG_BUFSIZE - dSize, pszFormat, args);
#endif // WIN32
		va_end(args);
	}

	
	if (dSize < TSK_LOG_BUFSIZE)
	{
#ifdef WIN32
		_snwprintf(buffer + dSize, TSK_LOG_BUFSIZE - dSize, __XT("[%s#%s:%d]\n"), pszFile, pszFun, dLine);
#else
		snprintf(buffer + dSize, TSK_LOG_BUFSIZE - dSize, __XT("[%s#%s:%d]\n"),  pszFile, pszFun,dLine);
#endif
		
	}
	if (g_logFile.IsOpen())
	{
#ifdef WIN32
		std::string strUTF8Log 	=Unicode_to_UTF8(buffer, XStrLen(buffer));
		g_logFile.Write((const byte*)strUTF8Log.c_str(), strUTF8Log.length());
#else
	g_logFile.Write((const byte*)buffer, XStrLen(buffer) * sizeof(XCHAR));
#endif
		g_logFile.Flush();
	}
	
#ifdef WIN32
	OutputDebugStringW(buffer);
#else
	printf("%s",buffer);
#ifdef OS_ANDROID
    int iAndroidLogLevel = LogLevelToAndroidLevel (dLevel);
    __android_log_write (iAndroidLogLevel, "YOUMEIM", buffer);
#endif
#endif
}