#include "Thread.h"

//=============================================================================
CThread::CThread(unsigned int (*ThreadFuncPtr)(void *))
	: m_ThreadFuncPtr(ThreadFuncPtr)
	, m_pThreadParam(NULL)
	, m_nThreadCount(0)
	, m_pThreadHandle(NULL)
{
}


CThread::~CThread(void)
{
	WaitThreadExit();
	
	if(NULL != m_pThreadHandle)
	{
		delete[] m_pThreadHandle;
		m_pThreadHandle = NULL;
	}
}

/// 开始线程
BOOL CThread::StartThread(void* pThreadParam, uint32_t nThreadCount)
{
	/// 参数检查
	if(NULL == pThreadParam || nThreadCount <= 0)
		return FALSE;

	if(NULL != m_pThreadHandle || NULL == m_ThreadFuncPtr)
		return FALSE;

	m_nThreadCount = nThreadCount;
	m_pThreadParam = pThreadParam;

	m_pThreadHandle = new THREAD_HANDLE[nThreadCount];
	if(NULL == m_pThreadHandle)
		return FALSE;

	memset(m_pThreadHandle, 0, sizeof(m_pThreadHandle)*nThreadCount);

#ifdef _WIN32
	unsigned int nThreadID = 0;
	for(uint32_t nIndex=0; nIndex<nThreadCount; ++nIndex)
	{
		HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, Win32ThreadFunc,
			this, 0, &nThreadID);
		m_pThreadHandle[nIndex] = (THREAD_HANDLE)hHandle;
	}

	return TRUE;
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
    pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
    //pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	for(uint32_t nIndex=0; nIndex<nThreadCount; ++nIndex)
	{
		pthread_create(m_pThreadHandle[nIndex], &attr, LinuxThreadFunc, this);
	}
	pthread_attr_destroy(&attr);
	m_nThreadCount = nThreadCount;
    return TRUE;
#endif
}

/// 结束线程
BOOL CThread::StopThread(void)
{
#ifdef _WIN32
	for(uint32_t nIndex=0; nIndex<m_nThreadCount; ++nIndex)
	{
		TerminateThread((HANDLE)m_pThreadHandle[nIndex], 0);
	}
#else
	for(uint32_t nIndex=0; nIndex<m_nThreadCount; ++nIndex)
	{
		pthread_cancel((pthread_t)m_pThreadHandle[nIndex]);
	}
#endif
	return TRUE;
}

/// 等待线程退出
BOOL CThread::WaitThreadExit(void)
{
#ifdef _WIN32
	::WaitForMultipleObjects(m_nThreadCount, (HANDLE*)m_pThreadHandle, 
		TRUE, INFINITE);
#else
	for(uint32_t nIndex=0; nIndex<m_nThreadCount; ++nIndex)
	{
		pthread_join((pthread_t)m_pThreadHandle[nIndex]);
	}
#endif

	return TRUE;
}

//=============================================================================
#ifdef _WIN32
unsigned int CThread::Win32ThreadFunc(void)
{
	return m_ThreadFuncPtr(m_pThreadParam);
}

unsigned int __stdcall CThread::Win32ThreadFunc(void* pThreadParam)
{
	CThread* pThis = (CThread*)pThreadParam;
	return pThis->Win32ThreadFunc();
}

#else
void CThread::LinuxThreadFunc(void)
{
	return m_ThreadFuncPtr(m_pThreadParam);
}

void* CThread::LinuxThreadFunc(void* pThreadParam)
{
	CThread* pThis = (CThread*)pThreadParam;
	return pThis->LinuxThreadFunc();
}
#endif
