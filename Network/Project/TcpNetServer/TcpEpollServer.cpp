#include "TcpEpollServer.h"
#include "DebugTrace.h"
#include "NetworkAPI.h"

#ifndef _WIN32
//=============================================================================
//设定文件描述符的阻塞位
//函数参数：int sockfd，目标socket
// int value，当为0时描述符被设为阻塞的，当为非0值时描述符被设为非阻塞的
static BOOL SetNonblock(int sockfd, BOOL bIsNonBlock)
{
	int oldflags = fcntl(sockfd, F_GETFL, 0);
	if(bIsNonBlock)
	{
		oldflags |= O_NONBLOCK;
	}
	else{
		oldflags &= ~O_NONBLOCK;
	}

	int result=fcntl(sockfd, F_SETFL, oldflags);
	if (result == -1)
	{
		TraceLogError("CTcpEpollServer SetNonblock 调用失败 ERROR=%s!\n",
			strerror(errno));
		return FALSE;
	}

	//Linux 假设有一半的发送/接收缓冲区是用来处理内核结构,
	//因此, 系统控制的缓冲区是网络可访问的缓冲区的两倍.
	int liBufSize = 8192;
	socklen_t lilen = 4;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &liBufSize, lilen);
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &liBufSize, lilen);

	return TRUE;
}

/// 数据包缓存数量
#define EPOLL_TCP_PACKET_CACHE_SIZE		10240

//=============================================================================
CTcpEpollServer::CTcpEpollServer(void)
	: m_hEpollHandle(-1)
	, m_EpollWaitThread(&EpollWaitThread)
	, m_CheckThread(&ConnectCheckThread)
	, m_PacketCache(EPOLL_TCP_PACKET_CACHE_SIZE)
{
}


CTcpEpollServer::~CTcpEpollServer(void)
{
	Destroy();
}

