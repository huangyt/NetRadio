#include "TcpNetTrans.h"
#include "DebugTrace.h"
#include "NetworkAPI.h"

//=============================================================================
#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

//=============================================================================
#define DEFAULT_ENCRYPT_KEY			"24698B5E-69E9-485B-BAB7-FC685D4AEFCC"
#define DEFAULT_ENCRYPT_KEY_SIZE	16

//=============================================================================
CTcpNetTrans::CTcpNetTrans(void)
	: m_CheckStateThread(&TcpCheckStateThread)
	, m_SendThread(&TcpSendThread)
	, m_RecvThread(&TcpRecvThread)
{
	m_pNetEvent = NULL;
	m_hSocket = INVALID_SOCKET;

	m_enTcpState = ENUM_TCP_STATE_NONE;
	m_enEncryptType = ENUM_ENCRYPT_AES;

	memset(m_szSvrAddr, 0, sizeof(m_szSvrAddr));
	m_nSvrPort = 0;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}


CTcpNetTrans::~CTcpNetTrans(void)
{
	Close();

#ifdef _WIN32
	WSACleanup();
#endif
}

BOOL CTcpNetTrans::Open(ITcpNetEvent* pNetEvent, uint16_t nPort)
{
	// 参数检查
	ASSERT(pNetEvent);
	if(NULL == pNetEvent)
	{
		return FALSE;
	}

	// 重复创建检查
	if(INVALID_SOCKET != m_hSocket)
	{
		return FALSE;
	}

	m_pNetEvent = pNetEvent;
	BOOL bResult = FALSE;

	do 
	{
		// 创建套接字
		m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(INVALID_SOCKET == m_hSocket)
		{
			TraceLogError("CTcpNetTrans::Open 创建SOCKET套接字失败 Error=%d\n", 
				WSAGetLastError());
			break;
		}

		// 绑定端口
		if(0 != nPort)
		{
			SOCKADDR_IN addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_port = htons(nPort);

			if (SOCKET_ERROR == bind(m_hSocket, (LPSOCKADDR)&addr,sizeof(addr)))
			{
				TraceLogError("CTcpNetTrans::Open SOCKET绑定端口%d失败\n", nPort);
				break;
			}
		}

		// 创建PackBuffer
		if(!m_PackBuffer.Create(DEFAULT_ENCRYPT_KEY, DEFAULT_ENCRYPT_KEY_SIZE))
			break;

		// 创建状态检查线程
		if(!m_CheckStateThread.StartThread(this, 1))
			break;
		// 创建发送线程
		if(!m_SendThread.StartThread(this, 1))
			break;
		// 创建接收线程
		if(!m_RecvThread.StartThread(this, 1))
			break;

		// 设置状态
		SetTcpState(ENUM_TCP_STATE_OPEN);

		bResult = TRUE;

	} while (FALSE);

	if(!bResult)
	{
		Close();
	}

	return bResult;
}

BOOL CTcpNetTrans::IsOpen(void) const
{
	return (INVALID_SOCKET != m_hSocket);
}

