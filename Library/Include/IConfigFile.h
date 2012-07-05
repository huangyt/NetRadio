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
/// \file    : IConfigFile.h
/// \brief   : 配置文件接口文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-17
///============================================================================

#ifndef __I_CONFIG_FILE_H__
#define __I_CONFIG_FILE_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"

//=============================================================================
#ifdef	_WIN32
#ifndef _CONFIG_FILE_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"libConfigFileD.lib")
#pragma message("LINK libConfigFileD.lib")
#else
#pragma comment(lib,"libConfigFile.lib")
#pragma message("LINK libConfigFileD.lib")
#endif	//_DEBUG
#endif	//_CONFIG_FILE_EXPORT
#endif	//_WIN32

//=============================================================================
enum ENUM_CONFIG_FILE_TYPE
{
	CONFIG_FILE_INI = 0,			///< INI配置文件
	CONFIG_FILE_XML = 1,			///< XML配置文件
};

//=============================================================================
// {641E7F06-9FBB-4A00-85AC-D07B2EF72A3B}
DEFINE_GUID(CLSID_IConfigFile, 
	0x641e7f06, 0x9fbb, 0x4a00, 0x85, 0xac, 0xd0, 0x7b, 0x2e, 0xf7, 0x2a, 0x3b);

//=============================================================================
class IConfigFile
{
public:
	/// 打开
	virtual BOOL OpenFile(const char* szPathName, 
		ENUM_CONFIG_FILE_TYPE enFileType = CONFIG_FILE_INI) = 0;
	/// 关闭
	virtual BOOL CloseFile(void) = 0;
	/// 保存
	virtual BOOL SaveFile(void) = 0;

public:
	/// 获得配置
	virtual const char* GetValueString(const char* szSection, 
		const char* szKey, const char* szDefaultValue) = 0;

	/// 设置配置
	virtual BOOL SetValueString(const char* szSection, 
		const char* szKey, const char* szValue) = 0;

	/// 获得配置
	virtual int32_t GetValueInt(const char* szSection, 
		const char* szKey, int32_t nDefaultValue) = 0;

	/// 设置配置
	virtual BOOL SetValueInt(const char* szSection, 
		const char* szKey, int32_t nValue) = 0;

	/// 获得配置
	virtual double GetValueDouble(const char* szSection, 
		const char* szKey, double nDefaultValue) = 0;

	/// 设置配置
	virtual BOOL SetValueDouble(const char* szSection, 
		const char* szKey, double nValue) = 0;
};

//=============================================================================
/// 创建配置文件类指针
IRESULT CreateConfigFile(const CLSID& oInterfaceID, void** ppInterface);

/// 释放配置文件类指针
IRESULT DestroyConfigFile(const CLSID& oInterfaceID, void* ppInterface);

#endif //__I_CONFIG_FILE_H__
