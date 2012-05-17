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
/// \file    : IEncrypt.h
/// \brief   : 加解密库接口文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-17
///============================================================================

#ifndef __I_ENCRYPT_H__
#define __I_ENCRYPT_H__

#include "TypeDefine.h"

//=============================================================================
enum ENUM_ENCRYPT_TYPE
{
	ENUM_ENCRYPT_NONE,				///< 默认值
	ENUM_ENCRYPT_AES,				///< AES
	ENUM_ENCRYPT_RC6,				///< RC6

	ENUM_ENCRYPT_COUNT,				///< 加解密类型数量
};

//=============================================================================
// class IEncrypt
class IEncrypt
{
public:
	//设置加密密钥
	virtual BOOL SetEncryptKey(const char* szEncryKey, int nKeyLen) = 0;

public:
	//加密
	virtual BOOL Encrypt(ENUM_ENCRYPT_TYPE enEncryptType, 
		const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize) = 0;

	//解密
	virtual BOOL Decrypt(ENUM_ENCRYPT_TYPE enEncryptType, 
		const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize) = 0;
};

//=============================================================================
/// 创建加解密类指针
IEncrypt* CreateEncrypt(void);

/// 释放加解密类指针
void DestroyEncrypt(IEncrypt* pEncrypt);

#endif //__I_ENCRYPT_H__
