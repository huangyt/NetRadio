#include "UdpNetTrans.h"
#include "DebugTrace.h"
#include "NetworkAPI.h"

//=============================================================================
#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")
#endif

//=============================================================================
#define DEFAULT_ENCRYPT_KEY			"D264CFE4-290A-463A-AA71-CA73EE524DC7"
#define DEFAULT_ENCRYPT_KEY_SIZE	16

//=============================================================================
CUdpNetTrans::CUdpNetTrans(void)
	: m_CheckStateThread(&UdpCheckStateThread)
	, m_SendThread(&UdpSendThread)
	, m_RecvThread(&UdpRecvThread)
{
	m_pNetEvent = NULL;
	m_hSocket = INVALID_SOCKET;

	m_enUdpState = ENUM_UDP_STATE_NONE;
	m_enEncryptType = ENUM_ENCRYPT_AES;

#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
}


CUdpNetTrans::~CUdpNetTrans(void)
{
	Close();

#ifdef _WIN32
	WSACleanup();
#endif
}

BOOL CUdpNetTrans::Open(IUdpNetEvent* pNetEvent, 
	const char* szLocalAddr, uint16_t nPort)
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
		m_hSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if(INVALID_SOCKET == m_hSocket)
		{
			TraceLogError("CUdpNetTrans::Open 创建SOCKET套接字失败 Error=%d\n", 
				WSAGetLastError());
			break;
		}

		struct sockaddr_in addr;
		memset(&addr, 0,sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(nPort);
		if (NULL != szLocalAddr)
		{
			SetSocketAddr(addr, inet_addr(szLocalAddr));
		}
		else
		{
			SetSocketAddr(addr, htonl(INADDR_ANY));
		}

		if (SOCKET_ERROR == bind(m_hSocket, (LPSOCKADDR)&addr,sizeof(addr)))
		{
			TraceLogError("CTcpNetTrans::Open SOCKET绑定端口%d失败\n", nPort);
			break;
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
		SetUdpState(ENUM_UDP_STATE_OPEN);

		bResult = TRUE;

	} while (FALSE);

	if(!bResult)
	{
		Close();
	}

	return bResult;
}

BOOL CUdpNetTrans::IsOpen(void) const
{
	return (INVALID_SOCKET != m_hSocket);
}

void CUdpNetTrans::Close(void)
{
	if(INVALID_SOCKET != m_hSocket)
	{
		// 设置状态
		SetUdpState(ENUM_UDP_STATE_CLOSE);

		// 关闭套接字
		shutdown(m_hSocket, 0); 
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
				udp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		{
			// 清空接收队列
			CCriticalAutoLock loAutoLock(m_oRecvQueueLock);
			while(m_RecvPacketQueue.GetCount() > 0)
			{
				udp_packet_t* pPacket = m_RecvPacketQueue.RemoveHead();
				m_PacketCache.FreePacket(pPacket);
			}
		}

		// 销毁PackBuffer
		m_PackBuffer.Destroy();
	}
}

BOOL CUdpNetTrans::SetEncryptType(ENUM_ENCRYPT_TYPE enType)
{
	m_enEncryptType = enType;
	return (m_enEncryptType != ENUM_ENCRYPT_NONE);
}

BOOL CUdpNetTrans::SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize)
{
	return m_PackBuffer.SetEncryptKey(szEncryptKey, nKeySize);
}

/// 发送数据包(异步方式)
uint32_t CUdpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize, 
	const char* szSvrAddr, uint16_t nSvrPort)
{
	/// 参数检查
	ASSERT(szBuffer);
	ASSERT(szSvrAddr);
	ASSERT(nBufferSize != 0);
	ASSERT(nSvrPort != 0);
	
	if(NULL == szBuffer || NULL == szSvrAddr 
		|| 0 == nBufferSize || 0 == nSvrPort)
	{
		return 0;
	}

	if(!IsIPAddr(szSvrAddr))
		return 0;

	if(!IsOpen())
		return 0;

	uint32_t nAddr = ntohl(inet_addr(szSvrAddr));
	return Send(szBuffer, nBufferSize, nAddr, nSvrPort);
}


