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
/// \file    : AudioEncoder.h
/// \brief   : 视频编编码类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-22
///============================================================================
#ifndef __AUDIO_ENCODER_H__
#define __AUDIO_ENCODER_H__

#include "IAudioCodec.h"
#include "Mp3Codec.h"
#include "AacCodec.h"

//=============================================================================
class CAudioEncoder : public IAudioEncoder
{
public:
	CAudioEncoder(void);
	~CAudioEncoder(void);

public:
	/// 创建编码器
	virtual BOOL Create(ENUM_AUDIO_CODEC_TYPE enCodecType);
	/// 销毁编码器
	virtual void Destroy(void);

	/// 获得音频参数
	virtual BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const;
	/// 设置音频参数
	virtual BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT);

	/// 设置音频质量
	virtual BOOL SetAudioQuant(uint16_t nQuant = 85);
	/// 获得音频质量
	virtual uint16_t GetAudioQuant(void) const;

	/// 设置带宽
	virtual BOOL SetBandWidth(uint32_t nBandWidth = 16000);
	/// 获得带宽
	virtual uint32_t GetBandWidth(void) const;

	/// 编码
	virtual int32_t Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	ENUM_AUDIO_CODEC_TYPE m_enCodecType;	///< 编码器类型
	CMp3Encoder m_Mp3Encoder;				///< MP3编码器
	CAacEncoder m_AacEncoder;				///< AAC编码器
};

#endif //__AUDIO_ENCODER_H__
