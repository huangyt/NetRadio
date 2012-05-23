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
/// \file    : TcpPacketQueue.h
/// \brief   : TCP数据包队列
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-18
///============================================================================
#ifndef __TCP_PACKET_QUEUE_H__
#define __TCP_PACKET_QUEUE_H__

#include "TypeDefine.h"
#include "ListTmpl.h"
#include "CriticalSection.h"

//=============================================================================
class CTcpPacketQueue
{
public:
	CTcpPacketQueue(void);
	~CTcpPacketQueue(void);

public:
	/// 添加数据包
	uint32_t AddTail(const tcp_packet_t* pPacket);

	/// 获得节点
	tcp_packet_t* GetHead(void);

	/// 删除节点
	tcp_packet_t* RemoveHead(void);

	/// 获得节点数量
	uint32_t GetCount(void) const;

	/// 判断队列是否为空
	BOOL IsEmpty(void) const;

	// 删除所有节点
	void RemoveAll(void);

private:
	CListTmpl<tcp_packet_t*> m_PacketQueue;	///< 数据包队列
};

#endif //__TCP_PACKET_QUEUE_H__
