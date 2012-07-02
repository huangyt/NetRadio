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
/// \file    : IColorSpace.h
/// \brief   : 色彩空间转换接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-02
///============================================================================
#ifndef __I_COLOR_SPACE_H__
#define __I_COLOR_SPACE_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"

//=============================================================================
// {7FF48B6C-2E16-47FD-8C07-80EDBF04A1FA}
DEFINE_GUID(CLSID_IColorSpaceYV12, 
	0x7ff48b6c, 0x2e16, 0x47fd, 0x8c, 0x7, 0x80, 0xed, 0xbf, 0x4, 0xa1, 0xfa);

//=============================================================================
/// 转换至YV12类型
enum TO_YV12_TYPE
{
	NONE_TO_YV12		= 0,			///< 默认值，无意义
	RGB555_TO_YV12		= 1,
	RGB565_TO_YV12		= 2,
	RGB_TO_YV12			= 3,
	BGR_TO_YV12			= 4,
	BGRA_TO_YV12		= 5,
	ABGR_TO_YV12		= 6,
	RGBA_TO_YV12		= 7,
	ARGB_TO_YV12		= 8,
	YUYV_TO_YV12		= 9,
	UYVY_TO_YV12		= 10,

	RGB555I_TO_YV12		= 11,
	RGB565I_TO_YV12		= 12,
	RGBI_TO_YV12		= 13,
	BGRI_TO_YV12		= 14,
	BGRAI_TO_YV12		= 15,
	ABGRI_TO_YV12		= 16,
	RGBAI_TO_YV12		= 17,
	ARGBI_TO_YV12		= 18,
	YUYVI_TO_YV12		= 19,
	UYVYI_TO_YV12		= 20,

	END_OF_TO_YV12,						///< 结束标志
};

/// YUV转换至类型
enum YV12_TO_TYPE
{
	YV12_TO_NONE		= 0,			///< 默认值，无意义
	YV12_TO_RGB555		= 1,
	YV12_TO_RGB565		= 2,
	YV12_TO_RGB			= 3,
	YV12_TO_BGR			= 4,
	YV12_TO_BGRA		= 5,
	YV12_TO_ABGR		= 6,
	YV12_TO_RGBA		= 7,
	YV12_TO_ARGB		= 8,
	YV12_TO_YUYV		= 9,
	YV12_TO_UYVY		= 0,

	YV12_TO_RGB555I		= 11,
	YV12_TO_RGB565I		= 12,
	YV12_TO_RGBI		= 13,
	YV12_TO_BGRI		= 14,
	YV12_TO_BGRAI		= 15,
	YV12_TO_ABGRI		= 16,
	YV12_TO_RGBAI		= 17,
	YV12_TO_ARGBI		= 18,
	YV12_TO_YUYVI		= 19,
	YV12_TO_UYVYI		= 20,

	END_OF_YV12_TO,						///< 结束标志
};

//=============================================================================
// class IColorSpaceYV12
class IColorSpaceYV12
{
public:
	/// 创建
	virtual BOOL Create(void) = 0;
	/// 释放
	virtual void Destroy(void) = 0;

public:
	/// 其他色彩空间转换至YV12
	virtual BOOL ColorSpaceToYV12(TO_YV12_TYPE enConverType, 
		uint8_t* src_ptr, int src_stride,
		uint8_t* y_dst, uint8_t* v_dst, uint8_t* u_dst,
		int y_stride, int uv_stride, int width, int height, int vflip) = 0;

	/// YV12转换至其他色彩空间
	virtual BOOL YV12ToColorSpace(YV12_TO_TYPE enConverType, 
		uint8_t* dst_ptr, int dst_stride,
		uint8_t* y_src, uint8_t* v_src, uint8_t* u_src,
		int y_stride, int uv_stride, int width, int height, int vflip) = 0;

	/// YV12转换至YV12
	virtual BOOL YV12ToYV12(uint8_t* y_dst, uint8_t* u_dst, uint8_t* v_dst,
		int y_dst_stride, int uv_dst_stride,
		uint8_t* y_src, uint8_t* u_src, uint8_t* v_src,
		int y_src_stride, int uv_src_stride,
		int width, int height, int vflip) = 0;
};

#endif //__I_COLOR_SPACE_H__
