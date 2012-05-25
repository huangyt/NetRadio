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
/// \file    : UdpNetTrans.h
/// \brief   : Udp网络传输类
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-23
///============================================================================
#ifndef __UDP_NET_TRANS_H__
#define __UDP_NET_TRANS_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <WinSock2.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h>
#include <execinfo.h>
#include <errno.h>
#endif

#include "TypeDefine.h"
#include "Thread.h"
#include "Event.h"
#include "CacheTmpl.h"
#include "IUdpNetTrans.h"
#include "UdpPackBuffer.h"
#include "UdpPacketQueue.h"
#include "UdpEventQueue.h"

//=============================================================================
/// UDP_NET_STATE
enum ENUM_UDP_STATE
{
	ENUM_UDP_STATE_NONE,			///< 占位用 无意义
	ENUM_UDP_STATE_OPEN,			///< 打开
	ENUM_UDP_STATE_CLOSE,			///< 关闭
};

//=============================================================================
class CUdpNetTrans : public IUdpNetTrans
{
public:
	CUdpNetTrans(void);
	~CUdpNetTrans(void);

public:
	/// 创建实例
	virtual BOOL Open(IUdpNetEvent* pNetEvent, const char* szLocalAddr = NULL, 
		uint16_t nPort = 0);
	/// 是否创建
	virtual BOOL IsOpen(void) const;
	/// 关闭
	virtual void Close(void);

	/// 设置加密类型
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES);
	/// 设置加密密钥
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize);

	/// 发送数据包(异步方式)
	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		const char* szSvrAddr, uint16_t nSvrPort);

	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		const sockaddr_in* pSvrAddr);

	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize, 
		uint32_t nSvrAddr, uint16_t nSvrPort);

	/// 接收数据包
	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		char* szSvrAddr, uint16_t& nSvrPort);

	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		sockaddr_in* pSvrAddr);

	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize, 
		uint32_t& nSvrAddr, uint16_t& nSvrPort);

private:
	/// 设置UDP状态
	ENUM_UDP_STATE SetUdpState(ENUM_UDP_STATE enNewState);
	/// 获得UDP状态
	ENUM_UDP_STATE GetUdpState(void) const;

	/// UDP收包处理函数
	void UdpRecvThreadFunc(void);
	/// UDP发包处理函数
	void UdpSendThreadFunc(void);
	/// UDP状态检查函数
	void UdpCheckStateFunk(void);

private:
	//UDP收包处理线程
	static unsigned int UdpRecvThread(void *pParam);
	//UDP发包处理线程
	static unsigned int UdpSendThread(void *pParam);
	//UDP状态检查线程
	static unsigned int UdpCheckStateThread(void *pParam);

private:
	ENUM_ENCRYPT_TYPE m_enEncryptType;	///< 加密类型
	IUdpNetEvent* m_pNetEvent;			///< 网络事件回调类指针
	SOCKET m_hSocket;					///< SOCKET句柄

	ENUM_UDP_STATE m_enUdpState;		///< 状态机

	CThread	m_CheckStateThread;			///< 状态检查线程
	CThread m_SendThread;				///< 发送线程
	CThread m_RecvThread;				///< 接收线程

	CUdpEventQueue m_EventQueue;		///< 事件队列

	CEvent	m_CheckEvent;				///< 状态检查事件
	CEvent	m_SendEvent;				///< 发包事件

	CUdpPackBuffer m_PackBuffer;		///< UDP打包
	CCacheTmpl<udp_packet_t> m_PacketCache;		///< UDP数据包缓存

	CUdpPacketQueue m_SendPacketQueue;	///< UDP数据包发送队列
	CUdpPacketQueue m_RecvPacketQueue;	///< UDP数据包接收队列

	CCriticalSection m_oSendQueueLock;	///< 发送队列临界
	CCriticalSection m_oRecvQueueLock;	///< 接收队列临界
};

#endif //__UDP_NET_TRANS_H__
