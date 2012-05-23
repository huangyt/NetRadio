//=============================================================================
/// 最大指针空间尺寸
#define MAX_POINTER_SIZE	8

static block_node_t* CreateBlockNode(block_node_t*& pHeadNode, uint32_t nMax, 
	uint32_t nElementSize)
{
	// 计算缓冲区尺寸
	uint32_t nBufferSize = MAX_POINTER_SIZE + nMax * nElementSize;
	// 申请内存
	char* pBuffer = new char[nBufferSize];
	// 指针类型转换
	block_node_t* pBlockNode = (block_node_t*)pBuffer;
	if(NULL != pBlockNode)
	{
		pBlockNode->m_pNext = pHeadNode;
		pBlockNode->m_pBuffer = pBuffer+MAX_POINTER_SIZE;
		pHeadNode = pBlockNode;
	}
	return pBlockNode;
}

static void DestroyBlockNode(block_node_t* pBlockNode)
{
	block_node_t* pNode = pBlockNode;
	while (pNode != NULL)
	{
		char* bytes = (char*)pNode;
		block_node_t* pNext = pNode->m_pNext;
		delete[] bytes;
		pNode = pNext;
	}
}

//=============================================================================
template<typename TYPE, typename ARG_TYPE>
CListTmpl<TYPE, ARG_TYPE>::CListTmpl(uint32_t nBlockSize)
{
	m_pNodeHead = NULL;
	m_pNodeTail = NULL;
	m_nNodeCount = 0;
	m_pNodeFree = NULL;
	m_pBlockCache = NULL;
	m_nBlockSize = nBlockSize;
}

template<typename TYPE, typename ARG_TYPE>
CListTmpl<TYPE, ARG_TYPE>::~CListTmpl(void)
{
	RemoveAll();
}

template<typename TYPE, typename ARG_TYPE>
inline uint32_t CListTmpl<TYPE, ARG_TYPE>::GetCount(void) const
{ 
	return m_nNodeCount; 
}

template<typename TYPE, typename ARG_TYPE>
inline uint32_t CListTmpl<TYPE, ARG_TYPE>::GetSize(void) const
{ 
	return m_nNodeCount; 
}

template<typename TYPE, typename ARG_TYPE>
inline BOOL CListTmpl<TYPE, ARG_TYPE>::IsEmpty(void) const
{
	return m_nNodeCount == 0;
}

template<typename TYPE, typename ARG_TYPE>
inline TYPE& CListTmpl<TYPE, ARG_TYPE>::GetHead(void)
{
	if(NULL == m_pNodeHead)
		throw (-1);
	return m_pNodeHead->m_Data;
}

template<typename TYPE, typename ARG_TYPE>
inline const TYPE& CListTmpl<TYPE, ARG_TYPE>::GetHead(void) const
{
	if(NULL == m_pNodeHead)
		throw (-1);
	return m_pNodeHead->m_Data;
}

template<typename TYPE, typename ARG_TYPE>
inline TYPE& CListTmpl<TYPE, ARG_TYPE>::GetTail(void)
{
	if(NULL == m_pNodeTail)
		throw (-1);
	return m_pNodeTail->m_Data;
}

template<typename TYPE, typename ARG_TYPE>
inline const TYPE& CListTmpl<TYPE, ARG_TYPE>::GetTail(void) const
{
	if(NULL == m_pNodeTail)
		throw (-1);
	return m_pNodeTail->m_Data;
}

template<typename TYPE, typename ARG_TYPE>
TYPE CListTmpl<TYPE, ARG_TYPE>::RemoveHead(void)
{
	if(NULL == m_pNodeHead)
		throw (-1);

	list_node_t* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->m_Data;

	m_pNodeHead = pOldNode->m_pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->m_pPrev = NULL;
	else
		m_pNodeTail = NULL;

	FreeNode(pOldNode);
	return returnValue;
}

template<typename TYPE, typename ARG_TYPE>
TYPE CListTmpl<TYPE, ARG_TYPE>::RemoveTail(void)
{
	if(NULL == m_pNodeTail)
		throw (-1);

	list_node_t* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->m_Data;

	m_pNodeTail = pOldNode->m_pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->m_pNext = NULL;
	else
		m_pNodeHead = NULL;

	FreeNode(pOldNode);
	return returnValue;
}