/// 创建TCP服务器
BOOL CTcpEpollServer::Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent,
	ENUM_ENCRYPT_TYPE enType)
{
	// 参数检查
	ASSERT(pSvrEvent);
	ASSERT(nSvrPort);
	if(NULL == pSvrEvent || 0 == nSvrPort)
		return FALSE;

	BOOL bResult = FALSE;
	m_pTcpEvent = pSvrEvent;
	SetEncryptType(enType);

	do
	{
		// 创建EPOLL
		uint32_t nProcessors = sysconf(_SC_NPROCESSORS_CONF);
		nProcessors = nProcessors == 0 ? 1 : nProcessors;
		uint32_t nThreadNumber = nProcessors * 2 + 2;

		m_hEpollHandle = CreateEpoll(nThreadNumber);
		if(-1 == m_hEpollHandle)
			break;

		// 创建SOCKET
		m_hListenSocket = CreateSocket(nSvrPort);
		if(INVALID_SOCKET == m_hListenSocket)
			break;

		m_ListenContext.m_hSocket = m_hListenSocket;

		// SOCKET和EPOLL绑定
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;			//读事件 ET模式
		ev.data.ptr = (void*)&m_ListenContext;

		if (epoll_ctl(m_hEpollHandle, EPOLL_CTL_ADD, m_hListenSocket, &ev) < 0)
		{
			TraceLogError("CTcpEpollServer::Create 绑定Epoll失败 ERROR=%s!\n",
				strerror(errno));
			break;
		}

		// 创建Epoll线程
		if(!m_EpollWaitThread.StartThread(this, nThreadNumber))
			break;

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

void CTcpEpollServer::Destroy(void)
{
	// 关闭SOCKET
	if(m_hListenSocket != INVALID_SOCKET)
	{
		DestroySocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		m_ListenContext.m_hSocket = INVALID_SOCKET;
	}

	//关闭EPOLL句柄
	if (m_hEpollHandle != -1)
	{
		DestroyEpoll(m_hEpollHandle);
		m_hEpollHandle = -1;
	}

	// 关闭所有SOCKET
	//关闭所有已连接SOCKET
	while (GetTcpContextCount() > 0)
	{
		CloseAllContext();
	}

	// 等待线程退出
	m_EpollWaitThread.WaitThreadExit();
	m_CheckThread.WaitThreadExit();
}

/// 发送数据
uint32_t CTcpEpollServer::Send(SOCKET hSocket, const char* szDataBuffer,
	uint16_t nDataSize)
{
	// 参数检查
	ASSERT(szDataBuffer);
	ASSERT(nDataSize);
	if(NULL == szDataBuffer || 0 == nDataSize)
		return 0;

	tcp_packet_t* pPacket = m_SendQueue.GetFreePacket();
	if(NULL != pPacket)
	{
		pPacket->m_nPackSize = m_SendPackBuffer.Pack(szDataBuffer, nDataSize, 
			pPacket->m_szPackBuffer, MAX_PACK_BUFFER_SIZE);
		if(pPacket->m_nPackSize > 0)
		{
			// 发送数据包
			m_SendQueue.SendPacket(hSocket, pPacket);
			return nDataSize;
		}
	}
	return 0;

}

//=============================================================================
/// 创建Epoll
int CTcpEpollServer::CreateEpoll(uint32_t nThreadNumber)
{
	//创建EPOLL
	int hEpollHandle = epoll_create(nThreadNumber);
	if (hEpollHandle == -1)
	{
		TraceLogError("CTcpEpollServer::CreateEpoll 创建Epoll失败 ERROR=%s!\n",
			strerror(errno));
	}
	return hEpollHandle;
}

/// 销毁Epoll
void CTcpEpollServer::DestroyEpoll(int hEpollHandle)
{
	//关闭EPOLL句柄
	if (hEpollHandle != -1)
	{
		close(hEpollHandle);
		hEpollHandle = -1;
	}
}

BOOL CTcpEpollServer::EpollAcceptSocket(SOCKET hSocket, const sockaddr_in& SockAddr)
{
	if(INVALID_SOCKET == hSocket)
		return FALSE;

    // 设置程非阻塞式SOCKET
    if (!SetNonblock(hSocket, TRUE))
    {
        return FALSE;
    }

	// 回调上层创建context
	CTcpContext *pContext = CreateContext();
	if (pContext == NULL)
	{
		return FALSE;
	}

	// Context 赋值
	pContext->m_hSocket = hSocket;
	memcpy(&pContext->m_oSocketAddr, &SockAddr, sizeof(sockaddr_in));

	// 添加至队列
	AddTcpContext(pContext);

	struct epoll_event ev;
	ev.events	= EPOLLIN | EPOLLOUT | EPOLLET;  //ET模式+读事件+写事件
	ev.data.ptr = (void*)pContext;
	if (epoll_ctl(m_hEpollHandle, EPOLL_CTL_ADD, hSocket, &ev) < 0)
	{
		RemoveTcpContext(pContext);
		DestroyContext(pContext);
		return FALSE;
	}

	return TRUE;
}

/// 创建SOCKET套接字
SOCKET CTcpEpollServer::CreateSocket(uint16_t nSvrPort)
{
	SOCKET hSocket = INVALID_SOCKET;

	BOOL bIsSucceed = FALSE;
	do
	{
		// 创建监听Socket
		SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if (hSocket == INVALID_SOCKET)
		{
			TraceLogError("CTcpEpollServer::CreateSocket 创建SOCKET失败 ERROR=%s!\n",
				strerror(errno));
			break;
		}

		//设置Socket绑定的服务器IP地址、端口可以重用。
		//防止服务器在发生意外时，端口未被释放，造成无法启动；设置后可以重新使用了。
		unsigned int optval = 0x1;
		setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(unsigned int));

		//设置程非阻塞式SOCKET
		if (!SetNonblock(hSocket,TRUE))
		{
			TraceLogError("CTcpEpollServer::CreateSocket SetNonblock失败 SOCKET=%d!\n",
				hSocket);
			break;
		}

		//绑定端口
		sockaddr_in  addr;
		memset(&addr,0,sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(nSvrPort);
		if (SOCKET_ERROR == bind(hSocket,(sockaddr *)&addr, sizeof(sockaddr_in)))
		{
			TraceLogError("CTcpEpollServer::CreateSocket 绑定端口失败 PORT=%d ERROR=%s!\n",
				nSvrPort, strerror(errno));
			break;
		}

		//开始侦听
		if (listen(hSocket, 100) == SOCKET_ERROR)
		{
			TraceLogError("CTcpEpollServer::CreateSocket 监听失败 ERROR=%s!\n",
				strerror(errno));
			break;
		}

		bIsSucceed = TRUE;
	} while (FALSE);

	if(!bIsSucceed)
	{
		if(INVALID_SOCKET != hSocket)
		{
			close(hSocket);
			hSocket = INVALID_SOCKET;
		}
	}
	return hSocket;
}

/// 销毁SOCKET套接字
void CTcpEpollServer::DestroySocket(SOCKET hSocket)
{
	if (hSocket != INVALID_SOCKET)
	{
		shutdown(hSocket, SHUT_RDWR);//关闭连接读读和写
		close(hSocket);

		hSocket = INVALID_SOCKET;
	}
}

/// 完成端口线程函数
void CTcpEpollServer::EpollWaitFunc(void)
{
	int32_t nEventCount = 0;
	struct epoll_event EpollEvent[128];	//epoll事件对象

	while(-1 != m_hEpollHandle)
	{
		nEventCount = epoll_wait(m_hEpollHandle, EpollEvent, 128, 500);
        if (nEventCount == -1)
        {
            continue;
        }

		for (uint32_t nIndex = 0; nIndex < nEventCount; ++nIndex)
		{
		    struct epoll_event* pEpollEvent = &EpollEvent[nIndex];
			CTcpContext* lpContext = (CTcpContext*)pEpollEvent->data.ptr;
			SOCKET hSocket = lpContext->m_hSocket;

			if (hSocket == m_hListenSocket)
			{
				SOCKET hAcceptSocket = INVALID_SOCKET;
				struct sockaddr_in oAddr;
				socklen_t nAddrSize = sizeof(sockaddr_in);

				do
				{
					uint32_t nAddrLen = sizeof(sockaddr_in);
					hAcceptSocket = accept(m_hListenSocket, (sockaddr*)&oAddr, &nAddrSize);
					if(INVALID_SOCKET == hAcceptSocket)
					{
						//当端口是非阻塞时，accept返回－1，并设置errno为EAGAIN，此时应该继续接受连接
						//当端口是非阻塞时，非阻塞套接字上不能立即完成的操作返回，设置errno为EWOULDBLOCK，此时应该继续接受连接
						if (errno != EAGAIN && errno != EWOULDBLOCK)
						{
						}
						break;
					}

					if(!EpollAcceptSocket(hAcceptSocket, oAddr))
					{
						close(hAcceptSocket);
						hAcceptSocket = INVALID_SOCKET;
					}
				}while(hAcceptSocket != INVALID_SOCKET);
			}
			else
			{
				if (pEpollEvent->events & EPOLLIN)
				{
					int32_t nRecvSize = 0;
					char szRecvBuff[MAX_PACK_BUFFER_SIZE];

					do
					{
						nRecvSize = recv(hSocket, szRecvBuff, MAX_PACK_BUFFER_SIZE, MSG_NOSIGNAL);
						if(nRecvSize > 0)
						{
							DealRecvData(szRecvBuff, nRecvSize, lpContext);
						}
						//接收数据长度为0，说明连接断开了
						else if(nRecvSize == 0)
						{
							// 关闭连接
							RemoveTcpContext(lpContext);
							break;
						}
						else
						{
							//当端口是非阻塞时，recv返回－1，并设置errno为EAGAIN，此时应该是数据读完了，
							//当端口是非阻塞时，非阻塞套接字上不能立即完成的操作返回，设置errno为EWOULDBLOCK，此时应该是数据读完了
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								// 关闭连接
								RemoveTcpContext(lpContext);
							}
						}
					}while(nRecvSize > 0);
				}

				if(pEpollEvent->events & EPOLLOUT)
				{
					m_SendQueue.SendPacket(hSocket);
				}
			}
		}
	}
}

/// 连接检查线程函数
void CTcpEpollServer::ConnectCheckFunc(void)
{
	uint64_t i64CheckTime = GetSystemTime();
	while(INVALID_SOCKET != m_hListenSocket)
	{
		Sleep(300);

		if (GetSystemTime() > i64CheckTime + 30000)
		{
			// 关闭无效连接
			CheckInvalidContext();

			i64CheckTime = GetSystemTime();
		}
	}
}

/// 完成端口线程
unsigned int CTcpEpollServer::EpollWaitThread(void *pParam)
{
	CTcpEpollServer* pThis = (CTcpEpollServer*)pParam;
	pThis->EpollWaitFunc();
	return 0;
}

/// 连接检查线程
unsigned int CTcpEpollServer::ConnectCheckThread(void* pParam)
{
	CTcpEpollServer* pThis = (CTcpEpollServer*)pParam;
	pThis->ConnectCheckFunc();
	return 0;
}

#endif //_WIN32
