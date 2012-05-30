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
/// \file    : InterfaceDefine.h
/// \brief   : 接口通用定义文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-17
///============================================================================

#ifndef __I_INTERFACE_DEFINES_H__
#define __I_INTERFACE_DEFINES_H__

//=============================================================================
#include <string.h>			// for memcmp

#ifdef _WIN32
#include <InitGuid.h>
#include <Guiddef.h>
#else

/// GUID定义
typedef struct _GUID {
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t  Data4[ 8 ];
}GUID, CLSID;

/// GUID初始化宏
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	extern "C" const GUID name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

/// 判断GUID是否相同
int IsEqualCLSID(GUID rguid1, GUID rguid2)
{
	return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}

#endif	//WIN32

//=============================================================================
// 接口返回值类型定义
typedef long			IRESULT;

// 接口调用成功测试宏
#define ISUCCEEDED(Status)	((IRESULT)(Status) >= 0)

// 接口调用失败测试宏
#define IFAILED(Status)		((IRESULT)(Status)<0)

// 接口返回值定义
#define I_OK			IRESULT(0x00000000L)
#define I_SUCCEED		IRESULT(0x00000001L)

// 接口返回值错误定义
#define I_NOERROR       I_OK

// 意外的失败
#define I_UNEXPECTED	IRESULT(0x8000FFFFL)

// 未实现
#define I_NOTIMPL       IRESULT(0x80004001L)

// 未能分配所需的内存
#define I_OUTOFMEMORY   IRESULT(0x8007000EL)

// 一个或多个参数无效
#define I_INVALIDARG    IRESULT(0x80070057L)

// 不支持此接口
#define I_NOINTERFACE   IRESULT(0x80004002L)

// 无效指针
#define I_POINTER       IRESULT(0x80004003L)

// 无效句柄
#define I_HANDLE        IRESULT(0x80070006L)

// 操作已中止
#define I_ABORT         IRESULT(0x80004004L)

// 未指定的失败
#define I_FAIL          IRESULT(0x80004005L)

// 一般的访问被拒绝错误
#define I_ACCESSDENIED  IRESULT(0x80070005L)

#endif //__I_INTERFACE_DEFINES_H__
