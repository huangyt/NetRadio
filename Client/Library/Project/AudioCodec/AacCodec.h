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
/// \file    : AacCodec.h
/// \brief   : AAC编解码器
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-22
///============================================================================
#ifndef __AAC_CODEC_H__
#define __AAC_CODEC_H__

#include "TypeDefine.h"
#include "IAudioCodec.h"
#include "aac\\Encoder\\include\\faac.h"
#include "aac\\Decoder\\include\\faad.h"

#ifdef _DEBUG
#pragma comment(lib, "libAacEncoderD.lib")
#pragma comment(lib, "libAacDecoderD.lib")
#pragma message("LINK libAacEncoderD.lib and libAacDecoderD.lib")
#else
#pragma comment(lib, "libAacEncoder.lib")
#pragma comment(lib, "libAacDecoder.lib")
#pragma message("LINK libAacEncoder.lib and libAacDecoder.lib")
#endif

//=============================================================================
class CAacEncoder
{
public:
	CAacEncoder(void);
	~CAacEncoder(void);

public:
	/// 创建编码器
	BOOL Create(void);
	/// 销毁编码器
	void Destroy(void);

	/// 获得音频参数
	BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const;
	/// 设置音频参数
	BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT);

	/// 设置音频质量
	BOOL SetAudioQuant(uint16_t nQuant = 85);
	/// 获得音频质量
	uint16_t GetAudioQuant(void) const;
	
	/// 设置带宽
	BOOL SetBandWidth(uint32_t nBandWidth = 16000);
	/// 获得带宽
	uint32_t GetBandWidth(void) const;

	/// 编码
	int32_t Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	int32_t ConvertToInt16(int8_t nSampleByte, const char* pInputBuffer, 
		uint32_t nInpubSize, int16_t* pOutputBuffer, uint32_t nOutputSize);

private:
	faacEncHandle m_hHandleEncoder;		///<　aac编码器句柄

	ENUM_FREQUENCY_TYPE m_enFrequency;	///< 采样频率
	ENUM_CHANNEL_TYPE   m_enChannel;	///< 声道数量
	ENUM_SAMPLE_TYPE	m_enSample;		///< 采样位数

	uint16_t m_nAudioQuant;				///< 质量
	uint32_t m_nBandWidth;				///< 带宽

	uint32_t m_nInputSamples;
	uint32_t m_nMaxOutputBytes;
};

//=============================================================================
class CAacDecoder
{
public:
	CAacDecoder(void);
	~CAacDecoder(void);

public:
	/// 创建编码器
	BOOL Create(void);
	/// 销毁编码器
	void Destroy(void);

	/// 获得音频参数
	BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const;
	/// 设置音频参数
	BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT);

	/// 编码
	int32_t Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	faacDecHandle m_hHandleDecoder;		///< aac解码器
	faacDecFrameInfo m_aacFrameInfo;

	ENUM_FREQUENCY_TYPE m_enFrequency;	///< 采样频率
	ENUM_CHANNEL_TYPE   m_enChannel;	///< 声道数量
	ENUM_SAMPLE_TYPE	m_enSample;		///< 采样位数

	BOOL m_bIsInitDec;
};

#endif// __AAC_CODEC_H__
