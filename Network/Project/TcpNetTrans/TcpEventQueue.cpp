#include "TcpEventQueue.h"

//=============================================================================
CTcpEventQueue::CTcpEventQueue(void)
{
}


CTcpEventQueue::~CTcpEventQueue(void)
{
	RemoveAll();
}

/// 添加事件
BOOL CTcpEventQueue::PushEvent(ENUM_TCP_NET_EVENT enEvent)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.AddTail(enEvent);
	return TRUE;
}

/// 删除事件
ENUM_TCP_NET_EVENT CTcpEventQueue::PopEvent(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	ENUM_TCP_NET_EVENT enEvent = m_EventList.RemoveHead();
	return enEvent;
}

/// 删除全部事件
void CTcpEventQueue::RemoveAll(void)
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	m_EventList.RemoveAll();
}

/// 判断队列是否为空
BOOL CTcpEventQueue::IsEmpty(void) const
{
	CCriticalAutoLock loAutoLock(m_ListLock);
	return m_EventList.IsEmpty();
}
