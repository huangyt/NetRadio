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
/// \file    : IAudioResample.h
/// \brief   : 音频重采样接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-04
///============================================================================
#ifndef __I_AUDIO_RESAMPLE_H__
#define __I_AUDIO_RESAMPLE_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"

//=============================================================================
// 重采样级别
enum ENUM_RESAMPLE_LEVEL
{
	RESAMPLE_BEST_QUALITY		= 0,		///< 最高品质
	RESAMPLE_MEDIUM_QUALITY		= 1,		///< 中等品质
	RESAMPLE_FASTEST			= 2,		///< 最快速度
};

// 重采样频率
enum ENUM_RESAMPLE_RATE
{
	RESAMPLE_RATE_8000			= 8000,
	RESAMPLE_RATE_11025			= 11025,
	RESAMPLE_RATE_12000			= 12000,
	RESAMPLE_RATE_16000			= 16000,
	RESAMPLE_RATE_22050			= 22050,
	RESAMPLE_RATE_24000			= 24000,
	RESAMPLE_RATE_32000			= 32000,
	RESAMPLE_RATE_44100			= 44100,
	RESAMPLE_RATE_48000			= 48000,
	RESAMPLE_RATE_96000			= 96000,
};

//=============================================================================
// {F4366D51-D1C7-453A-A924-1FA44419BD99}
DEFINE_GUID(CLSID_IAudioResample, 
	0xf4366d51, 0xd1c7, 0x453a, 0xa9, 0x24, 0x1f, 0xa4, 0x44, 0x19, 0xbd, 0x99);

//=============================================================================
// class IAudioResample
class IAudioResample
{
public:
	/// 创建
	virtual BOOL Create(ENUM_RESAMPLE_LEVEL enResampleLevel, int nAudioChannel, 
		ENUM_RESAMPLE_RATE enAudioRate, ENUM_RESAMPLE_RATE enResampleRate) = 0;
	/// 销毁
	virtual void Destroy(void) = 0;

	/// 重采样
	virtual uint32_t Resample(int16_t* pInput, uint32_t nInputSize, 
		int16_t* pOutput, uint32_t nOutputSize) = 0;
};

#endif //__I_AUDIO_RESAMPLE_H__
