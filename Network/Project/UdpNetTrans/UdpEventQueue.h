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
/// \file    : UdpEventQueue.h
/// \brief   : Udp事件队列
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-23
///============================================================================
#ifndef __UDP_EVENT_QUEUE_H__
#define __UDP_EVENT_QUEUE_H__

#include "IUdpNetTrans.h"
#include "TypeDefine.h"
#include "ListTmpl.h"
#include "CriticalSection.h"

//=============================================================================
// class CUdpEventQueue
class CUdpEventQueue
{
public:
	CUdpEventQueue(void);
	~CUdpEventQueue(void);

public:
	/// 添加事件
	BOOL PushEvent(ENUM_UDP_NET_EVENT enEvent);
	/// 删除事件
	ENUM_UDP_NET_EVENT PopEvent(void);
	/// 删除全部事件
	void RemoveAll(void);
	/// 判断队列是否为空
	BOOL IsEmpty(void) const;

private:
	CListTmpl<ENUM_UDP_NET_EVENT> m_EventList;
	mutable CCriticalSection m_ListLock;
};

#endif //__UDP_EVENT_QUEUE_H__
