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
/// \file    : StdSerialize.h
/// \brief   : 标准序列化头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-16
///============================================================================

#include "TypeDefine.h"

//=============================================================================

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef	_WIN32
#ifndef _SERIALIZE_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"libSerializeD.lib")
#else
#pragma comment(lib,"libSerialize.lib")
#endif	//_TCPNETTRANS_EXPORT
#endif	//_TCPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
// CStdSerialize
class CStdSerialize
{
public:
	enum ENUM_TYPE{LOAD, STORE};

public:
	CStdSerialize(char* apBuffer, uint32_t alBufferSize, ENUM_TYPE aenType);
	virtual ~CStdSerialize(void);

public:
	//序列化int8_t类型的值
	virtual void Serialize(int8_t& nValue);

	//序列化uint8_t类型的值
	virtual void Serialize(uint8_t& nValue);

	//序列化int16_t类型的值
	virtual void Serialize(int16_t& nValue);

	//序列化uint16_t类型的值
	virtual void Serialize(uint16_t& nValue);

	//序列化int32_t类型的值
	virtual void Serialize(int32_t& nValue);

	//序列化uint32_t类型的值
	virtual void Serialize(uint32_t& nValue);

	//序列化int64_t类型的值
	virtual void Serialize(int64_t& nValue);

	//序列化uint64_t类型的值
	virtual void Serialize(uint64_t& nValue);

	//序列化float类型的值
	virtual void Serialize(float& fValue);

	//序列化double类型的值
	virtual void Serialize(double& fValue);

	//序列化串类型的值(以\0结尾的字符串)
	virtual void Serialize(char* pValue, uint16_t nMaxSize);

	// 序列化二进制流
	virtual void Serialize(char* pValue, uint16_t nSize, uint16_t nMaxSize);

	//取得打包长度
	virtual long GetDataSize(void) const;
	
	// 取得序列化类型
	virtual ENUM_TYPE GetType(void) const;

protected:
	ENUM_TYPE m_enType;			///< 序列化类型
	uint32_t  m_lDataSize;		///< 数据长度
	char*	  m_pBuffer;		///< 缓冲区指针
	uint32_t  m_lBufferSize;	///< 缓冲区长度
};

