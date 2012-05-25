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
/// \file    : TcpIocpServer.h
/// \brief   : TCP完成端口服务器
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-24
///============================================================================
#ifndef __TCP_IOCP_SERVER_H__
#define __TCP_IOCP_SERVER_H__

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WinSock2.h>
#include <MSWSock.h>

#include "TypeDefine.h"
#include "ITcpNetServer.h"
#include "ListTmpl.h"
#include "Thread.h"
#include "CacheTmpl.h"

//=============================================================================
// 套接字IO动作类型
enum ENUM_IO_TYPE
{
	IO_ACCEPT,
	IO_READ,
	IO_WRITE,
};

// 单个IO数据结构
struct OVERLAPPEDPLUS 
{
	OVERLAPPED m_Overlapped;					// 每一个重叠网络操作的重叠结构
	SOCKET	m_hSocket;							// 这个网络操作所使用的Socket
	ENUM_IO_TYPE m_enIOType;					// 标识网络操作的类型(对应上面的枚举)
	WSABUF	m_wsaBuffer;						// WSA类型的缓冲区
	char	m_szBuffer[MAX_PACK_BUFFER_SIZE];	// 这个是WSABUF里具体存字符的缓冲区
	uint16_t m_nBufferSize;						// 缓冲区长度
	uint64_t m_nOverlapKey;						// 重叠结构标识
};

//=============================================================================
class CTcpIocpServer : public ITcpNetServer
{
public:
	CTcpIocpServer(void);
	~CTcpIocpServer(void);

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
	/// 创建完成端口
	HANDLE CreateCompletePort(void);
	/// 销毁完成端口
	void DestroyCompletePort(HANDLE hHandle);

	/// 创建SOCKET套接字
	SOCKET CreateSocket(uint16_t nSvrPort);
	/// 销毁SOCKET套接字
	void DestroySocket(SOCKET hSocket);

	/// 创建ACCEPT事件
	HANDLE CreateAcceptEvent(SOCKET hSocket);
	/// 销毁ACCEPT事件
	void DestroyAcceptEvent(HANDLE hEvent);

	/// 加载AcceptEx函数指针
	LPFN_ACCEPTEX GetAcceptExPtr(SOCKET hSocket);
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockAddrs(SOCKET hSocket);

private:
	/// 申请重叠结构内存
	OVERLAPPEDPLUS* MallocOverlap(ENUM_IO_TYPE enIOType);
	/// 释放重叠结构内存
	void FreeOverlap(OVERLAPPEDPLUS* pOverlap);

	/// 添加AcceptEx队列
	BOOL AddAcceptEx(OVERLAPPEDPLUS* pOverlap);
	/// 删除AcceptEx队列
	BOOL RemoveAcceptEx(OVERLAPPEDPLUS* pOverlap);
	/// 关闭AcceptEx队列中的全部SOCKET
	void CloseAllAcceptExSocket(void);
	/// 检查AcceptEx队列的超时数据
	void CheckAcceptExTimeout(void);
	/// 获得AcceptEx队列尺寸
	uint32_t GetAcceptExCount(void) const;

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
	/// 处理接收数据
	BOOL DealRecvData(uint32_t nRecvDataLen, CTcpContext *pContext,
		OVERLAPPEDPLUS* pOverlapPlus);

	/// 完成端口线程函数
	void CompletePortFunc(void);
	/// 连接检查线程函数
	void ConnectCheckFunc(void);

	/// 完成端口线程
	static unsigned int CompletePortThread(void *pParam);
	/// 连接检查线程
	static unsigned int ConnectCheckThread(void* pParam);

private:
	HANDLE m_hIocp;					///< 完成端口句柄

	SOCKET m_hListenSocket;			///< SOCKET套接字
	ITcpServerEvent* m_pEvent;		///< 事件回调接口指针

	HANDLE m_hAcceptEvent;			///< 连接事件
	CTcpContext m_oListenContext;	/// 监听端口的上下文句柄

	/// Context队列
	CListTmpl<CTcpContext*> m_ContextList;
	/// Context队列临界
	mutable CCriticalSection m_ContextListLock;

	/// 连接队列
	CListTmpl<OVERLAPPEDPLUS*> m_AcceptExList;
	/// 连接队列临界
	mutable CCriticalSection m_AcceptExListLock;

	// AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	LPFN_ACCEPTEX m_lpfnAcceptEx;                
	LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockAddrs; 

	CThread m_CompletePotrThread;	///< 完成端口线程
	CThread m_CheckThread;			///< 检查线程

	/// 重叠结构缓存
	CCacheTmpl<OVERLAPPEDPLUS> m_OverloapCache;	

	//Write overlap
	WSAOVERLAPPED m_oWriteOverlap;      
	CCriticalSection m_oWriteOverlapLock;
};

#endif //_WIN32
#endif //__TCP_IOCP_SERVER_H__
