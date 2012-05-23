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
/// \file    : Mutex.h
/// \brief   : ª•≥‚¡ø
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-21
///============================================================================
#ifndef __MUTEX_H__
#define __MUTEX_H__

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "TypeDefine.h"

//=============================================================================
// class CMutex
class CMutex
{
public:
	CMutex(const TCHAR* szMutexName)
	{
#ifdef _WIN32
		m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
#else
		pthread_mutexattr_t attr;   
		pthread_mutexattr_init(&attr);   
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_hMutex, &attr);
		pthread_mutexattr_destroy(&attr);
#endif
	}

	~CMutex(void)
	{
#ifdef _WIN32
		CloseHandle(m_hMutex);
#else
		pthread_mutex_destroy(&m_hMutex);
#endif
	}

	__inline void EnterMutex(void)
	{
#ifdef WIN32
		WaitForSingleObject(m_hMutex, INFINITE);
#else
		pthread_mutex_lock(&m_hMutex);
#endif
	}

	__inline void LeaveMutex(void)
	{
#ifdef WIN32
		ReleaseMutex(m_hMutex);
#else
		pthread_mutex_unlock(&m_hMutex);
#endif
	};

private:
#ifdef _WIN32
	HANDLE m_hMutex;
#else
	pthread_mutex_t m_hMutex;
#endif
};

//=============================================================================
// class CMutexAutoLock
class CMutexAutoLock
{
public:
	CMutexAutoLock(CMutex& mutex)
		: m_Mutex(mutex)
	{
		m_Mutex.EnterMutex();
	}

	~CMutexAutoLock(void)
	{
		m_Mutex.leaveMutex();
	}

private:
	Mutex& m_Mutex;
};

#endif //__MUTEX_H__
