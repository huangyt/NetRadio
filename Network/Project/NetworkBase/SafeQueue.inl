///============================================================================
/// \file    : SafeQueue.h
/// \brief   : 安全队列实现时间
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-18
///============================================================================
template <typename T>
CSafeQueue<typename T>::CSafeQueue(void)
	: m_pHeadNode(NULL)
	, m_pTailNode(NULL)
	, m_pNodeCache(NULL)
	, m_nNodeCount(0)
	, m_nCacheCount(0)
	, m_nMaxCacheCount(DEFAULT_NODE_CACHE_SIZE)
{
}

template <typename T>
CSafeQueue<typename T>::~CSafeQueue(void)
{
	try
	{
		RemoveAll();
	}
	catch (...)
	{
	}
}

/// 添加节点
template <typename T>
uint32_t CSafeQueue<typename T>::AddTail(T* pData)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	uint32_t nResult = 0;
	if(NULL != pData)
	{
		queue_node_t* pNode = MallocNode();
		if(NULL != pNode)
		{
			pNode->m_pQueueData = pData;

			if(NULL == m_pTailNode)
			{
				m_pTailNode = pNode;
				m_pHeadNode = pNode;
			}
			else
			{
				m_pTailNode->m_pNextNode = pNode;
				m_pTailNode = pNode;
			}

			++m_nNodeCount;
			nResult = m_nNodeCount;
		}
	}
	return nResult;
}

/// 获得节点
template <typename T>
T* CSafeQueue<typename T>::GetHead(void)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	T* pResult = NULL;
	if(NULL != m_pHeadNode)
	{
		pResult = m_pHeadNode->m_pQueueData;
	}
	return pResult;
}

/// 删除节点
template <typename T>
T* CSafeQueue<typename T>::RemoveHead(void)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	T* pResult = NULL;
	if(NULL != m_pHeadNode)
	{
		pResult = m_pHeadNode->m_pQueueData;

		queue_node_t* pNext = m_pHeadNode->m_pNextNode;
		m_pHeadNode = pNext;

		if(NULL == m_pHeadNode)
			m_pTailNode = NULL;

		--m_nNodeCount;
	}
	return pResult;
}

/// 获得节点数量
template <typename T>
uint32_t CSafeQueue<typename T>::GetCount(void)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	return m_nNodeCount;
}

// 删除所有节点
template <typename T>
void CSafeQueue<typename T>::RemoveAll(void)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	while(NULL != m_pHeadNode)
	{
		queue_node_t* pNext = m_pHeadNode->m_pNextNode;
		delete m_pHeadNode;
		m_pHeadNode = pNext;		
	}

	while(NULL != m_pNodeCache)
	{
		queue_node_t* pNext = m_pNodeCache->m_pNextNode;
		delete m_pNodeCache;
		m_pNodeCache = pNext;
	}

	m_nNodeCount = 0;
	m_nCacheCount = 0;
}

/// 设置缓存尺寸
template <typename T>
void CSafeQueue<typename T>::SetCacheSize(uint32_t nMaxCacheSize)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	m_nMaxCacheCount = nMaxCacheSize;

	// 清空Cache
	if(0 == m_nMaxCacheCount)
	{
		while(NULL != m_pNodeCache)
		{
			queue_node_t* pNode = m_pNodeCache->m_pNextNode;
			delete m_pNodeCache;
			m_pNodeCache = pNode;
		}
	}
}

/// 获得缓存尺寸
template <typename T>
uint32_t CSafeQueue<typename T>::GetCacheSize(void) const
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	return m_nMaxCacheCount;
}

//=============================================================================
/// 创建数据包节点
template <typename T>
queue_node_t* CSafeQueue<typename T>::MallocNode(void)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	queue_node_t* pQueueNode = NULL;

	if(NULL != m_pNodeCache)
	{
		pQueueNode = m_pNodeCache;
		m_pNodeCache = m_pNodeCache->m_pNextNode;
		--m_nCacheCount;
	}
	else
	{
		pQueueNode = new queue_node_t;
	}
	memset(pQueueNode, 0, sizeof(queue_node_t));
	return pPacketNode;
}

/// 释放数据包节点
template <typename T>
void CSafeQueue<typename T>::FreeNode(queue_node_t* pQueueNode)
{
	CCriticalAutoLock loAutoLock(m_oQueueLock);

	if(NULL != pQueueNode)
	{
		if(m_nCacheCount < m_nMaxCacheCount)
		{
			pQueueNode->m_pNextNode = m_pNodeCache;
			m_pNodeCache = pQueueNode;
			++m_nCacheCount;
		}
		else
		{
			delete pPacket;
			pPacket = NULL;
		}
	}
}
