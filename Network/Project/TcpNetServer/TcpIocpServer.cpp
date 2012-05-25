#ifdef _WIN32
#include "TcpIocpServer.h"
#include "DebugTrace.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

//=============================================================================
/// 监听数量
#define DEFAULT_LISTEN_BACK_LOG	50
/// 重叠结构缓存池尺寸
#define OVERLAP_CACHE_SIZE		2048

/// 最大连接数
#define MAX_CONNECT_COUNT		50000

//=============================================================================
CTcpIocpServer::CTcpIocpServer(void)
	: m_hIocp(INVALID_HANDLE_VALUE)
	, m_hListenSocket(INVALID_SOCKET)
	, m_pEvent(NULL)
	, m_lpfnAcceptEx(NULL)
	, m_lpfnGetAcceptExSockAddrs(NULL)
	, m_CompletePotrThread(&CompletePortThread)
	, m_CheckThread(&ConnectCheckThread)
	, m_OverloapCache(OVERLAP_CACHE_SIZE)
{
	WSADATA lwsaData;
	WSAStartup(MAKEWORD(2,2), &lwsaData);

	//send时公用的overlap,
	memset(&m_oWriteOverlap, 0, sizeof(WSAOVERLAPPED));
	m_oWriteOverlap.hEvent = WSACreateEvent();		  //写事件
	m_oWriteOverlap.hEvent = (HANDLE)((DWORD)m_oWriteOverlap.hEvent | 0x01); //写事件不再通知完成断口
}


CTcpIocpServer::~CTcpIocpServer(void)
{
	Destroy();

	WSACleanup();
}

BOOL CTcpIocpServer::Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent)
{
	// 参数检查
	ASSERT(pSvrEvent);
	ASSERT(nSvrPort != 0);
	if(NULL == pSvrEvent || 0 == nSvrPort)
		return FALSE;

	// 重复创建检查
	if(INVALID_SOCKET != m_hListenSocket)
		return FALSE;

	BOOL bResult = FALSE;
	do 
	{
		// 创建完成端口
		m_hIocp = CreateCompletePort();
		if(INVALID_HANDLE_VALUE == m_hIocp)
			break;

		// 创建SOCKET句柄
		m_hListenSocket = CreateSocket(nSvrPort);
		if(INVALID_SOCKET == m_hListenSocket)
			break;

		// 完成端口与SOCKET绑定
		m_oListenContext.m_hSocket = m_hListenSocket;
		if (!CreateIoCompletionPort((HANDLE)m_hListenSocket, m_hIocp, 
			(ULONG_PTR)&m_oListenContext, 0))
		{
			TraceLogError("CTcpIocpServer::Create 与完成端口关联失败 ERROR=%d\n", 
				WSAGetLastError());
			break;
		}

		// 绑定端口
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(nSvrPort);
		if (SOCKET_ERROR == bind(m_hListenSocket, (LPSOCKADDR)&addr, sizeof(addr)))
		{
			TraceLogError("CTcpIocpServer::Create 绑定端口失败 PORT=%d ERROR=%d\n", 
				nSvrPort, WSAGetLastError());
			break;
		}

		// 开始监听
		if(listen(m_hListenSocket, DEFAULT_LISTEN_BACK_LOG) == SOCKET_ERROR)
		{
			TraceLogError("CTcpIocpServer::Create 监听失败 ERROR=%d!\n", 
				WSAGetLastError());
			break;
		}

		// 获得AcceptEx相关函数指针
		m_lpfnAcceptEx = GetAcceptExPtr(m_hListenSocket);
		m_lpfnGetAcceptExSockAddrs = GetAcceptExSockAddrs(m_hListenSocket);
		if(NULL == m_lpfnAcceptEx || NULL == m_lpfnGetAcceptExSockAddrs)
			break;

		// 创建ACCEPT事件
		m_hAcceptEvent = CreateAcceptEvent(m_hListenSocket);
		if(INVALID_HANDLE_VALUE == m_hAcceptEvent)
		{
			break;
		}

		// 投递ACCEPTEX
		for (int i = 0; i < 100 ; i++)
		{
			OVERLAPPEDPLUS* pOverlap = MallocOverlap(IO_ACCEPT);
			if(NULL != pOverlap)
			{
				if(!AddAcceptEx(pOverlap))
				{
					FreeOverlap(pOverlap);
				}
			}
		}

		// 创建检查线程
		if(!m_CheckThread.StartThread(this))
			break;

		bResult = TRUE;
	} while (FALSE);

	if(!bResult)
	{
		Destroy();
	}
	return bResult;
}

