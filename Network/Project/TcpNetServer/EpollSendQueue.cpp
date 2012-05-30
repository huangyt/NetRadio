#include "EpollSendQueue.h"
#include "DebugTrace.h"
#include "ITcpNetServer.h"

#ifndef _WIN32
//=============================================================================
/// 数据包缓存尺寸
#define DEFAULT_PACKET_CACHE_SIZE		2048

//=============================================================================
CEpollSendQueue::CEpollSendQueue(void)
	: m_PacketCache(DEFAULT_PACKET_CACHE_SIZE)
	, m_PacketNodeCache(DEFAULT_PACKET_CACHE_SIZE)
{
}


CEpollSendQueue::~CEpollSendQueue(void)
{
	RemoveAllPacket();
}

/// 发送数据包
BOOL CEpollSendQueue::SendPacket(SOCKET hSocket, tcp_packet_t* pPacket)
{
	// 参数检查
	ASSERT(pPacket);
	if(NULL == pPacket)
		return FALSE;

	uint32_t nGroupIndex = CalcSocketGroup(hSocket);

	packet_node_t* pNode = m_PacketNodeCache.Malloc();
	if(NULL != pNode)
	{
		pNode->m_hSocket = hSocket;
		pNode->m_pPacket = pPacket;

		CCriticalAutoLock loAutoLock(m_SendQueueLock[nGroupIndex]);
		m_SendQueue[nGroupIndex].AddTail(pNode);

		SendPacket(hSocket);
		return TRUE;
	}
	return FALSE;
}

/// 发送数据包
void CEpollSendQueue::SendPacket(SOCKET hSocket)
{
	uint32_t nGroupIndex = CalcSocketGroup(hSocket);
	
	CCriticalAutoLock loAutoLock(m_SendQueueLock[nGroupIndex]);

	POSITION pos = m_SendQueue[nGroupIndex].GetHeadPosition();
	while(NULL != pos)
	{
		POSITION posDel = pos;
		packet_node_t* pNode = m_SendQueue[nGroupIndex].GetNext(pos);
		if(NULL != pNode && pNode->m_hSocket == hSocket)
		{
			uint32_t nSendSize = send(hSocket, pNode->m_pPacket->m_szPackBuffer, 
				pNode->m_pPacket->m_nPackSize, 0);
			if (nSendSize == -1 && errno != EAGAIN) 
			{
				/// 回收内存
				m_PacketCache.Free(pNode->m_pPacket);
				/// 删除数据包
				m_SendQueue[nGroupIndex].RemoveAt(posDel);
				break;
			}

			if(nSendSize > 0)
			{
				if(pNode->m_pPacket->m_nPackSize == nSendSize)
				{
					/// 回收内存
					m_PacketCache.Free(pNode->m_pPacket);
					// 全部发送完毕，删除数据包
					m_SendQueue[nGroupIndex].RemoveAt(posDel);
				}
				else
				{
					// 数据包未全部发送完毕，有可能发生吗？
					char* pBuffer = pNode->m_pPacket->m_szPackBuffer + nSendSize;
					uint16_t nBufferSize = pNode->m_pPacket->m_nPackSize - nSendSize;
					memmove(pNode->m_pPacket->m_szPackBuffer, pBuffer, nBufferSize);
					break;
				}
			}
			else
			{
				if(errno == EAGAIN)
					break;
			}
		}
	}
}

/// 删除指定SOCKET的全部数据包
void CEpollSendQueue::RemoveAllPacket(SOCKET hSocket)
{
	uint32_t nGroupIndex = CalcSocketGroup(hSocket);

	CCriticalAutoLock loAutoLock(m_SendQueueLock[nGroupIndex]);

	POSITION pos = m_SendQueue[nGroupIndex].GetHeadPosition();
	while(NULL != pos)
	{
		POSITION posDel = pos;
		packet_node_t* pNode = m_SendQueue[nGroupIndex].GetNext(pos);
		if(NULL != pNode && pNode->m_hSocket == hSocket)
		{
			/// 回收内存
			m_PacketCache.Free(pNode->m_pPacket);
			/// 删除数据包
			m_SendQueue[nGroupIndex].RemoveAt(posDel);
		}
	}
}

/// 删除全部数据包
void CEpollSendQueue::RemoveAllPacket(void)
{
	for(uint32_t nIndex=0; nIndex<SEND_QUEUE_GROUP_SIZE; ++nIndex)
	{
		CCriticalAutoLock loAutoLock(m_SendQueueLock[nIndex]);

		POSITION pos = m_SendQueue[nIndex].GetHeadPosition();
		while(NULL != pos)
		{
			POSITION posDel = pos;
			packet_node_t* pNode = m_SendQueue[nIndex].GetNext(pos);
			if(NULL != pNode)
			{
				/// 回收内存
				m_PacketCache.Free(pNode->m_pPacket);
				/// 删除数据包
				m_SendQueue[nIndex].RemoveAt(posDel);
			}
		}
	}
}

/// 获得一个空闲的数据包
tcp_packet_t* CEpollSendQueue::GetFreePacket(void)
{
	return m_PacketCache.Malloc();
}

//=============================================================================
/// 计算SOCKET所在的分组
uint32_t CEpollSendQueue::CalcSocketGroup(SOCKET hSocket)
{
	uint32_t nGroupIndex = hSocket & 0x0F;
	return nGroupIndex; 
}

#endif //_WIN32