template<typename TYPE, typename ARG_TYPE>
typename CListTmpl<TYPE, ARG_TYPE>::list_node_t* 
	CListTmpl<TYPE, ARG_TYPE>::NewNode(list_node_t* pPrevNode, 
	list_node_t* pNextNode)
{
	if (m_pNodeFree == NULL)
	{
		// 添加一个内存块
		block_node_t* pBlockNode = CreateBlockNode(m_pBlockCache, m_nBlockSize,
			sizeof(list_node_t));

		if(NULL != pBlockNode)
		{
			// chain them into free list
			list_node_t* pNode = (list_node_t*)pBlockNode->m_pBuffer;
			// free in reverse order to make it easier to debug
			pNode += m_nBlockSize - 1;
			for (int32_t i = m_nBlockSize-1; i >= 0; i--, pNode--)
			{
				pNode->m_pNext = m_pNodeFree;
				m_pNodeFree = pNode;
			}
		}
	}
	
	if(NULL == m_pNodeFree)
		throw -1;

	CListTmpl::list_node_t* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->m_pNext;
	pNode->m_pPrev = pPrevNode;
	pNode->m_pNext = pNextNode;
	m_nNodeCount++;

	// 执行构造函数
	//::new( (void*)( &pNode->m_Data ) ) TYPE;
	return pNode;
}

template<typename TYPE, typename ARG_TYPE>
void CListTmpl<TYPE, ARG_TYPE>::FreeNode(list_node_t* pNode)
{
	//pNode->m_Data.~TYPE();
	pNode->m_pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nNodeCount--;

	// if no more elements, cleanup completely
	if (m_nNodeCount == 0)
		RemoveAll();
}

template<typename TYPE, typename ARG_TYPE>
POSITION CListTmpl<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newNode)
{
	list_node_t* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->m_Data = newNode;
	if (m_pNodeHead != NULL)
		m_pNodeHead->m_pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POSITION)pNewNode;
}

template<typename TYPE, typename ARG_TYPE>
POSITION CListTmpl<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newNode)
{
	list_node_t* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->m_Data = newNode;
	if (m_pNodeTail != NULL)
		m_pNodeTail->m_pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POSITION) pNewNode;
}

template<typename TYPE, typename ARG_TYPE>
void CListTmpl<TYPE, ARG_TYPE>::AddHead(CListTmpl* pNewList)
{
	if(NULL != pNewList)
	{
		POSITION pos = pNewList->GetTailPosition();
		while (pos != NULL)
			AddHead(pNewList->GetPrev(pos));
	}
}

template<typename TYPE, typename ARG_TYPE>
void CListTmpl<TYPE, ARG_TYPE>::AddTail(CListTmpl* pNewList)
{
	if(NULL != pNewList)
	{
		POSITION pos = pNewList->GetHeadPosition();
		while (pos != NULL)
			AddTail(pNewList->GetNext(pos));
	}
}

template<typename TYPE, typename ARG_TYPE>
void CListTmpl<TYPE, ARG_TYPE>::RemoveAll(void)
{
	// 执行析构函数
	list_node_t* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->m_pNext)
		pNode->m_Data.~TYPE();

	m_nNodeCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	
	DestroyBlockNode(m_pBlockCache);
	m_pBlockCache = NULL;
}

template<typename TYPE, typename ARG_TYPE>
inline POSITION CListTmpl<TYPE, ARG_TYPE>::GetHeadPosition(void) const
{
	return (POSITION) m_pNodeHead; 
}

template<typename TYPE, typename ARG_TYPE>
inline POSITION CListTmpl<TYPE, ARG_TYPE>::GetTailPosition(void) const
{
	return (POSITION) m_pNodeTail;
}

template<typename TYPE, typename ARG_TYPE>
inline TYPE& CListTmpl<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition)
{
	list_node_t* pNode = (list_node_t*) rPosition;
	rPosition = (POSITION) pNode->m_pNext;
	return pNode->m_Data;
}

template<typename TYPE, typename ARG_TYPE>
inline const TYPE& CListTmpl<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) const
{
	list_node_t* pNode = (list_node_t*) rPosition;
	rPosition = (POSITION) pNode->m_pNext;
	return pNode->m_Data; 
}