void CTcpIocpServer::Destroy(void)
{
	// 销毁SOCKET套接字
	if(INVALID_SOCKET != m_hListenSocket)
	{
		DestroySocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
	}

	//关闭检查线程
	if(INVALID_HANDLE_VALUE != m_hAcceptEvent)
	{
		WSASetEvent(m_hAcceptEvent);
	}

	//等待Context删除完成
	while ((GetTcpContextCount() > 0 || GetAcceptExCount() > 0))
	{
		//关闭所有已连接SOCKET
		CloseAllContext();

		//关闭所有未连接SOCKET
		CloseAllAcceptExSocket();

		Sleep(5);
	}

	// 等待线程退出
	m_CompletePotrThread.WaitThreadExit();
	m_CheckThread.WaitThreadExit();

	// 销毁完成端口句柄
	if(INVALID_HANDLE_VALUE != m_hIocp)
	{
		DestroyCompletePort(m_hIocp);
		m_hIocp = INVALID_HANDLE_VALUE;
	}

	// 销毁ACCEPT事件
	if(INVALID_HANDLE_VALUE != m_hAcceptEvent)
	{
		DestroyAcceptEvent(m_hAcceptEvent);
		m_hAcceptEvent = INVALID_HANDLE_VALUE;
	}

	// 清空缓存
	m_OverloapCache.Clear();
}

/// 发送数据
uint32_t CTcpIocpServer::Send(SOCKET hSocket, 
	const char* szDataBuffer, uint16_t nDataSize)
{
	// 参数检查
	ASSERT(szDataBuffer);
	ASSERT(nDataSize);
	if(NULL == szDataBuffer || 0 == nDataSize)
		return 0;

	WSABUF loWsaBuff;
	loWsaBuff.len = nDataSize;
	loWsaBuff.buf = (char *)szDataBuffer;

	//写事件不再通知完成断口（详见潘爱民VC内幕）
	uint32_t nNumberOfBytes = 0;

	CCriticalAutoLock loAutoLock(m_oWriteOverlapLock);
	int lResult = WSASend(hSocket, &loWsaBuff,1, (DWORD*)&nNumberOfBytes, 0, 
		&m_oWriteOverlap,NULL);
	return nNumberOfBytes;
}

//=============================================================================
/// 创建完成端口
HANDLE CTcpIocpServer::CreateCompletePort(void)
{
	//创建完成端口
	HANDLE hHandle = INVALID_HANDLE_VALUE;
	hHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hHandle == INVALID_HANDLE_VALUE)
	{
		TraceLogError("CTcpIocpServer::CreateCompletePort 创建完成端口失败 ERROR=%d\n", 
			GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	// 计算线程数量
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	uint32_t nProcessors = si.dwNumberOfProcessors;
	uint32_t nThreadNumber = nProcessors * 2 + 2;

	// 创建工作线程 
	if(!m_CompletePotrThread.StartThread(this, nThreadNumber))
	{
		CloseHandle(hHandle);
		hHandle = INVALID_HANDLE_VALUE;
	}

	return hHandle;
}

/// 销毁完成端口
void CTcpIocpServer::DestroyCompletePort(HANDLE hHandle)
{
	if (INVALID_HANDLE_VALUE != hHandle)
	{
		CloseHandle(hHandle);
		hHandle = INVALID_HANDLE_VALUE;
	}
}

/// 创建SOCKET套接字
SOCKET CTcpIocpServer::CreateSocket(uint16_t nSvrPort)
{
	// 创建SOCKET
	SOCKET hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == hSocket)
	{
		TraceLogError("CTcpIocpServer::CreateSocket 创建SOCKET失败 ERROR=%d\n", 
			nSvrPort, WSAGetLastError());
		return INVALID_SOCKET;
	}

	return hSocket;
}