uint32_t CUdpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize, 
	const sockaddr_in* pSvrAddr)
{
	/// 参数检查
	ASSERT(szBuffer);
	ASSERT(nBufferSize != 0);
	ASSERT(pSvrAddr);

	if(NULL == szBuffer || 0 == nBufferSize || NULL == pSvrAddr)
	{
		return 0;
	}

	if(!IsOpen())
		return 0;

	uint32_t nAddr = ntohl(GetSocketAddr(*pSvrAddr));
	uint32_t nPort = ntohl(pSvrAddr->sin_port);

	return Send(szBuffer, nBufferSize, nAddr, nPort);
}

uint32_t CUdpNetTrans::Send(const char* szBuffer, uint32_t nBufferSize, 
	uint32_t nSvrAddr, uint16_t nSvrPort)
{
	/// 参数检查
	ASSERT(szBuffer);
	ASSERT(nBufferSize != 0);
	ASSERT(nSvrPort != 0);

	if(NULL == szBuffer || 0 == nBufferSize || 0 == nSvrPort)
	{
		return 0;
	}

	if(!IsOpen())
		return 0;

	// 分配数据包
	udp_packet_t* pPacket = m_PacketCache.MallocPacket();
	ASSERT(pPacket);
	if(NULL != pPacket)
	{
		// 打包
		pPacket->m_nPackSize = m_PackBuffer.Pack(szBuffer, nBufferSize, 
			pPacket->m_szPackBuffer, MAX_PACK_BUFFER_SIZE, m_enEncryptType);

		if(pPacket->m_nPackSize > 0)
		{
			pPacket->m_nSvrAddr = nSvrAddr;
			pPacket->m_nSvrPort = nSvrPort;

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


/// 接收数据包
uint32_t CUdpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize, 
	char* szSvrAddr, uint16_t& nSvrPort)
{
	uint32_t nSvrAddr = 0;
	uint32_t nResult = Recv(szBuffer, nBufferSize, nSvrAddr, nSvrPort);
	strcpy(szSvrAddr, GetIPAddr(nSvrAddr));
	return nResult;
}

uint32_t CUdpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize, 
	sockaddr_in* pSvrAddr)
{
	uint32_t nSvrAddr = 0;
	uint16_t nSvrPort = 0;

	uint32_t nResult = Recv(szBuffer, nBufferSize, nSvrAddr, nSvrPort);
	
	if(NULL != pSvrAddr)
	{
		memset(pSvrAddr, 0,sizeof(sockaddr_in));
		pSvrAddr->sin_family = AF_INET;
		pSvrAddr->sin_port = htons(nSvrPort);
		SetSocketAddr(*pSvrAddr, nSvrAddr);
	}

	return nResult;
}

uint32_t CUdpNetTrans::Recv(char* szBuffer, uint32_t nBufferSize, 
	uint32_t& nSvrAddr, uint16_t& nSvrPort)
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
	udp_packet_t* pPacket = m_RecvPacketQueue.RemoveHead();
	if(NULL != pPacket)
	{
		int32_t nPackSize = m_PackBuffer.UnPack(pPacket->m_szPackBuffer, 
			pPacket->m_nPackSize, szBuffer, nRecvSize, nTimeStamp);

		nSvrAddr = pPacket->m_nSvrAddr;
		nSvrPort = pPacket->m_nSvrPort;

		// 回收内存
		m_PacketCache.FreePacket(pPacket);
		return nRecvSize;
	}

	return 0;
}

