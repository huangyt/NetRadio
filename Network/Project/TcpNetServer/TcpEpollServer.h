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

#include "TypeDefine.h"
#include "TcpServerBase.h"
#include "ListTmpl.h"
#include "Thread.h"
#include "CacheTmpl.h"
#include "EpollSendQueue.h"

//=============================================================================
class CTcpEpollServer  : public CTcpServerBase
{
public:
	CTcpEpollServer(void);
	~CTcpEpollServer(void);

public:
	/// 创建TCP服务器
	virtual BOOL Create(uint16_t nSvrPort, ITcpServerEvent* pSvrEvent,
		ENUM_ENCRYPT_TYPE enType);
	/// 销毁TCP服务器
	virtual void Destroy(void);

	/// 发送数据
	virtual uint32_t Send(SOCKET hSocket, const char* szDataBuffer,
		uint16_t nDataSize);

private:
	/// 创建Epoll
	int CreateEpoll(uint32_t nConnectNumber);
	/// 销毁Epoll
	void DestroyEpoll(int hEpollHandle);
	/// EpollAccept
	BOOL EpollAcceptSocket(SOCKET hSocket, const sockaddr_in& SockAddr);

	/// 创建SOCKET套接字
	SOCKET CreateSocket(uint16_t nSvrPort);
	/// 销毁SOCKET套接字
	void DestroySocket(SOCKET hSocket);

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
	int m_hEpollHandle;				///< EPOLL句柄

	CTcpContext m_ListenContext;	///< 监听上下文句柄

	CEpollSendQueue m_SendQueue;	///< 发送队列

	CThread m_EpollWaitThread;		///< Epoll等待线程
	CThread m_CheckThread;			///< 检查线程
};

#endif //_WIN32

#endif //__TCP_EPOLL_SERVER_H__

