#include "TcpPacketQueue.h"

//=============================================================================
CTcpPacketQueue::CTcpPacketQueue(void)
{
}


CTcpPacketQueue::~CTcpPacketQueue(void)
{
	RemoveAll();
}

uint32_t CTcpPacketQueue::AddTail(const tcp_packet_t* pPacket)
{
	uint32_t nResult = 0;
	tcp_packet_t* pTempPacket = const_cast<tcp_packet_t*>(pPacket);
	if(NULL != m_PacketQueue.AddTail(pTempPacket))
	{
		nResult = m_PacketQueue.GetCount();
	}
	return nResult;
}

tcp_packet_t* CTcpPacketQueue::GetHead(void)
{
	tcp_packet_t* pPacket = NULL;
	if(!m_PacketQueue.IsEmpty())
	{
		pPacket = m_PacketQueue.GetHead();
	}
	return pPacket;
}

tcp_packet_t* CTcpPacketQueue::RemoveHead(void)
{
	tcp_packet_t* pPacket = NULL;
	if(!m_PacketQueue.IsEmpty())
	{
		pPacket = m_PacketQueue.RemoveHead();
	}
	return pPacket;
}

uint32_t CTcpPacketQueue::GetCount(void) const
{
	return m_PacketQueue.GetCount();
}

BOOL CTcpPacketQueue::IsEmpty(void) const
{
	return m_PacketQueue.IsEmpty();
}

void CTcpPacketQueue::RemoveAll(void)
{
	m_PacketQueue.RemoveAll();
}