//=============================================================================
/// 设置UDP状态
ENUM_UDP_STATE CUdpNetTrans::SetUdpState(ENUM_UDP_STATE enNewState)
{
	ENUM_UDP_STATE enOldState = m_enUdpState;

	if(enNewState != m_enUdpState)
	{
		m_enUdpState = enNewState;

		switch(m_enUdpState)
		{
		case ENUM_UDP_STATE_OPEN:			///< 打开
			m_EventQueue.PushEvent(UDP_NET_EVENT_OPEN);
			m_CheckEvent.SetEvent();
			break;
		case ENUM_UDP_STATE_CLOSE:			///< 关闭
			m_EventQueue.PushEvent(UDP_NET_EVENT_CLOSE);
			m_CheckEvent.SetEvent();
			break;
		default:
			break;
		}
	}
	return enOldState;
}

/// 获得UDP状态
ENUM_UDP_STATE CUdpNetTrans::GetUdpState(void) const
{
	return m_enUdpState;
}

/// UDP收包处理函数
void CUdpNetTrans::UdpRecvThreadFunc(void)
{
	for(;;)
	{
		// 分配数据包
		udp_packet_t* pPacket = m_PacketCache.MallocPacket();
		ASSERT(pPacket);
		if(NULL != pPacket)
		{
			struct sockaddr_in addr;
			int nAddrSize = sizeof(addr);

			// 接收网络数据
			int32_t nRecvSize = recvfrom(m_hSocket, pPacket->m_szPackBuffer, 
				MAX_PACK_BUFFER_SIZE, 0, (sockaddr*)&addr, &nAddrSize);

			if(nRecvSize > 0)
			{
				CCriticalAutoLock loAutoLock(m_oRecvQueueLock);

				// 添加至接收队列
				pPacket->m_nPackSize = nRecvSize;
				m_RecvPacketQueue.AddTail(pPacket);

				// 事件触发
				m_EventQueue.PushEvent(UDP_NET_EVENT_RECV);
				m_CheckEvent.SetEvent();
			}
			else
			{
				// 释放数据包内存
				m_PacketCache.FreePacket(pPacket);
			}
		}

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

/// UDP发包处理函数
void CUdpNetTrans::UdpSendThreadFunc(void)
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
			udp_packet_t* pPacket = m_SendPacketQueue.RemoveHead();
			if(NULL != pPacket)
			{
				struct sockaddr_in addr;
				memset(&addr, 0,sizeof(addr));
				addr.sin_family = AF_INET;
				addr.sin_port = htons(pPacket->m_nSvrPort);
				SetSocketAddr(addr, pPacket->m_nSvrAddr);

				// 发送数据包
				sendto(m_hSocket, pPacket->m_szPackBuffer, pPacket->m_nPackSize, 
					0, (const sockaddr*)&addr, sizeof(addr));

				// 回收数据包内存空间
				m_PacketCache.FreePacket(pPacket);

				// 事件触发
				m_EventQueue.PushEvent(UDP_NET_EVENT_SEND);
				m_CheckEvent.SetEvent();
			}
		}
	}
}

/// UDP状态检查函数
void CUdpNetTrans::UdpCheckStateFunk(void)
{
	for(;;)
	{
		// 等待事件
		m_CheckEvent.WaitEvent();
		m_CheckEvent.ResetEvent();

		while(!m_EventQueue.IsEmpty())
		{
			// 从事件队列中取出一个事件
			ENUM_UDP_NET_EVENT enEvent = m_EventQueue.PopEvent();
			m_pNetEvent->OnEvent(enEvent);
		}		

		if(INVALID_SOCKET ==  m_hSocket)
			return;
	}
}

//UDP收包处理线程
unsigned int CUdpNetTrans::UdpRecvThread(void *pParam)
{
	CUdpNetTrans* pThis = (CUdpNetTrans*)pParam;
	pThis->UdpRecvThreadFunc();
	return 0;
}

//UDP发包处理线程
unsigned int CUdpNetTrans::UdpSendThread(void *pParam)
{
	CUdpNetTrans* pThis = (CUdpNetTrans*)pParam;
	pThis->UdpSendThreadFunc();
	return 0;
}

//UDP状态检查线程
unsigned int CUdpNetTrans::UdpCheckStateThread(void *pParam)
{
	CUdpNetTrans* pThis = (CUdpNetTrans*)pParam;
	pThis->UdpCheckStateFunk();
	return 0;
}


