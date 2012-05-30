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
/// \file    : EpollSendQueue.h
/// \brief   : Epoll发送队列
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-30
///============================================================================
#ifndef __EPOLL_SEND_QUEUE_H__
#define __EPOLL_SEND_QUEUE_H__

#ifndef _WIN32

#include "TypeDefine.h"
#include "CacheTmpl.h"
#include "CriticalSection.h"

//=============================================================================
/// 发送队列分组尺寸
#define SEND_QUEUE_GROUP_SIZE	16

//=============================================================================
typedef struct _packet_node_t
{
	SOCKET m_hSocket;					///< SOCKET句柄
	tcp_packet_t* m_pPacket;			///< TCP数据包指针指针
}packet_node_t;

//=============================================================================
class CEpollSendQueue
{
public:
	CEpollSendQueue(void);
	~CEpollSendQueue(void);

public:
	/// 发送数据包
	BOOL SendPacket(SOCKET hSocket, tcp_packet_t* pPacket);
	/// 发送数据包
	void SendPacket(SOCKET hSocket);
	/// 删除指定SOCKET的全部数据包
	void RemoveAllPacket(SOCKET hSocket);
	/// 删除全部数据包
	void RemoveAllPacket(void);

public:
	/// 获得一个空闲的数据包
	tcp_packet_t* GetFreePacket(void);

private:
	/// 计算SOCKET所在的分组
	uint32_t CalcSocketGroup(SOCKET hSocket);

private:
	/// TCP数据包缓存
	CCacheTmpl<tcp_packet_t> m_PacketCache;
	/// 数据包节点缓存
	CCacheTmpl<packet_node_t> m_PacketNodeCache;

	/// 发送队列
	CListTmpl<packet_node_t*> m_SendQueue[SEND_QUEUE_GROUP_SIZE];		
	/// 发送队列临界
	CCriticalSection m_SendQueueLock[SEND_QUEUE_GROUP_SIZE];
};

#endif //_WIN32
#endif //__EPOLL_SEND_QUEUE_H__
