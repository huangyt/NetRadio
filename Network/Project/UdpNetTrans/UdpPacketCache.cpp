#include <assert.h>
#include "UdpPacketCache.h"

//=============================================================================
CUdpPacketCache::CUdpPacketCache(uint32_t nCacheSize)
	: m_PacketCache(nCacheSize)
	, m_pBlockCache(NULL)
	, m_nBlockSize(nCacheSize)
{
}


CUdpPacketCache::~CUdpPacketCache(void)
{
	CCriticalAutoLock loAutoLock(m_oCacheLock);

	m_PacketCache.RemoveAll();
	// 释放内存
	DestroyBlockNode(m_pBlockCache);
	m_pBlockCache = NULL;
}

/// 分配一个数据包
udp_packet_t* CUdpPacketCache::MallocPacket(void)
{
	CCriticalAutoLock loAutoLock(m_oCacheLock);

	udp_packet_t* pResult = NULL;

	if(m_PacketCache.IsEmpty())
	{
		block_node_t* pBlockNode = CreateBlockNode(m_pBlockCache, m_nBlockSize,
			sizeof(udp_packet_t));

		if(NULL != pBlockNode)
		{
			udp_packet_t* pNode = (udp_packet_t*)pBlockNode->m_pBuffer;
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
		udp_packet_t* pNode = m_PacketCache.RemoveHead();
		pResult = pNode;
	}
	return pResult;
}

/// 释放一个数据包
void CUdpPacketCache::FreePacket(udp_packet_t* pPacket)
{
	CCriticalAutoLock loAutoLock(m_oCacheLock);

	if(pPacket != NULL)
	{
		m_PacketCache.AddTail(pPacket);
	}
}