#pragma once

#ifdef WIN32
#include <Windows.h>
typedef HANDLE SEMAPHORE_T;
#else
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/syslimits.h>
#include <sys/errno.h>

#	include <fcntl.h> /* O_CREAT */
#	include <sys/stat.h> /* S_IRUSR, S_IWUSR*/
typedef sem_t* SEMAPHORE_T;

#ifndef NAME_MAX
#define NAME_MAX 255

#endif
#	include <fcntl.h> /* O_CREAT */
#	include <sys/stat.h> /* S_IRUSR, S_IWUSR*/
#endif // WIN32
namespace youmecommon {

class CXSemaphore
{
public:
	CXSemaphore(int iInitValue=0);
	~CXSemaphore();

	bool Increment();
	bool Decrement();

	SEMAPHORE_T m_handle;
#if (defined OS_OSX) || (defined OS_IOS) || (defined OS_IOSSIMULATOR)
    char m_semName[NAME_MAX+1];
    
#endif
};
}

