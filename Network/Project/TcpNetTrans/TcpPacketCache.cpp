#include <assert.h>
#include "TcpPacketCache.h"

//=============================================================================
CTcpPacketCache::CTcpPacketCache(uint32_t nCacheSize)
	: m_PacketCache(nCacheSize)
	, m_pBlockCache(NULL)
	, m_nBlockSize(nCacheSize)
{
}


CTcpPacketCache::~CTcpPacketCache(void)
{
	CCriticalAutoLock loAutoLock(m_oCacheLock);

	m_PacketCache.RemoveAll();
	// 释放内存
	DestroyBlockNode(m_pBlockCache);
	m_pBlockCache = NULL;
}

/// 分配一个数据包
tcp_packet_t* CTcpPacketCache::MallocPacket(void)
{
	CCriticalAutoLock loAutoLock(m_oCacheLock);

	tcp_packet_t* pResult = NULL;

	if(m_PacketCache.IsEmpty())
	{
		block_node_t* pBlockNode = CreateBlockNode(m_pBlockCache, m_nBlockSize,
			sizeof(tcp_packet_t));

		if(NULL != pBlockNode)
		{
			tcp_packet_t* pNode = (tcp_packet_t*)pBlockNode->m_pBuffer;
			// free in reverse order to make it easier to debug
			pNode += m_nBlockSize - 1;
			for (int32_t i = m_nBlockSize-1; i >= 0; i--, pNode--)
			{
				m_PacketCache.AddTail(pNode);
			}
		}
	}

	if(!m_PacketCache.IsEmpty())
	{
		tcp_packet_t* pNode = m_PacketCache.RemoveHead();
		pResult = pNode;
	}
	return pResult;
}

/// 释放一个数据包
void CTcpPacketCache::FreePacket(tcp_packet_t* pPacket)
{
	CCriticalAutoLock loAutoLock(m_oCacheLock);

	if(pPacket != NULL)
	{
		m_PacketCache.AddTail(pPacket);
	}
}