/// 销毁SOCKET套接字
void CTcpIocpServer::DestroySocket(SOCKET hSocket)
{
	if(INVALID_SOCKET != hSocket)
	{
		closesocket(hSocket);
		hSocket = NULL;
	}
}

/// 创建ACCEPT事件
HANDLE CTcpIocpServer::CreateAcceptEvent(SOCKET hSocket)
{
	HANDLE hEvent = WSACreateEvent();
	if(INVALID_HANDLE_VALUE == hEvent)
	{
		TraceLogError("CTcpIocpServer::CreateAcceptEvent 创建监听事件句柄失败!\n");
		return INVALID_HANDLE_VALUE;
	}

	if(SOCKET_ERROR == WSAEventSelect(hSocket, hEvent, FD_ACCEPT))
	{
		WSACloseEvent(hEvent);
		hEvent = INVALID_HANDLE_VALUE;
		TraceLogError("CTcpIocpServer::CreateAcceptEvent 为侦听SOCKET关联ACCEPT事件失败!\n");
	}

	return hEvent;
}

/// 销毁ACCEPT事件
void CTcpIocpServer::DestroyAcceptEvent(HANDLE hEvent)
{
	if(INVALID_HANDLE_VALUE != hEvent)
	{
		WSACloseEvent(hEvent);
		hEvent = INVALID_HANDLE_VALUE;
	}
}

/// 加载AcceptEx函数指针
LPFN_ACCEPTEX CTcpIocpServer::GetAcceptExPtr(SOCKET hSocket)
{
	// AcceptEx的GUID，用于导出函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	// 使用AcceptEx函数，因为这个是属于WinSock2规范之外的微软另外提供的扩展函数
	// 所以需要额外获取一下函数的指针, 在未取得函数指针的情况下就调用AcceptEx的开销是很大的

	// 获取AcceptEx函数指针
	DWORD dwBytes = 0;
	LPFN_ACCEPTEX pFnAcceptEx = NULL;

	if(SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx,sizeof(GuidAcceptEx), &pFnAcceptEx, sizeof(pFnAcceptEx), 
		&dwBytes, NULL, NULL))  
	{  
		TraceLogError("CTcpIocpServer::GetAcceptExPtr 未能获取函数指针。ERROR=%d\n", 
			WSAGetLastError());
	}
	return pFnAcceptEx;
}

LPFN_GETACCEPTEXSOCKADDRS CTcpIocpServer::GetAcceptExSockAddrs(SOCKET hSocket)
{
	// GetAcceptExSockaddrs的GUID，用于导出函数指针
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS; 

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	DWORD dwBytes = 0;
	LPFN_GETACCEPTEXSOCKADDRS pFnGetAcceptExSockAddrs = NULL;

	if(SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidGetAcceptExSockAddrs, sizeof(GuidGetAcceptExSockAddrs), 
		&pFnGetAcceptExSockAddrs, sizeof(pFnGetAcceptExSockAddrs),   
		&dwBytes, NULL, NULL))  
	{  
		TraceLogError("CTcpIocpServer::GetAcceptExSockAddrs 未能获取函数指针。ERROR=%d\n", 
			WSAGetLastError());
	}
	return pFnGetAcceptExSockAddrs;
}


