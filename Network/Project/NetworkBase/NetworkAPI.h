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
/// \file    : NetworkAPI.h
/// \brief   : 网络操作函数头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-21
///============================================================================
#ifndef __NETWORK_API_H__
#define __NETWORK_API_H__

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

#include <time.h>
#include "TypeDefine.h"

//=============================================================================
#ifndef _WIN32
/// 得到毫秒级系统时间
uint32_t GetTickCount(void);
#endif

/// 获得时间的字符串形式
const char* GetTimeStr(time_t* pTime);

//=============================================================================
//SOCKET相关函数
/// 获得SOCKET地址
uint32_t GetSocketAddr(const sockaddr_in& addr);

/// 设置SOCKET地址
void SetSocketAddr(sockaddr_in& addr, uint32_t nAddr);

/// 获得IP地址
const char* GetIPAddr(uint32_t nAddr);

/// 判断是否为IP地址
BOOL IsIPAddr(const char* szAddr);

/// 获得IDC地址
long GetIdcIP(uint32_t nIPAddr);

/// 判断IP地址是否为同一个IDC
BOOL IsSameIdc(uint32_t nIPAddr1, uint32_t nIPAddr2);

/// 获得本地IP地址列表
uint16_t GetLocalIP(struct sockaddr_in* pAddr, uint16_t nAddrSize);

#endif //__NETWORK_API_H__

