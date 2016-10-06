#pragma once
#include <CrossPlatformDefine/PlatformDef.h>


enum YouMe_LOG_LEVEL
{
	LOG_LEVEL_FATAL = 1,
	LOG_LEVEL_ERROR = 10,
	LOG_LEVEL_WARNING = 20,
	LOG_LEVEL_INFO = 40,
	LOG_LEVEL_DEBUG = 50,
};

bool YouMe_LOG_Init(const XString& strLogPath);
void YouMe_LOG_SetLevel(YouMe_LOG_LEVEL level);
void YouMe_Log_Uninit();

#ifdef LOG_FATAL
#define YouMe_LOG_Fatal( ...) YouMe_LOG_imp (_XFUNCTION_,_XFILE_, __LINE__, (LOG_LEVEL_FATAL), __VA_ARGS__)
#else
#define YouMe_LOG_Fatal( ...)
#endif

#ifdef LOG_ERROR
#define YouMe_LOG_Error( ...) YouMe_LOG_imp (_XFUNCTION_,_XFILE_, __LINE__, (LOG_LEVEL_ERROR), __VA_ARGS__)
#else
#define YouMe_LOG_Error( ...)
#endif

#ifdef LOG_WARNING
#define YouMe_LOG_Warning( ...) YouMe_LOG_imp (_XFUNCTION_,_XFILE_, __LINE__, (LOG_LEVEL_WARNING), __VA_ARGS__)
#else
#define YouMe_LOG_Warning( ...)
#endif

#ifdef LOG_INFO
#define YouMe_LOG_Info( ...) YouMe_LOG_imp (_XFUNCTION_,_XFILE_, __LINE__, (LOG_LEVEL_INFO), __VA_ARGS__)
#else
#define YouMe_LOG_Info( ...)
#endif

#ifdef LOG_DEBUG
#define YouMe_LOG_Debug( ...) YouMe_LOG_imp (_XFUNCTION_,_XFILE_, __LINE__, (LOG_LEVEL_DEBUG), __VA_ARGS__)
#else
#define YouMe_LOG_Debug( ...)
#endif



void YouMe_LOG_imp(const XCHAR *pszFun, const XCHAR *pszFile, int dLine, int dLevel, const XCHAR *pszFormat, ...);
