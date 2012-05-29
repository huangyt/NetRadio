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
/// \file    : TcpServerBase.h
/// \brief   : TCP服务器积累
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-28
///============================================================================
#ifndef __TCP_SERVER_BASE_H__
#define __TCP_SERVER_BASE_H__

#include "TypeDefine.h"
#include "ITcpNetServer.h"
#include "ListTmpl.h"
#include "Thread.h"
#include "CacheTmpl.h"
#include "EncryptInfo.h"

//=============================================================================
class CTcpServerBase : public ITcpNetServer
{
public:
	CTcpServerBase(void);
	virtual ~CTcpServerBase(void);

public:
	/// 设置加密类型
	virtual BOOL SetEncryptType(ENUM_ENCRYPT_TYPE enType);
	/// 设置加密密钥
	virtual BOOL SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize);

	/// 检查Context是否有效
	virtual BOOL ContextIsValid(const CTcpContext* pContext);
	//断开指定连接
	virtual BOOL ResetContext(CTcpContext* pContext);

protected:
	/// 创建SOCKET套接字
	virtual SOCKET CreateSocket(uint16_t nSvrPort);
	/// 销毁SOCKET套接字
	virtual void DestroySocket(SOCKET hSocket);

protected:
	/// 创建Context
	CTcpContext* CreateContext(void);
	/// 释放Context
	void DestroyContext(CTcpContext* pContext);

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

protected:
	/// 处理接收数据
	BOOL DealRecvData(const char* szDataBuffer, uint32_t nDataSize, 
		CTcpContext *pContext);

protected:
	SOCKET m_hListenSocket;				///< SOCKET套接字
	ITcpServerEvent* m_pTcpEvent;		///< 事件回调接口指针

	/// 发送打包类
	CTcpPackBuffer m_SendPackBuffer;	///< TCP发送打包类

	/// 加密信息
	CEncryptInfo m_EncryptInfo;

	/// Context队列
	CListTmpl<CTcpContext*> m_ContextList;
	/// Context队列临界
	mutable CCriticalSection m_ContextListLock;
};

#endif //__TCP_SERVER_BASE_H__
