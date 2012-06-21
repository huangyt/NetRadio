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
/// \file    : IVideoCodec.h
/// \brief   : 视频编解码接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-20
///============================================================================
#ifndef __I_VIDEO_CODEC_H__
#define __I_VIDEO_CODEC_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"

//=============================================================================
/// 编码器类型
enum ENUM_VIDEO_CODEC_TYPE
{
	ENUM_VIDEO_CODEC_XVID	= 0,			///< XVID
	ENUM_VIDEO_CODEC_H264	= 1,			///< H264
};

/// 默认视频质量
#define DEFAULT_CODEC_VIDEO_QUANT	100
/// 默认视频帧率
#define DEFAULT_CODEC_FRAME_RATE	25

//=============================================================================
// {0A6ED810-2FD2-4566-8AF7-0A4A296D4CBD}
DEFINE_GUID(CLSID_IVideoEncoder, 
	0xa6ed810, 0x2fd2, 0x4566, 0x8a, 0xf7, 0xa, 0x4a, 0x29, 0x6d, 0x4c, 0xbd);

//=============================================================================
/// 视频编码器
class IVideoEncoder
{
public:
	/// 创建编码器
	virtual BOOL Create(ENUM_VIDEO_CODEC_TYPE enCodecType) = 0;
	/// 销毁编码器
	virtual void Destroy(void) = 0;

	/// 设置视频帧信息
	virtual BOOL SetFrameInfo(uint16_t nVideoWidth, 
		uint16_t nVideoHeight) = 0;
	/// 获得视频帧信息
	virtual BOOL GetFrameInfo(uint16_t& nVideoWidth, 
		uint16_t& nVideoHeight) const = 0;

	/// 设置视频质量
	virtual BOOL SetVideoQuant(uint16_t nQuant = 85) = 0;
	/// 获得视频质量
	virtual uint16_t GetVideoQuant(void) const = 0;

	/// 设置帧率
	virtual BOOL SetFrameRate(uint16_t nFrameRate) = 0;
	/// 获得帧率
	virtual uint16_t GetFrameRate(void) const = 0;

	/// 编码
	virtual int32_t Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize) = 0;
};

//=============================================================================
// {B95631D4-8101-4E4B-BDAC-39A4BDD2BFC7}
DEFINE_GUID(CLSID_IVideoDecoder, 
	0xb95631d4, 0x8101, 0x4e4b, 0xbd, 0xac, 0x39, 0xa4, 0xbd, 0xd2, 0xbf, 0xc7);

//=============================================================================
/// 视频解码器
class IVideoDecoder
{
public:
	/// 创建编码器
	virtual BOOL Create(ENUM_VIDEO_CODEC_TYPE enCodecType) = 0;
	/// 销毁编码器
	virtual void Destroy(void) = 0;

	/// 设置视频帧信息
	virtual BOOL SetFrameInfo(uint16_t nVideoWidth, 
		uint16_t nVideoHeight) = 0;
	/// 获得视频帧信息
	virtual BOOL GetFrameInfo(uint16_t& nVideoWidth, 
		uint16_t& nVideoHeight) const = 0;

	/// 解码
	virtual int32_t Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize) = 0;
};

#endif //__I_VIDEO_CODEC_H__
