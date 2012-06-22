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
/// \file    : VideoEncoder.h
/// \brief   : 视频编编码类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-20
///============================================================================
#ifndef __VIDEO_ENCODER_H__
#define __VIDEO_ENCODER_H__

#include "IVideoCodec.h"
#include "XvidCodec.h"
#include "H264Codec.h"

//=============================================================================
class CVideoEncoder : public IVideoEncoder
{
public:
	CVideoEncoder(void);
	~CVideoEncoder(void);

public:
	/// 创建编码器
	BOOL Create(ENUM_VIDEO_CODEC_TYPE enCodecType);
	/// 销毁编码器
	void Destroy(void);

	/// 设置视频帧信息
	BOOL SetFrameInfo(uint16_t nVideoWidth, 
		uint16_t nVideoHeight);
	/// 获得视频帧信息
	BOOL GetFrameInfo(uint16_t& nVideoWidth, 
		uint16_t& nVideoHeight) const;

	/// 设置视频质量
	BOOL SetVideoQuant(uint16_t nQuant = 85);
	/// 获得视频质量
	uint16_t GetVideoQuant(void) const;

	/// 设置帧率
	BOOL SetFrameRate(uint16_t nFrameRate);
	/// 获得帧率
	uint16_t GetFrameRate(void) const;

	/// 编码
	int32_t Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	ENUM_VIDEO_CODEC_TYPE m_enCodecType;	///< CODEC类型
	CXvidEncoder m_XvidEncoder;				///< XVID编码器
	CH264Encoder m_H264Encoder;				///< H264编码器

	uint16_t m_nVideoWidth;					///< 视频宽度
	uint16_t m_nVideoHeight;				///< 视频高度
	uint16_t m_nVideoQuant;					///< 视频质量
	uint16_t m_nFrameRate;					///< 视频帧率
};

#endif //__VIDEO_ENCODER_H__
