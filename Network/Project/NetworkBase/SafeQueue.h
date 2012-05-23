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
/// \file    : SafeQueue.h
/// \brief   : 安全队列头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-18
///============================================================================
#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include "TypeDefine.h"
#include "CriticalSection.h"

//=============================================================================
/// 默认队列节点缓存数量
#define DEFAULT_NODE_CACHE_SIZE		16

//=============================================================================
// struct queue_node_t
typedef struct _queue_node_t
{
	void* m_pQueueData;						///< 节点数据
	struct _queue_node_t* m_pNextNode;		///< 下一节点指针
}queue_node_t;

//=============================================================================
// class CSafeQueue
template <typename T>
class CSafeQueue
{
public:
	CSafeQueue(void);
	~CSafeQueue(void);

public:
	/// 添加节点
	uint32_t AddTail(T* pData);

	/// 获得节点
	T* GetHead(void);

	/// 删除节点
	T* RemoveHead(void);

	/// 获得节点数量
	uint32_t GetCount(void);

	// 删除所有节点
	void RemoveAll(void);

public:
	/// 设置缓存尺寸
	void SetCacheSize(uint32_t nMaxCacheSize);

	/// 获得缓存尺寸
	uint32_t GetCacheSize(void) const;

private:
	/// 创建数据包节点
	queue_node_t* MallocNode(void);
	/// 释放数据包节点
	void FreeNode(queue_node_t* pNode);

private:
	queue_node_t* m_pHeadNode;		///< 头节点
	queue_node_t* m_pTailNode;		///< 尾节点
	queue_node_t* m_pNodeCache;		///< 节点缓存

	uint32_t m_nNodeCount;			///< 节点数量
	uint32_t m_nCacheCount;			///< 缓存数量
	uint32_t m_nMaxCacheCount;		///< 最大缓存数量
	CCriticalSection m_oQueueLock;	///< 队列锁
};


#include "SafeQueue.inl"

#endif //__SAFE_QUEUE_H__