template<typename TYPE, typename ARG_TYPE>
inline TYPE& CListTmpl<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition)
{
	list_node_t* pNode = (list_node_t*) rPosition;
	rPosition = (POSITION) pNode->m_pPrev;
	return pNode->m_Data; 
}

template<typename TYPE, typename ARG_TYPE>
inline const TYPE& CListTmpl<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) const
{
	list_node_t* pNode = (list_node_t*) rPosition;
	rPosition = (POSITION) pNode->m_pPrev;
	return pNode->m_Data;
}

template<typename TYPE, typename ARG_TYPE>
inline TYPE& CListTmpl<TYPE, ARG_TYPE>::GetAt(POSITION position)
{
	list_node_t* pNode = (list_node_t*) position;
	return pNode->m_Data; 
}

template<typename TYPE, typename ARG_TYPE>
inline const TYPE& CListTmpl<TYPE, ARG_TYPE>::GetAt(POSITION position) const
{
	list_node_t* pNode = (list_node_t*) position;
	return pNode->m_Data; 
}

template<typename TYPE, typename ARG_TYPE>
inline void CListTmpl<TYPE, ARG_TYPE>::SetAt(POSITION pos, ARG_TYPE newNode)
{
	list_node_t* pNode = (list_node_t*) pos;
	pNode->m_Data = newNode;
}

template<typename TYPE, typename ARG_TYPE>
void CListTmpl<TYPE, ARG_TYPE>::RemoveAt(POSITION position)
{
	list_node_t* pOldNode = (list_node_t*) position;

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->m_pNext;
	}
	else
	{
		pOldNode->m_pPrev->m_pNext = pOldNode->m_pNext;
	}

	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->m_pPrev;
	}
	else
	{
		pOldNode->m_pNext->m_pPrev = pOldNode->m_pPrev;
	}
	FreeNode(pOldNode);
}

template<typename TYPE, typename ARG_TYPE>
POSITION CListTmpl<TYPE, ARG_TYPE>::InsertBefore(POSITION position, 
	ARG_TYPE newNode)
{
	if (position == NULL)
		return AddHead(newNode); // insert before nothing -> head of the list

	// Insert it before position
	list_node_t* pOldNode = (list_node_t*) position;
	list_node_t* pNewNode = NewNode(pOldNode->m_pPrev, pOldNode);
	pNewNode->m_Data = newNode;

	if (pOldNode->m_pPrev != NULL)
	{
		pOldNode->m_pPrev->m_pNext = pNewNode;
	}
	else
	{
		if(pOldNode != m_pNodeHead)
			throw (-1);
		m_pNodeHead = pNewNode;
	}
	pOldNode->m_pPrev = pNewNode;
	return (POSITION) pNewNode;
}

template<typename TYPE, typename ARG_TYPE>
POSITION CListTmpl<TYPE, ARG_TYPE>::InsertAfter(POSITION position, 
	ARG_TYPE newNode)
{
	if (position == NULL)
		return AddTail(newNode); // insert after nothing -> tail of the list

	// Insert it before position
	list_node_t* pOldNode = (list_node_t*) position;
	list_node_t* pNewNode = NewNode(pOldNode, pOldNode->m_pNext);
	pNewNode->m_Data = newNode;

	if (pOldNode->m_pNext != NULL)
	{
		pOldNode->m_pNext->m_pPrev = pNewNode;
	}
	else
	{
		if(pOldNode != m_pNodeTail)
			throw (-1);
		m_pNodeTail = pNewNode;
	}
	pOldNode->m_pNext = pNewNode;
	return (POSITION) pNewNode;
}

template<typename TYPE, typename ARG_TYPE>
POSITION CListTmpl<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, 
	POSITION startAfter) const
{
	list_node_t* pNode = (list_node_t*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		pNode = pNode->m_pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->m_pNext)
	{
		if (pNode->m_Data == searchValue)
			return (POSITION)pNode;
	}
	return NULL;
}

template<typename TYPE, typename ARG_TYPE>
POSITION CListTmpl<TYPE, ARG_TYPE>::FindIndex(uint32_t nIndex) const
{
	if (nIndex >= m_nNodeCount || nIndex < 0)
		return NULL;

	list_node_t* pNode = m_pNodeHead;
	while (nIndex--)
	{
		pNode = pNode->m_pNext;
	}
	return (POSITION) pNode;
}
