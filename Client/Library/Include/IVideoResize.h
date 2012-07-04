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
/// \file    : IVideoResize.h
/// \brief   : 视频重置大小接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-02
///============================================================================
#ifndef __I_VIDEO_RESIZE_H__
#define __I_VIDEO_RESIZE_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"

//=============================================================================
// Resize算法
enum ENUM_RESIZE_ALGORITHM
{
	ENUM_RESIZE_NONE			= 0,	///< 只添加黑边或裁切图像
	ENUM_RESIZE_FAST_BILINEAR	= 1,	///< 快速双线性 - 快速, 质量尚可
	ENUM_RESIZE_BILINEAER		= 2,	///< 双线性 - 品质略好于前一种, 但速度有一点点慢
	ENUM_RESIZE_BICUBIC			= 3,	///< 三线性 - 高品质, 更慢
	ENUM_RESIZE_EXPERIMENTAL	= 4,	///< 三线性的另一种, 处理参数不同
	ENUM_RESIZE_POINT			= 5,	///< 点 - 最快, 品质差
	ENUM_RESIZE_AREA			= 6,	///< 区域 - 区域平均缩放
	//ENUM_RESIZE_BICUBLIN		= 7,	///< 三线混合
	ENUM_RESIZE_GAUSS			= 8,	///< 高斯
	ENUM_RESIZE_SINC			= 9,
	ENUM_RESIZE_LANCZOS			= 10,	///< 米切尔双立方体算法 画质更锐利和真实，适合普通电影
	ENUM_RESIZE_SPLINE			= 11,	
};

// Resize支持的色彩空间类型
enum ENUM_RESIZE_COLOR_SPACE
{
	ENUM_RESIZE_CSP_YV12,
	ENUM_RESIZE_CSP_RGB24,
};

//=============================================================================
// {14DA111E-59D4-4B87-9389-FD360FA53D85}
DEFINE_GUID(CLSID_IVideoResize, 
	0x14da111e, 0x59d4, 0x4b87, 0x93, 0x89, 0xfd, 0x36, 0xf, 0xa5, 0x3d, 0x85);

//=============================================================================
// class IVideoResize
class IVideoResize
{
public:
	/// 创建
	virtual BOOL Create(ENUM_RESIZE_ALGORITHM enAlgorithm, 
		ENUM_RESIZE_COLOR_SPACE enColorSpace,
		uint32_t nSrcWidth, uint32_t nSrcHeight, 
		uint32_t nDstWidth, uint32_t nDstHeight) = 0;
	/// 销毁
	virtual void Destroy(void) = 0;

	/// Resize
	virtual uint32_t Resize(const char* pInFrame, uint32_t nInFrameSize, 
		char* pOutFrame, uint32_t nOutFrameSize) = 0;
};

#endif //__I_VIDEO_RESIZE_H__
