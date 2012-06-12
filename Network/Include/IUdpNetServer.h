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
/// \file    : IUdpNetServer.h
/// \brief   : UDP网络服务器接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-28
///============================================================================
#ifndef	__I_UDP_NET_SERVER_H__
#define __I_UDP_NET_SERVER_H__

#include "TypeDefine.h"
#include "IEncrypt.h"
#include "InterfaceDefine.h"
#include "TcpPackBuffer.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <WinSock2.h>
#include <MSWSock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#endif

//=============================================================================
#ifdef	_WIN32
#ifndef _UDPNETTRANS_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"UdpNetServerD.lib")
#else
#pragma comment(lib,"UdpNetServer.lib")
#endif	//_DEBUG
#endif	//_UDPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
/// UDP服务器数据加密密钥长度
#define UDP_SVR_ENCRYPT_KEY_SIZE		16

//=============================================================================
// class IUdpServerEvent
class IUdpServerEvent
{
public:
	// 收到数据
	virtual void OnRecvData(char* apPacketData, uint32_t nPacketSize, 
		const sockaddr_in* apPeerAddr) = 0;
};

//=============================================================================
// class IUdpNetServer
class IUdpNetServer
{
public:
	/// 创建TCP服务器
	virtual BOOL Create(uint16_t nSvrPort, IUdpServerEvent* pSvrEvent, 
		ENUM_ENCRYPT_TYPE enType = ENUM_ENCRYPT_AES) = 0;
	/// 销毁TCP服务器
	virtual void Destroy(void) = 0;

	/// 设置加密类型
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType) = 0;
	/// 设置加密密钥
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize) = 0;

	/// 发送数据
	virtual uint32_t SendTo(SOCKET hSocket, const char* szDataBuffer, 
		const sockaddr_in* apPeerAddr) = 0;
};

#endif //__I_UDP_NET_SERVER_H__
