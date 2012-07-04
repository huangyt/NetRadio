#pragma once/******************************************************************************
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
/// \file    : VideoResize.h
/// \brief   : 视频重置大小类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-02
///============================================================================
#ifndef __VIDEO_RESIZE_H__
#define __VIDEO_RESIZE_H__

#include "IVideoResize.h"
#include "mplayer\\mplayer.h"

//=============================================================================
class CVideoResize : public IVideoResize
{
public:
	CVideoResize(void);
	~CVideoResize(void);

public:
	/// 创建
	BOOL Create(ENUM_RESIZE_ALGORITHM enAlgorithm, 
		ENUM_RESIZE_COLOR_SPACE enColorSpace,
		uint32_t nSrcWidth, uint32_t nSrcHeight, 
		uint32_t nDstWidth, uint32_t nDstHeight);
	/// 销毁
	void Destroy(void);

	/// Resize
	uint32_t Resize(const char* pInFrame, uint32_t nInFrameSize, 
		char* pOutFrame, uint32_t nOutFrameSize);

private:
	int GetSWSMethod(ENUM_RESIZE_ALGORITHM enAlgorithm);
	int GetSWSColorSpace(ENUM_RESIZE_COLOR_SPACE enColorSpace);

	/// ResizeRGB24
	uint32_t ResizeRGB24(const char* pInFrame, uint32_t nInFrameSize, 
		char* pOutFrame, uint32_t nOutFrameSize);

	/// ResizeRGB24
	uint32_t ResizeYV12(const char* pInFrame, uint32_t nInFrameSize, 
		char* pOutFrame, uint32_t nOutFrameSize);

private:
	SwsContext* m_hContext;

	ENUM_RESIZE_COLOR_SPACE m_enColorSpace;
	uint32_t m_nSrcWidth;
	uint32_t m_nSrcHeight;
	uint32_t m_nDstWidth;
	uint32_t m_nDstHeight;
};

#endif //__VIDEO_RESIZE_H_
