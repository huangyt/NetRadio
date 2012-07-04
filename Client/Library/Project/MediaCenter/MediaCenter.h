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
/// \file    : MediaCenter.h
/// \brief   : 媒体中心
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-15
///============================================================================
#ifndef __MEDIA_CENTER_H__
#define __MEDIA_CENTER_H__

#include "IMediaCenter.h"
#include "types\\stdint.h"
#include "CriticalSection.h"

//=============================================================================
/// 媒体模块数量
#define MAX_MEDIA_MODULE_COUNT		64

/// 媒体模块接口数量
#define MAX_MEDIA_INTERFACE_COUNT	64

/// 模块名称长度
#define MAX_MODULE_NAME_SIZE		256

//=============================================================================
/// 模块信息
typedef struct _module_info
{
	TCHAR m_szModuleName[MAX_MODULE_NAME_SIZE];	///< 模块名称
	HANDLE m_hModuleHandle;						///< 模块句柄
	uint32_t nRefCount;							///< 引用计数
}module_info_t;

/// 接口信息
typedef struct _interface_info
{
	GUID  m_InterfaceID;						///< 接口标识
	void* m_pInterface;							///< 接口指针

	TCHAR m_szModuleName[MAX_MODULE_NAME_SIZE];	///< 模块名称
}interface_info_t;

//=============================================================================
class CMediaCenter : public IMediaCenter
{
public:
	CMediaCenter(void);
	~CMediaCenter(void);

public:
	/// 查询接口
	virtual IRESULT QueryInterface(const CLSID& oInterfaceID, 
		void** ppInterface);

	/// 释放接口
	virtual IRESULT ReleaseInterface(const CLSID& oInterfaceID, 
		void* pInterface);

private:
	/// 创建接口
	IRESULT CreateInterface(const TCHAR* pszModuleName, 
		const CLSID& oInterfaceID, void** ppInterface);

	/// 释放接口
	IRESULT DestroyInteface(const TCHAR* pszModuleName, 
		const CLSID& oInterfaceID, void* ppInterface);

	/// 判断接口是否支持
	BOOL IsSupportInterface(const CLSID& oInterfaceID) const;
	/// 获得接口所属的模块名称
	TCHAR* GetModuleName(const CLSID& oInterfaceID) const;

private:
	/// 模块信息数组
	static module_info_t m_arrModuleInfo[MAX_MEDIA_MODULE_COUNT];
	/// 接口信息数组
	static interface_info_t m_arrInterfaceInfo[MAX_MEDIA_INTERFACE_COUNT];
	/// 临界
	mutable CCriticalSection m_ModuleLock;
};

#endif //__MEDIA_CENTER_H__