/// 申请重叠结构内存
OVERLAPPEDPLUS* CTcpIocpServer::MallocOverlap(ENUM_IO_TYPE enIOType)
{
	OVERLAPPEDPLUS* pOverlap = m_OverloapCache.Malloc();
	if(NULL != pOverlap)
	{
		memset(&(pOverlap->m_Overlapped), 0, sizeof(WSAOVERLAPPED));
		pOverlap->m_enIOType = enIOType;
		pOverlap->m_wsaBuffer.buf = pOverlap->m_szBuffer;
		pOverlap->m_wsaBuffer.len = MAX_PACK_BUFFER_SIZE;   
		pOverlap->m_hSocket = INVALID_SOCKET;
		pOverlap->m_nOverlapKey = 0;
	}
	return pOverlap;
}

/// 释放重叠结构内存
void CTcpIocpServer::FreeOverlap(OVERLAPPEDPLUS* pOverlap)
{
	if(NULL != pOverlap)
	{
		m_OverloapCache.Free(pOverlap);
	}
}

/// 添加AcceptEx队列
BOOL CTcpIocpServer::AddAcceptEx(OVERLAPPEDPLUS* pOverlap)
{
	// 参数检查
	ASSERT(pOverlap);
	if(NULL == pOverlap)
		return FALSE;

	// 创建Socket
	SOCKET hAcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == hAcceptSocket)
	{
		TraceLogError("CTcpIocpServer::AddAcceptEx 创建SOCKET句柄失败 ERROR=%d\n", 
			WSAGetLastError());
		return FALSE;
	}

	// 自动继承m_hSocket的属性
	setsockopt(hAcceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
		(char *)&m_hListenSocket, sizeof(m_hListenSocket));

	// 申请OverlapPlus
	pOverlap->m_hSocket = hAcceptSocket;
	pOverlap->m_enIOType = IO_ACCEPT;

	{
		// 添加至队列
		CCriticalAutoLock loAutoLock(m_AcceptExListLock);
		POSITION position = m_AcceptExList.AddTail(pOverlap);
		// 设置OverlapKey(必须，方便快速删除用)
		pOverlap->m_nOverlapKey = (uint64_t)position;
	}

	// 接受连接请求
	DWORD dwBytes = 0;
	BOOL bIsSuccess = m_lpfnAcceptEx(m_hListenSocket, hAcceptSocket,
		pOverlap->m_szBuffer, 
		pOverlap->m_wsaBuffer.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
		(LPWSAOVERLAPPED)pOverlap);

	if (!bIsSuccess && WSAGetLastError() != ERROR_IO_PENDING)
	{
		closesocket(hAcceptSocket);
		hAcceptSocket = INVALID_SOCKET;

		TraceLogError("CTcpIocpServer::AddAcceptEx AcceptEx()调用失败 ERROR=%d\n", 
			WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

/// 删除AcceptEx队列
BOOL CTcpIocpServer::RemoveAcceptEx(OVERLAPPEDPLUS* pOverlap)
{
	// 参数检查
	ASSERT(pOverlap);
	if(NULL == pOverlap)
		return FALSE;

	CCriticalAutoLock loAutoLock(m_AcceptExListLock);

	POSITION pos = (POSITION)pOverlap->m_nOverlapKey;
	m_AcceptExList.RemoveAt(pos);
	return TRUE;
}

/// 关闭AcceptEx队列中的全部SOCKET
void CTcpIocpServer::CloseAllAcceptExSocket(void)
{
	CCriticalAutoLock loAutoLock(m_AcceptExListLock);

	//关闭所有未连接SOCKET
	POSITION pos = m_AcceptExList.GetHeadPosition();
	while(NULL != pos)
	{
		OVERLAPPEDPLUS* pOverlap = m_AcceptExList.GetNext(pos);
		if(INVALID_SOCKET != pOverlap->m_hSocket)
		{
			closesocket(pOverlap->m_hSocket);
			pOverlap->m_hSocket = INVALID_SOCKET;
		}
	}
}

/// 检查AcceptEx队列的超时数据
void CTcpIocpServer::CheckAcceptExTimeout(void)
{
	CCriticalAutoLock loAutoLock(m_AcceptExListLock);

	int32_t nResult  = 0;
	uint32_t nOptVal = 0;
	int32_t nOptLen  = 0;

	POSITION pos = m_AcceptExList.GetHeadPosition();
	while(NULL != pos)
	{
		OVERLAPPEDPLUS* pOverlap = m_AcceptExList.GetNext(pos);

		//若用户已建立连接，但在30秒内没发包，则断开连接
		nOptLen = sizeof(nOptVal);

		nResult = getsockopt(pOverlap->m_hSocket, SOL_SOCKET, SO_CONNECT_TIME, 
			(char*)&nOptVal, &nOptLen);
		if ((SOCKET_ERROR == nResult) || ((nOptVal != 0xFFFFFFFF) && (nOptVal > 40)))
		{	
			//关闭连接
			if (pOverlap->m_hSocket != INVALID_SOCKET)
			{
				TraceLogWarn("CTcpIocpServer 连接超时 SOCKET=%d TIME=%d(s)\n", 
					pOverlap->m_hSocket, nOptVal);
				
				closesocket(pOverlap->m_hSocket);
				pOverlap->m_hSocket = INVALID_SOCKET;
			}
		}
	}
}

/// 获得AcceptEx队列尺寸
uint32_t CTcpIocpServer::GetAcceptExCount(void) const
{
	CCriticalAutoLock loAutoLock(m_AcceptExListLock);
	return m_AcceptExList.GetCount();
}

/// 添加Context
BOOL CTcpIocpServer::AddTcpContext(CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	CCriticalAutoLock loAutoLock(m_ContextListLock);
	POSITION pos = m_ContextList.AddTail(pContext);
	pContext->m_i64ContextKey = (uint32_t)pos;
	return TRUE;
}

/// 删除Context
BOOL CTcpIocpServer::RemoveTcpContext(CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

    BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// 通过KEY值取出Context指针（该指针有可能已经失效，请勿修改该指针)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// 判断指针地址是否相同
	if(pTempContext == pContext)
	{
		// 判断SOCKET句柄是否相同
		if(pTempContext->m_hSocket == pContext->m_hSocket)
		{
			m_ContextList.RemoveAt(pos);
			m_pEvent->DestroyContext(pContext);
			bResult = TRUE;
		}
	}
	return bResult;
}

/// 检查Context是否有效
BOOL CTcpIocpServer::ContextIsValid(const CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

    BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// 通过KEY值取出Context指针（该指针有可能已经失效，请勿修改该指针)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// 判断指针地址是否相同
	if(pTempContext == pContext)
	{
		// 判断SOCKET句柄是否相同
		if(pTempContext->m_hSocket == pContext->m_hSocket)
			bResult = TRUE;
	}
	return bResult;
}

BOOL CTcpIocpServer::ResetContext(CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

    BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// 通过KEY值取出Context指针（该指针有可能已经失效，请勿修改该指针)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// 判断指针地址是否相同
	if(pTempContext == pContext)
	{
		// 判断SOCKET句柄是否相同
		if(pTempContext->m_hSocket == pContext->m_hSocket 
			&& pTempContext->m_oSocketAddr.sin_addr.s_addr 
			== pContext->m_oSocketAddr.sin_addr.s_addr)
		{
			pContext->ResetContext();
			bResult = TRUE;
		}
	}
	return bResult;
}

/// 关闭所有Context
void CTcpIocpServer::CloseAllContext(void)
{
	//关闭所有已连接SOCKET
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		closesocket(pContext->m_hSocket);
	}
}

/// 关闭无效链接
void CTcpIocpServer::CheckInvalidContext(void)
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	//检查无效连接
	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		if(!pContext->CheckValid())
		{
			closesocket(pContext->m_hSocket);
			pContext->m_hSocket = INVALID_SOCKET;
		}
	}
}

