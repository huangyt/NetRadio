/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : CacheTmpl.h
/// \brief   : 缓存模板类
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-24
///============================================================================
#ifndef __CACHE_TMPL_H__
#define __CACHE_TMPL_H__

#include <stdlib.h>
#include "TypeDefine.h"
#include "ListTmpl.h"
#include "CriticalSection.h"

//=============================================================================
/// 最小缓存尺寸
#define MIN_CACHE_SIZE	16

//=============================================================================
template <typename TYPE>
class CCacheTmpl
{
private:
	typedef struct _cache_node_t
	{
		struct _cache_node_t* m_pNext;	///< 下一个节点
		TYPE* m_pType;					///< 数据指针
	}cache_node_t;

public:
	CCacheTmpl(uint32_t nMinCacheSize = MIN_CACHE_SIZE)
		: m_pCacheList(NULL)
		, m_nCacheCount(0)
		, m_nMinCacheSize(nMinCacheSize)
		, m_pFreeNodeList(NULL)
		, m_nFreeNodeCount(0)
	{
	}

	~CCacheTmpl(void)
	{
		Clear();
	}

public:
	/// 分配一个数据包
	TYPE* Malloc(void)
	{
		CCriticalAutoLock loAutoLock(m_oCacheLock);

		if(NULL == m_pCacheList)
		{
			for(uint32_t nIndex =0; nIndex<m_nMinCacheSize; ++nIndex)
			{
				cache_node_t* pNode = MallocNode();
				if(NULL != pNode)
				{
					pNode->m_pType = (TYPE*)malloc(sizeof(TYPE));
					if(NULL != pNode->m_pType)
					{
						pNode->m_pNext = m_pCacheList;
						m_pCacheList = pNode;
						++m_nCacheCount;
					}
					else
					{
						FreeNode(pNode);
						pNode = NULL;
					}
				}
			}
		}

		TYPE* pType = NULL;
		if(NULL != m_pCacheList)
		{
			cache_node_t* pNode = m_pCacheList;
			m_pCacheList = pNode->m_pNext;

			FreeNode(pNode);
			pNode = NULL;

			pType = pNode->m_pType;
			--m_nCacheCount;
		}
		return pType;
	}

	/// 释放一个数据包
	void Free(TYPE* pPointer)
	{
		CCriticalAutoLock loAutoLock(m_oCacheLock);

		if(pPointer != NULL)
		{
			if(m_nCacheCount < m_nMinCacheSize)
			{
				cache_node_t* pNode = MallocNode();
				if(NULL != pNode)
				{
					pNode->m_pType = pPointer;
					pNode->m_pNext = m_pCacheList;
					m_pCacheList = pNode;
					++m_nCacheCount;
				}
				else
				{
					free(pPointer);
				}
			}
			else
			{
				delete pPointer;
				pPointer = NULL;
			}
		}
	}

	void Clear(void)
	{
		CCriticalAutoLock loAutoLock(m_oCacheLock);

		while(NULL != m_pCacheList)
		{
			cache_node_t* pNode = m_pCacheList;
			m_pCacheList = pNode->m_pNext;

			free(pNode->m_pType);
			FreeNode(pNode);
		}

		while(NULL != m_pFreeNodeList)
		{
			cache_node_t* pNode = m_pFreeNodeList;
			m_pFreeNodeList = pNode->m_pNext;
			free(pNode);
		}
	}

private:
	/// 分配空闲节点
	cache_node_t* MallocNode(void)
	{
		if(NULL == m_pFreeNodeList)
		{
			for(uint32_t nIndex =0; nIndex<m_nMinCacheSize; ++nIndex)
			{
				cache_node_t* pNode = (cache_node_t*)malloc(sizeof(cache_node_t));
				if(NULL != pNode)
				{
					pNode->m_pNext = m_pFreeNodeList;
					m_pFreeNodeList = pNode;

					++m_nFreeNodeCount;
				}
			}
		}

		cache_node_t* pNode = NULL;
		if(NULL != m_pFreeNodeList)
		{
			pNode = m_pFreeNodeList;
			m_pFreeNodeList = pNode->m_pNext;

			--m_nFreeNodeCount;
		}
		return pNode;
	}

	/// 释放空闲节点
	void FreeNode(cache_node_t* pNode)
	{
		if(NULL != pNode)
		{
			if(m_nFreeNodeCount < m_nMinCacheSize)
			{
				pNode->m_pNext = m_pFreeNodeList;
				m_pFreeNodeList = pNode;
				++m_nFreeNodeCount;
			}
			else
			{
				free(pNode);
				pNode = NULL;
			}
		}
	}

private:
	cache_node_t* m_pCacheList;				///< 缓存链表
	uint32_t m_nCacheCount;					///< 缓存链表
	uint32_t m_nMinCacheSize;				///< 最小缓存数量

	cache_node_t* m_pFreeNodeList;			///< 空闲节点链表
	uint32_t m_nFreeNodeCount;				///< 空闲节点数量
	CCriticalSection m_oCacheLock;			///< 缓存临界
};

#endif //__CACHE_TMPL_H__
