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
/// \file    : Encrypt.h
/// \brief   : 加解密类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-17
///============================================================================
#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

#include "IEncrypt.h"
#include "rc6/rc6.h"
#include "aes/aes.h"
#include "3des/des.h"
#include "rsa/rsaref.h"

//=============================================================================
class CEncrypt : public IEncrypt
{
public:
	CEncrypt(void);
	~CEncrypt(void);

public:
	//设置加密密钥
	BOOL SetEncryptKey(const char* szEncryKey, int nKeyLen);

public:
	//加密
	BOOL Encrypt(ENUM_ENCRYPT_TYPE enEncryptType, 
		const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);

	//解密
	BOOL Decrypt(ENUM_ENCRYPT_TYPE enEncryptType, 
		const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);

private:
	//空加密解密
	BOOL NullEncrypt(const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);

	BOOL NullDecrypt(const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);

	//rc6
	BOOL Rc6Encrypt(const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);
	BOOL Rc6Decrypt(const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);

	//aes
	BOOL AesEncrypt(const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);
	BOOL AesDecrypt(const char* szInDataBuff, size_t nInDataSize, 
		char* szOutDataBuff, size_t& nOutDataSize);

private:	
	//加密
	typedef BOOL (CEncrypt:: *EncryptFunc)(const char* szInDataBuff, 
		size_t nInDataSize, char* szOutDataBuff, size_t& nOutDataSize);
	//解密
	typedef BOOL (CEncrypt:: *DecryptFunc)(const char* szInDataBuff, 
		size_t nInDataSize, char* szOutDataBuff, size_t& nOutDataSize);

	// 加密函数数组
	EncryptFunc m_szEncryFun[ENUM_ENCRYPT_COUNT];
	// 解密函数数组
	DecryptFunc m_szDecryFun[ENUM_ENCRYPT_COUNT];

	rc6_ctx     m_stRc6Ctx;           //RC6加解密CONTEX
	aes_ctx     m_stAesEncryCtx;      //AES加密CONTEX
	aes_ctx     m_stAesDecryCtx;      //AES解密CONTEX
};

#endif //__ENCRYPT_H__
