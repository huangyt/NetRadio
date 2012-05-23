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
/// \file    : ITcpNetTrans.h
/// \brief   : TCP网络传输接口类
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-17
///============================================================================

#ifndef __I_TCP_NET_TRANS_H__
#define __I_TCP_NET_TRANS_H__

#include "TypeDefine.h"
#include "IEncrypt.h"
#include "InterfaceDefine.h"

//=============================================================================
#ifdef	_WIN32
#ifndef _TCPNETTRANS_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"TcpNetTransD.lib")
#else
#pragma comment(lib,"TcpNetTrans.lib")
#endif	//_TCPNETTRANS_EXPORT
#endif	//_TCPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
struct sockaddr_in;

//=============================================================================
/// TCP网络事件
enum ENUM_TCP_NET_EVENT
{
	TCP_NET_EVENT_NONE,			///< 占位用 无意义
	TCP_NET_EVENT_OPEN,			///< 打开
	TCP_NET_EVENT_CLOSE,		///< 关闭
	TCP_NET_EVENT_CONNECT,		///< 连接成功
	TCP_NET_EVENT_DISSCONNECT,	///< 连接断开
	TCP_NET_EVENT_RECV,			///< 接收数据
	TCP_NET_EVENT_SEND,			///< 发送数据 
};

//=============================================================================
// class ITcpNetEvent
class ITcpNetEvent
{
public:
	// 网络事件回调
	virtual void OnEvent(ENUM_TCP_NET_EVENT enEvent) = 0;
};

//=============================================================================
// ITcpNetTrans接口标示
// {78353EBC-EF1D-4E68-8E1A-558B70B36223}
DEFINE_GUID(CLSID_ITcpNetTrans, 
	0x78353ebc, 0xef1d, 0x4e68, 0x8e, 0x1a, 0x55, 0x8b, 0x70, 0xb3, 0x62, 0x23);

//=============================================================================
// class ITcpNetTrans
class ITcpNetTrans
{
public:
	/// 创建实例
	virtual BOOL Open(ITcpNetEvent* pNetEvent, uint16_t nPort = 0);
	/// 是否创建
	virtual BOOL IsOpen(void) const = 0;
	/// 关闭
	virtual void Close(void) = 0;

	/// 设置加密类型
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES) = 0;
	/// 设置加密密钥
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize) = 0;

	/// 连接到对方
	virtual BOOL Connet(const char* szSvrAddr, uint16_t nSvrPort) = 0;
	/// 是否连接上
	virtual BOOL IsConnet(void) const = 0;

	/// 发送数据包(异步方式)
	virtual uint32_t Send(const char* szBuffer, uint32_t nBufferSize) = 0;
	/// 接收数据包
	virtual uint32_t Recv(char* szBuffer, uint32_t nBufferSize) = 0;
};

//=============================================================================
// API
/// 创建TCP传输接口
IRESULT CreateTcpNetTrans(const CLSID& oInterfaceID, void** ppInterface);

/// 释放TCP传输接口
IRESULT DestroyTcpNetTrans(const CLSID& oInterfaceID, void* pInterface);


#endif //__I_TCP_NET_TRANS_H__