#include "UdpEventQueue.h"

//=============================================================================
CUdpEventQueue::CUdpEventQueue(void)
{
}


CUdpEventQueue::~CUdpEventQueue(void)
{
	RemoveAll();
}

/// 添加事件
BOOL CUdpEventQueue::PushEvent(ENUM_UDP_NET_EVENT enEvent)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.AddTail(enEvent);
	return TRUE;
}

/// 删除事件
ENUM_UDP_NET_EVENT CUdpEventQueue::PopEvent(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	ENUM_UDP_NET_EVENT enEvent = m_EventList.RemoveHead();
	return enEvent;
}

/// 删除全部事件
void CUdpEventQueue::RemoveAll(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.RemoveAll();
}

/// 判断队列是否为空
BOOL CUdpEventQueue::IsEmpty(void) const
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	return m_EventList.IsEmpty();
}