/// 获得连接数量
uint32_t CTcpIocpServer::GetTcpContextCount(void) const
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);
	return m_ContextList.GetCount();
}

//=============================================================================
/// 接收数据
BOOL CTcpIocpServer::DealRecvData(uint32_t nRecvDataLen, 
	CTcpContext *pContext, OVERLAPPEDPLUS* pOverlapPlus)
{
	// 参数检查
	ASSERT(nRecvDataLen);
	ASSERT(pContext);
	ASSERT(pOverlapPlus);
	if(0 == nRecvDataLen || NULL == pContext || NULL == pOverlapPlus)
		return FALSE;

	m_pEvent->OnRecvData(pOverlapPlus->m_szBuffer, nRecvDataLen, pContext);

	return TRUE;
}

/// 完成端口线程函数
void CTcpIocpServer::CompletePortFunc(void)
{
	DWORD dwNumberOfBytes = 0;
	CTcpContext *lpTcpContext = NULL;
	CTcpContext* lpNewContext = NULL;

	LPWSAOVERLAPPED lpOverlapped = NULL;
	OVERLAPPEDPLUS *lpOverlapPlus = NULL;

	int32_t nRet = 0;
	uint32_t nFlags = 0;

	struct sockaddr_in* LocalAddr = NULL;
	struct sockaddr_in* RemoteAddr = NULL;
	int32_t LocalAddrLen = 0;
	int32_t RemoteAddrLen = 0;

	while (TRUE)
	{
		lpOverlapped = NULL;
		BOOL bResult = GetQueuedCompletionStatus(m_hIocp, &dwNumberOfBytes, 
			(ULONG *)&lpTcpContext, &lpOverlapped, INFINITE);
		if (NULL == lpOverlapped)
		{
			continue;
		}

		if(!bResult)
		{
			//PostQueuedCompletionStatus发过来一个空的单句柄数据，表示线程要退出了
			if (NULL == lpTcpContext)
			{
				break;
			}

			lpOverlapPlus = (OVERLAPPEDPLUS *)lpOverlapped;
			switch(lpOverlapPlus->m_enIOType)
			{
			case IO_WRITE:
				{	
					//注意：在这里不能访问lpTcpContext
					FreeOverlap(lpOverlapPlus);
					break;
				}
			case IO_READ:
				{
					// 接收数据长度为零,释放lpOverlapPlus和lpTcpContext    
					if (0 == dwNumberOfBytes)
					{
						RemoveTcpContext(lpTcpContext);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					//分析处理数据
					if (!DealRecvData(dwNumberOfBytes, lpTcpContext, lpOverlapPlus))
					{
						//释放lpOverlapPlus和lpTcpContext
						RemoveTcpContext(lpTcpContext);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					nRet = WSARecv(lpTcpContext->m_hSocket, &lpOverlapPlus->m_wsaBuffer,
						1, &dwNumberOfBytes, (DWORD*)&nFlags, (OVERLAPPED*)lpOverlapPlus, NULL);
					if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
					{
						//释放lpOverlapPlus和lpTcpContext
						RemoveTcpContext(lpTcpContext);
						FreeOverlap(lpOverlapPlus);						
					}

					break;
				}
			case IO_ACCEPT:
				{
					if(!RemoveAcceptEx(lpOverlapPlus))
					{
						continue;
					}

					//若已投递的ACCEPT数小于100，则补充ACCEPT请求
					if (GetAcceptExCount() < 100)
					{
						OVERLAPPEDPLUS* pOverlap = MallocOverlap(IO_ACCEPT);
						if(NULL != pOverlap)
						{
							if(!AddAcceptEx(pOverlap))
							{
								FreeOverlap(pOverlap);
							}
						}
					}

					if (0 == dwNumberOfBytes)
					{
						closesocket(lpOverlapPlus->m_hSocket);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					//取得对方IP
					m_lpfnGetAcceptExSockAddrs(lpOverlapPlus->m_szBuffer,
						lpOverlapPlus->m_wsaBuffer.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
						sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
						(SOCKADDR **)&LocalAddr, &LocalAddrLen,
						(SOCKADDR **)&RemoteAddr, &RemoteAddrLen);

					//申请单句柄内容,然后把socket和完成端口关联起来
					lpNewContext = m_pEvent->CreateContext();
					if (!lpNewContext)
					{
						closesocket(lpOverlapPlus->m_hSocket);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					lpNewContext->m_hSocket = lpOverlapPlus->m_hSocket;
					lpNewContext->m_oSocketAddr.sin_addr.s_addr = RemoteAddr->sin_addr.s_addr;
					lpNewContext->m_oSocketAddr.sin_port = RemoteAddr->sin_port;

					//添加到保存客户端连接的列表里
					AddTcpContext(lpNewContext);

					//将新建立的套接字关联到完成端口
					if (!CreateIoCompletionPort((HANDLE)lpNewContext->m_hSocket,\
						m_hIocp, (DWORD)lpNewContext, 0))
					{
						RemoveTcpContext(lpNewContext);
						FreeOverlap(lpOverlapPlus);							
						continue;
					}

					//分析处理数据
					if (!DealRecvData(dwNumberOfBytes, lpNewContext, lpOverlapPlus))
					{
						RemoveTcpContext(lpNewContext);
						FreeOverlap(lpOverlapPlus);
						continue;
					}

					lpOverlapPlus->m_enIOType = IO_READ;	
					nRet = WSARecv(lpNewContext->m_hSocket, 	&lpOverlapPlus->m_wsaBuffer,
						1, &dwNumberOfBytes, (DWORD*)&nFlags, (OVERLAPPED*)lpOverlapPlus, NULL);
					if (nRet == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
					{
						RemoveTcpContext(lpNewContext);
						FreeOverlap(lpOverlapPlus);
					}					
					break;
				}
			}
		}
	}
}

/// 连接检查线程函数
void CTcpIocpServer::ConnectCheckFunc(void)
{
	uint32_t nTimeNow = 0;
	uint32_t nLastCheckTime = GetTickCount();

	int32_t nResult = 0;
	int32_t nOptlen = 0;
	uint32_t nOptval = 0;
	//int32_t nAcceptCount = 0;

	OVERLAPPEDPLUS *lpOverlapPlus = NULL;
	CTcpContext* lpTcpContext = NULL;
	uint32_t nWaitResult = 0;

	while(INVALID_SOCKET != m_hListenSocket)
	{
		nWaitResult = WSAWaitForMultipleEvents(1, &m_hAcceptEvent, FALSE, 1000, FALSE);
		if (INVALID_SOCKET == m_hListenSocket)
		{
			break;
		}

		if (WSA_WAIT_EVENT_0 == nWaitResult || GetAcceptExCount() < 2) //已无连接可用
		{
			WSAResetEvent(m_hAcceptEvent);

			//投递AcceptEx请求			
			for(uint32_t nAcceptCount = 0; nAcceptCount < 100; nAcceptCount++)
			{
				OVERLAPPEDPLUS* pOverlap = MallocOverlap(IO_ACCEPT);
				if(NULL != pOverlap)
				{
					if(!AddAcceptEx(pOverlap))
					{
						FreeOverlap(pOverlap);
					}
				}
			}
		}

		//10秒检查一次(预防拒绝服务攻击)
		nTimeNow = GetTickCount();
		if (nTimeNow > nLastCheckTime + 10000)
		{
			// 检查空连接(用户已建立连接，但在规定时间内没发包)
			CheckAcceptExTimeout();
			
			//检查无效连接
			CheckInvalidContext();

			nLastCheckTime = nTimeNow;
		}
	}
}

/// 完成端口线程
unsigned int CTcpIocpServer::CompletePortThread(void *pParam)
{
	CTcpIocpServer* pThis = (CTcpIocpServer*)pParam;
	pThis->CompletePortFunc();
	return 0;
}

/// 连接检查线程
unsigned int CTcpIocpServer::ConnectCheckThread(void* pParam)
{
	CTcpIocpServer* pThis = (CTcpIocpServer*)pParam;
	pThis->ConnectCheckFunc();
	return 0;
}

#endif
