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
/// \file    : UdpPacketCache.h
/// \brief   : Udp数据包缓存
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-18
///============================================================================
#ifndef __UDP_PACKET_CACHE_H__
#define __UDP_PACKET_CACHE_H__

#include "TypeDefine.h"
#include "ListTmpl.h"
#include "CriticalSection.h"

//=============================================================================
// 最小UDP包缓存尺寸
#define MIN_UDP_PACKET_CACHE_SIZE	1024

//=============================================================================
class CUdpPacketCache
{
public:
	CUdpPacketCache(uint32_t nCacheSize = MIN_UDP_PACKET_CACHE_SIZE);
	~CUdpPacketCache(void);

public:
	/// 分配一个数据包
	udp_packet_t* MallocPacket(void);
	/// 释放一个数据包
	void FreePacket(udp_packet_t* pPacket);

private:
	CListTmpl<udp_packet_t*> m_PacketCache;	///< 缓存链表

	block_node_t* m_pBlockCache;			///< 块缓存
	uint32_t m_nBlockSize;					///< 块缓存尺寸

	CCriticalSection m_oCacheLock;
};

#endif //__UDP_PACKET_CACHE_H__
