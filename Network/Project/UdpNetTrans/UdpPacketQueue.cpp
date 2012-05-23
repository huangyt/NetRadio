#include "UdpPacketQueue.h"

//=============================================================================
CUdpPacketQueue::CUdpPacketQueue(void)
{
}


CUdpPacketQueue::~CUdpPacketQueue(void)
{
	RemoveAll();
}

uint32_t CUdpPacketQueue::AddTail(const udp_packet_t* pPacket)
{
	uint32_t nResult = 0;
	udp_packet_t* pTempPacket = const_cast<udp_packet_t*>(pPacket);
	if(NULL != m_PacketQueue.AddTail(pTempPacket))
	{
		nResult = m_PacketQueue.GetCount();
	}
	return nResult;
}

udp_packet_t* CUdpPacketQueue::GetHead(void)
{
	udp_packet_t* pPacket = NULL;
	if(!m_PacketQueue.IsEmpty())
	{
		pPacket = m_PacketQueue.GetHead();
	}
	return pPacket;
}

udp_packet_t* CUdpPacketQueue::RemoveHead(void)
{
	udp_packet_t* pPacket = NULL;
	if(!m_PacketQueue.IsEmpty())
	{
		pPacket = m_PacketQueue.RemoveHead();
	}
	return pPacket;
}

uint32_t CUdpPacketQueue::GetCount(void) const
{
	return m_PacketQueue.GetCount();
}

BOOL CUdpPacketQueue::IsEmpty(void) const
{
	return m_PacketQueue.IsEmpty();
}

void CUdpPacketQueue::RemoveAll(void)
{
	m_PacketQueue.RemoveAll();
}