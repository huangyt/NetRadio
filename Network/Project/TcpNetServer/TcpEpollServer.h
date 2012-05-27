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
/// \file    : TcpEpollServer.h
/// \brief   : TCP完成端口服务器
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-24
///============================================================================
#ifndef __TCP_EPOLL_SERVER_H__
#define __TCP_EPOLL_SERVER_H__

#ifndef _WIN32
#include <time.h>
#include <stdlib.h>

#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/epoll.h>
#include <sys/resource.h>

#else
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WinSock2.h>
#include <MSWSock.h>
#endif

#include "TypeDefine.h"
#include "ITcpNetServer.h"
#include "ListTmpl.h"
#include "Thread.h"
#include "CacheTmpl.h"

//=============================================================================
class CTcpEpollServer  : public ITcpNetServer
{
public:
	CTcpEpollServer(void);
	~CTcpEpollServer(void);

public:
	/// 创建TCP服务器
	virtual BOOL Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent);
	/// 销毁TCP服务器
	virtual void Destroy(void);

	/// 检查Context是否有效
	virtual BOOL ContextIsValid(const CTcpContext* pContext);
	//断开指定连接
	virtual BOOL ResetContext(CTcpContext* pContext);

	/// 发送数据
	virtual uint32_t Send(SOCKET hSocket, const char* szDataBuffer, 
		uint16_t nDataSize);

private:
	/// 创建Epoll
	int CreateEpoll(void);
	/// 销毁Epoll
	void DestroyEpoll(int hEpollHandle);
	/// EpollAccept
	BOOL EpollAcceptSocket(SOCKET hSocket, const sockaddr_in& SockAddr);

	/// 创建SOCKET套接字
	SOCKET CreateSocket(uint16_t nSvrPort);
	/// 销毁SOCKET套接字
	void DestroySocket(SOCKET hSocket);

	/// 添加Context
	BOOL AddTcpContext(CTcpContext* pContext);
	/// 删除Context
	BOOL RemoveTcpContext(CTcpContext* pContext);
	/// 关闭所有Context
	void CloseAllContext(void);
	/// 关闭无效链接
	void CheckInvalidContext(void);
	/// 获得连接数量
	uint32_t GetTcpContextCount(void) const;

private:
	/// 完成端口线程函数
	void EpollWaitFunc(void);
	/// 连接检查线程函数
	void ConnectCheckFunc(void);

	/// 完成端口线程
	static unsigned int EpollWaitThread(void *pParam);
	/// 连接检查线程
	static unsigned int ConnectCheckThread(void* pParam);

private:
	SOCKET m_hListenSocket;			///< 监听端口
	int	   m_hEpollHandle;			///< EPOLL句柄

	ITcpServerEvent* m_pEvent;		///< 事件回调接口指针

	CTcpContext m_ListenContext;	///< 监听上下文句柄

	/// Context队列
	CListTmpl<CTcpContext*> m_ContextList;
	/// Context队列临界
	mutable CCriticalSection m_ContextListLock;

	/// TCP数据包缓存
	CCacheTmpl<tcp_packet_t> m_PacketCache;

	///// 连接队列
	//CListTmpl<OVERLAPPEDPLUS*> m_AcceptExList;
	///// 连接队列临界
	//mutable CCriticalSection m_AcceptExListLock;

	CThread m_EpollWaitThread;		///< Epoll等待线程
	CThread m_CheckThread;			///< 检查线程
};

//#endif //_WIN32

#endif //__TCP_EPOLL_SERVER_H__

