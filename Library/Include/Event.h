/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : Event.h
/// \brief   : ÊÂ¼þ
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-21
///============================================================================
#ifndef __EVENT_H__
#define __EVENT_H__

#ifdef _WIN32
#include <Windows.h>
#include <WinSock2.h>
#else
#include <pthread.h>
#endif

#include "TypeDefine.h"

//=============================================================================
#define TIMEOUT_INF ~((timeout_t) 0)

//=============================================================================
#ifndef _WIN32
int32_t GetTimeOfDay(struct timeval *tp)
{
	struct timeval temp;
	int32_t ret = 0;

	ret = ::gettimeofday(&temp, NULL);
	if(ret == 0)
	{
		memcpy(tp, &temp, sizeof(struct timeval));
	}
	return ret;
}

timespec *GetTimeout(struct timespec *spec, uint32_t nTimeoutMS)
{
	static	struct timespec myspec;

	if(spec == NULL)
		spec = &myspec;

	struct timeval current;

	SysTime::getTimeOfDay(&current);
	spec->tv_sec = current.tv_sec + ((timer + current.tv_usec / 1000) / 1000);
	spec->tv_nsec = ((current.tv_usec / 1000 + timer) % 1000) * 1000000;
}
#endif

//=============================================================================
// class CEvent
class CEvent
{
public:
	CEvent(void)
	{
#ifdef _WIN32
		m_hEvent = ::CreateEventA(NULL, TRUE, FALSE, NULL);
#else
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutex_init(&m_Mutex, &attr);
		pthread_mutexattr_destroy(&attr);
		pthread_cond_init(&m_Cond, NULL);
		m_bIsSignaled = false;
		m_nCount = 0;
#endif
	}

	~CEvent(void)
	{
#ifdef _WIN32
		::CloseHandle(m_hEvent);
#else
		pthread_cond_destroy(&m_Cond);
		pthread_mutex_destroy(&m_Mutex);
#endif
	}

	__inline BOOL WaitEvent(void)
	{
#ifdef _WIN32
		return (::WaitForSingleObject(m_hEvent, INFINITE) == WAIT_OBJECT_0);
#else
		return WaitEvent(TIMEOUT_INF);
#endif
	}

	__inline BOOL WaitEvent(uint32_t nTimeoutMS)
	{
#ifdef _WIN32
		return (::WaitForSingleObject(m_hEvent, nTimeoutMS) == WAIT_OBJECT_0);
#else
		int	rc = 0;
		struct	timespec spec;

		pthread_mutex_lock(&m_Mutex);
		long count = m_nCount;

		while(!m_bIsSignaled && m_nCount == count) 
		{
			if(timer != TIMEOUT_INF)
				rc = pthread_cond_timedwait(&m_Cond, &m_Mutex, GetTimeout(&spec, nTimeoutMS));
			else
				pthread_cond_wait(&m_Cond, &m_Mutex);
			if(rc == ETIMEDOUT)
				break;
		}
		pthread_mutex_unlock(&m_Mutex);
		if(rc == ETIMEDOUT)
			return FALSE;
		return TRUE;
#endif
	}

	__inline BOOL SetEvent(void)
	{
#ifdef _WIN32
		return ::SetEvent(m_hEvent);
#else
		pthread_mutex_lock(&m_Mutex);
		m_bIsSignaled = TRUE;
		++m_nCount;
		pthread_cond_broadcast(&m_Cond);
		pthread_mutex_unlock(&m_Mutex);
		return TRUE;
#endif
	}

	__inline BOOL ResetEvent(void)
	{
#ifdef _WIN32
		return ::ResetEvent(m_hEvent);
#else
		m_bIsSignaled = FALSE;
		return TRUE;
#endif
	}

private:
#ifndef WIN32
	pthread_mutex_t m_Mutex;
	pthread_cond_t m_Cond;
	BOOL m_bIsSignaled
	uint32_t m_nCount;
#else
	HANDLE m_hEvent;
#endif
};

#endif //__EVENT_H__
