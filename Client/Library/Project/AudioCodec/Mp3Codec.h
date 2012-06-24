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
/// \file    : Mp3Codec.h
/// \brief   : MP3编解码器
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-22
///============================================================================
#ifndef __MP3_CODEC_H__
#define __MP3_CODEC_H__

#include "TypeDefine.h"
#include "IAudioCodec.h"

#include "mp3\\Encoder\\include\\lame.h"
#include "mp3\\Decoder\\mad.h"

#ifdef _DEBUG
#pragma comment(lib, "libMp3EncoderD.lib")
#pragma comment(lib, "libMp3DecoderD.lib")
#pragma message("LINK libMp3EncoderD.lib and libMp3DecodeDr.lib")
#else
#pragma comment(lib, "libMp3Encoder.lib")
#pragma comment(lib, "libMp3Decoder.lib")
#pragma message("LINK libMp3Encoder.lib and libMp3Decoder.lib")
#endif

//=============================================================================
class CMp3Encoder
{
public:
	CMp3Encoder(void);
	~CMp3Encoder(void);

public:
	/// 创建编码器
	BOOL Create(void);
	/// 销毁编码器
	void Destroy(void);

	/// 编码
	int32_t Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	lame_t m_hHandleEncoder;			///<　MP3编码器句柄
};

//=============================================================================
class CMp3Decoder
{
public:
	CMp3Decoder(void);
	~CMp3Decoder(void);

public:
	/// 创建编码器
	BOOL Create(void);
	/// 销毁编码器
	void Destroy(void);

	/// 编码
	int32_t Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize);

private:
	// mad 输入回调函数
	static enum mad_flow AudioInput(void *data, struct mad_stream *stream);
	// mad header回调函数
	static enum mad_flow AudioHeader(void *data, struct mad_header const *header);
	// mad filter回调函数
	static enum mad_flow AudioFilter(void *data, struct mad_stream const *stream, 
		struct mad_frame *frame);
	// mad 输出回调函数
	static enum mad_flow AudioOutput(void *data, struct mad_header const *header, 
		struct mad_pcm *pcm);
	// mad 错误回调函数
	static enum mad_flow Error(void *data, struct mad_stream *stream, 
		struct mad_frame *frame);
	// mad 消息回调函数
	static enum mad_flow Message(void *, void *, unsigned int *);

private:
	void* m_hHandleDecoder;				///< MP3解码器句柄
	struct mad_decoder m_MadDecoder;	///< MP3解码器
};

#endif// __MP3_CODEC_H__
