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
/// \file    : IAudioCodec.h
/// \brief   : 音频编解码接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-22
///============================================================================
#ifndef __I_AUDIO_CODEC_H__
#define __I_AUDIO_CODEC_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"
#include "DeviceDefine.h"

//=============================================================================
/// 编码器类型
enum ENUM_AUDIO_CODEC_TYPE
{
	//ENUM_AUDIO_CODEC_MP3	= 0,			///< MP3 暂不支持
	ENUM_AUDIO_CODEC_AAC	= 1,			///< AAC
};

//=============================================================================
// {9D504D06-D493-48A9-92A8-1D802547369A}
DEFINE_GUID(CLSID_IAudioEncoder, 
	0x9d504d06, 0xd493, 0x48a9, 0x92, 0xa8, 0x1d, 0x80, 0x25, 0x47, 0x36, 0x9a);

//=============================================================================
/// 音频编码器
class IAudioEncoder
{
public:
	/// 创建编码器
	virtual BOOL Create(ENUM_AUDIO_CODEC_TYPE enCodecType) = 0;
	/// 销毁编码器
	virtual void Destroy(void) = 0;

	/// 获得音频参数
	virtual BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const = 0;
	/// 设置音频参数
	virtual BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT) = 0;

	/// 设置音频质量
	virtual BOOL SetAudioQuant(uint16_t nQuant = 85) = 0;
	/// 获得音频质量
	virtual uint16_t GetAudioQuant(void) const = 0;

	/// 设置带宽
	virtual BOOL SetBandWidth(uint32_t nBandWidth = 16000) = 0;
	/// 获得带宽
	virtual uint32_t GetBandWidth(void) const = 0;

	/// 编码
	virtual int32_t Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize) = 0;
};

//=============================================================================
// {98E064BE-B9A1-4579-BD66-220DAE2D1DAF}
DEFINE_GUID(CLSID_IAudioDecoder, 
	0x98e064be, 0xb9a1, 0x4579, 0xbd, 0x66, 0x22, 0xd, 0xae, 0x2d, 0x1d, 0xaf);


//=============================================================================
/// 音频解码器
class IAudioDecoder
{
public:
	/// 创建编码器
	virtual BOOL Create(ENUM_AUDIO_CODEC_TYPE enCodecType) = 0;
	/// 销毁编码器
	virtual void Destroy(void) = 0;

	/// 获得音频参数
	virtual BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const = 0;
	/// 设置音频参数
	virtual BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT) = 0;

	/// 解码
	virtual int32_t Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize) = 0;
};

#endif //__I_VIDEO_CODEC_H__