void CTcpNetTrans::Close(void)
{
	if(INVALID_SOCKET != m_hSocket)
	{
		// 设置状态
		SetTcpState(ENUM_TCP_STATE_CLOSE);

		// 关闭套接字
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;

		// 等待接收线程退出
		m_RecvThread.WaitThreadExit();

		// 等待发送线程退出
		m_SendEvent.SetEvent();
		m_SendThread.WaitThreadExit();

		// 等待状态检查线程退出
		m_CheckStateThread.WaitThreadExit();

		{
			// 清空发送队列
			CCriticalAutoLock loAutoLock(m_oSendQueueLock);
			while(m_SendPacketQueue.GetCount() > 0)
			{
				tcp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		{
			// 清空接收队列
			CCriticalAutoLock loAutoLock(m_oRecvQueueLock);
			while(m_RecvPacketQueue.GetCount() > 0)
			{
				tcp_packet_t* pPacket = m_RecvPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		// 销毁PackBuffer
		m_PackBuffer.Destroy();
	}
}

BOOL CTcpNetTrans::SetEncryptType(ENUM_ENCRYPT_TYPE enType)
{
	m_enEncryptType = enType;
	return (m_enEncryptType != ENUM_ENCRYPT_NONE);
}

BOOL CTcpNetTrans::SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize)
{
	return m_PackBuffer.SetEncryptKey(szEncryptKey, nKeySize);
}

BOOL CTcpNetTrans::Connet(const char* szSvrAddr, uint16_t nSvrPort)
{
	// 参数检查
	ASSERT(szSvrAddr != NULL);
	ASSERT(nSvrPort != 0);
	if(NULL == szSvrAddr || nSvrPort == 0)
		return FALSE;

	if(!IsOpen())
		return FALSE;

	sockaddr_in addr;
	memset(&addr, 0, sizeof(sockaddr_in));

	// 判断参数是否为IP地址
	if(IsIPAddr(szSvrAddr))
	{
		// szSvrAddr为ip地址
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(szSvrAddr);
		addr.sin_port = htons(nSvrPort);
	}
	else
	{
		// szSvrAddr为名称
		struct hostent*	pHostent;
		pHostent = gethostbyname(szSvrAddr);
		if (pHostent == NULL)
		{
			TraceLogError("CTcpNetTrans::Connet:gethostbyname error: %s\n", 
				szSvrAddr);
			return FALSE;
		}

		memcpy(&addr.sin_addr, pHostent->h_addr, pHostent->h_length);
		addr.sin_family = pHostent->h_addrtype;
		addr.sin_port = htons(nSvrPort);
	}

	if (connect(m_hSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		uint32_t nErrorNo = 0;
#ifdef WIN32
		nErrorNo = WSAGetLastError();
#else
		nErrorNo = errno;
#endif
		TraceLogError("CTcpNetTrans::Connet:连接指定地址(%s:%d)失败!失败代码：%d\n", 
			szSvrAddr, nSvrPort, nErrorNo);
		return FALSE;
	}

	strncpy(m_szSvrAddr, szSvrAddr, MAX_URL_ADDR_SIZE);
	m_nSvrPort = nSvrPort;

	// 设置状态
	SetTcpState(ENUM_TCP_STATE_CONNECT);

	return TRUE;
}

BOOL CTcpNetTrans::GetConnectAddr(char* szSvrAddr, uint16_t& nSvrPort)
{
	ASSERT(szSvrAddr);
	if(NULL == szSvrAddr)
		return FALSE;

	strncpy(szSvrAddr, m_szSvrAddr, MAX_URL_ADDR_SIZE);
	nSvrPort = m_nSvrPort;

	return TRUE;
}

BOOL CTcpNetTrans::IsConnet(void) const
{
	return (m_enTcpState == ENUM_TCP_STATE_CONNECT);
}

uint32_t CTcpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize)
{
	if(!IsConnet())
	{
		ASSERT(FALSE);
		return 0;
	}

	// 分配数据包
	tcp_packet_t* pPacket = m_PacketCache.MallocPacket();
	ASSERT(pPacket);
	if(NULL != pPacket)
	{
		// 打包
		pPacket->m_nPackSize = m_PackBuffer.Pack(szBuffer, nBufferSize, 
			pPacket->m_szPackBuffer, MAX_PACK_BUFFER_SIZE, m_enEncryptType);

		if(pPacket->m_nPackSize > 0)
		{
			{
				// 添加至发送队列
				CCriticalAutoLock loAutoLock(m_oSendQueueLock);
				m_SendPacketQueue.AddTail(pPacket);
			}

			// 触发发送事件
			m_SendEvent.SetEvent();
			return nBufferSize;
		}
		else
		{
			// 释放数据包
			m_PacketCache.FreePacket(pPacket);
			return 0;
		}
	}
	return 0;
}

uint32_t CTcpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize)
{
	// 参数检查
	ASSERT(szBuffer);
	ASSERT(nBufferSize >= MAX_PACK_BUFFER_SIZE);
	if(NULL == szBuffer || nBufferSize < MAX_PACK_BUFFER_SIZE)
		return 0;

	uint16_t nRecvSize = 0;
	uint16_t nTimeStamp = 0;
	BOOL m_bIsComplete = FALSE;

	// 取出数据包
	CCriticalAutoLock loAutoLock(m_oRecvQueueLock);

	// 当前数据包
	tcp_packet_t* pPacket = m_RecvPacketQueue.GetHead();
	if(NULL != pPacket)
	{
		int32_t nPackSize = m_PackBuffer.UnPack(pPacket->m_szPackBuffer, 
			pPacket->m_nPackSize, szBuffer, nRecvSize, nTimeStamp);

		BOOL bIsFreePacket = TRUE;
		if(nPackSize > 0)
		{
			// 判断是否全部解完
			if(nPackSize < pPacket->m_nPackSize)
			{
				// 内存拷贝
				char* pBuffer = pPacket->m_szPackBuffer + nPackSize;
				uint32_t nSize = pPacket->m_nPackSize - nPackSize;

				memmove(pPacket->m_szPackBuffer, pBuffer, nSize);
				pPacket->m_nPackSize = nSize;

				bIsFreePacket = FALSE;

				// 事件触发（此时还有数据包尚未解完）
				m_EventQueue.PushEvent(TCP_NET_EVENT_RECV);
				m_CheckEvent.SetEvent();
			}
		}

		if(bIsFreePacket)
		{
			// 从队列中删除
			m_RecvPacketQueue.RemoveHead();
			// 回收内存
			m_PacketCache.FreePacket(pPacket);
		}

		return nRecvSize;
	}

	return 0;
}

//=============================================================================
/// 设置TCP状态
ENUM_TCP_STATE CTcpNetTrans::SetTcpState(ENUM_TCP_STATE enNewState)
{
	ENUM_TCP_STATE enOldState = m_enTcpState;

	if(enNewState != m_enTcpState)
	{
		m_enTcpState = enNewState;

		switch(m_enTcpState)
		{
		case ENUM_TCP_STATE_OPEN:			///< 打开
			m_EventQueue.PushEvent(TCP_NET_EVENT_OPEN);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_TCP_STATE_CLOSE:			///< 关闭
			m_EventQueue.PushEvent(TCP_NET_EVENT_CLOSE);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_TCP_STATE_CONNECT:		///< 连接成功
			m_EventQueue.PushEvent(TCP_NET_EVENT_CONNECT);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_TCP_STATE_DISSCONNECT:	///< 连接断开
			if(enOldState == ENUM_TCP_STATE_CONNECT)
			{
				m_EventQueue.PushEvent(TCP_NET_EVENT_DISSCONNECT);
				m_CheckEvent.SetEvent();
			}
			break;
		default:
			break;
		}
	}
	return enOldState;
}

/// 获得TCP状态
ENUM_TCP_STATE CTcpNetTrans::GetTcpState(void) const
{
	return m_enTcpState;
}

/// TCP收包处理函数
void CTcpNetTrans::TcpRecvThreadFunc(void)
{
	for(;;)
	{
		// 分配数据包
		tcp_packet_t* pPacket = m_PacketCache.MallocPacket();
		ASSERT(pPacket);
		if(NULL != pPacket)
		{
			// 接收网络数据
			int32_t nRecvSize = recv(m_hSocket, pPacket->m_szPackBuffer, 
				MAX_PACK_BUFFER_SIZE, 0);

			if(nRecvSize > 0)
			{
				CCriticalAutoLock loAutoLock(m_oRecvQueueLock);

				// 添加至接收队列
				pPacket->m_nPackSize = nRecvSize;
				m_RecvPacketQueue.AddTail(pPacket);

				// 事件触发
				m_EventQueue.PushEvent(TCP_NET_EVENT_RECV);
				m_CheckEvent.SetEvent();
			}
			else
			{
				// 释放数据包内存
				m_PacketCache.FreePacket(pPacket);
				// 设置状态
				SetTcpState(ENUM_TCP_STATE_DISSCONNECT);
				break;
			}
		}

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

/// TCP发包处理函数
void CTcpNetTrans::TcpSendThreadFunc(void)
{
	for(;;)
	{
		if(INVALID_SOCKET ==  m_hSocket)
			return;

		// 等待事件
		m_SendEvent.WaitEvent();
		m_SendEvent.ResetEvent();

		CCriticalAutoLock loAutoLock(m_oSendQueueLock);
		while(!m_SendPacketQueue.IsEmpty())
		{
			tcp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
			if(NULL != pPacket)
			{
				// 发送数据包
				send(m_hSocket, pPacket->m_szPackBuffer, pPacket->m_nPackSize, 0);
				// 回收数据包内存空间
				m_PacketCache.FreePacket(pPacket);

				// 事件触发
				m_EventQueue.PushEvent(TCP_NET_EVENT_SEND);
				m_CheckEvent.SetEvent();
			}
		}
	}
}

/// TCP状态检查函数
void CTcpNetTrans::TcpCheckStateFunk(void)
{
	for(;;)
	{
		// 等待事件
		m_CheckEvent.WaitEvent();
		m_CheckEvent.ResetEvent();

		while(!m_EventQueue.IsEmpty())
		{
			// 从事件队列中取出一个事件
			ENUM_TCP_NET_EVENT enEvent = m_EventQueue.PopEvent();
			m_pNetEvent->OnEvent(enEvent);
		}		

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

//TCP收包处理线程
unsigned int CTcpNetTrans::TcpRecvThread(void *pParam)
{
	CTcpNetTrans* pThis = (CTcpNetTrans*)pParam;
	pThis->TcpRecvThreadFunc();
	return 0;
}

//TCP发包处理线程
unsigned int CTcpNetTrans::TcpSendThread(void *pParam)
{
	CTcpNetTrans* pThis = (CTcpNetTrans*)pParam;
	pThis->TcpSendThreadFunc();
	return 0;
}

//TCP状态检查线程
unsigned int CTcpNetTrans::TcpCheckStateThread(void *pParam)
{
	CTcpNetTrans* pThis = (CTcpNetTrans*)pParam;
	pThis->TcpCheckStateFunk();
	return 0;
}
