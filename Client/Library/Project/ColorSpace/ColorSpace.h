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
/// \file    : ColorSpace.h
/// \brief   : 色彩空间转换类
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-02
///============================================================================
#ifndef __COLOR_SPACE_H__
#define __COLOR_SPACE_H__

#include "IColorSpace.h"
#include "ColorSpaceLib.h"

//=============================================================================
// class CColorSpaceYV12
class CColorSpaceYV12 : public IColorSpaceYV12
{
public:
	CColorSpaceYV12(void);
	~CColorSpaceYV12(void);

public:
	/// 创建
	virtual BOOL Create(void);
	/// 释放
	virtual void Destroy(void);

public:
	/// 其他色彩空间转换至YV12
	virtual BOOL ColorSpaceToYV12(TO_YV12_TYPE enConverType, 
		uint8_t* src_ptr, int src_stride,
		uint8_t* y_dst, uint8_t* v_dst, uint8_t* u_dst,
		int y_stride, int uv_stride, int width, int height, int vflip);

	/// YV12转换至其他色彩空间
	virtual BOOL YV12ToColorSpace(YV12_TO_TYPE enConverType, 
		uint8_t* dst_ptr, int dst_stride,
		uint8_t* y_src, uint8_t* v_src, uint8_t* u_src,
		int y_stride, int uv_stride, int width, int height, int vflip);

	/// YV12转换至YV12
	virtual BOOL YV12ToYV12(uint8_t* y_dst, uint8_t* u_dst, uint8_t* v_dst,
		int y_dst_stride, int uv_dst_stride,
		uint8_t* y_src, uint8_t* u_src, uint8_t* v_src,
		int y_src_stride, int uv_src_stride,
		int width, int height, int vflip);

private:
	static BOOL m_bIsInit;		///< 是否初始化
	static colorspace_to_yv12_ptr m_ToYv12FuncPtr[END_OF_TO_YV12];
	static yv12_to_colorspace_ptr m_Yv12ToFuncPtr[END_OF_YV12_TO];
};

#endif //__COLOR_SPACE_H__
