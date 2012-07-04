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
/// \file    : AudioResample.h
/// \brief   : 音频重采样类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-04
///============================================================================
#ifndef __AUDIO_RESAMPLE_H__
#define __AUDIO_RESAMPLE_H__

#include "IAudioResample.h"
#include "samplerate.h"

#ifdef _DEBUG
#pragma comment(lib,"libSamplerateD.lib")
#pragma message("LINK libSamplerateD.lib")
#else
#pragma comment(lib,"libSamplerate.lib")
#pragma message("LINK libSamplerate.lib")
#endif //_DEBUG

//=============================================================================
class CAudioResample : public IAudioResample
{
public:
	CAudioResample(void);
	~CAudioResample(void);

public:
	/// 创建
	virtual BOOL Create(ENUM_RESAMPLE_LEVEL enResampleLevel, int nAudioChannel, 
		ENUM_RESAMPLE_RATE enAudioRate, ENUM_RESAMPLE_RATE enResampleRate);
	/// 销毁
	virtual void Destroy(void);

	/// 重采样
	virtual uint32_t Resample(int16_t* pInput, uint32_t nInputSize, 
		int16_t* pOutput, uint32_t nOutputSize);

private:
	SRC_STATE* m_pSrcState;
	double m_fSrcRadio;
	int m_nAudioChannel;
};

#endif //__AUDIO_RESAMPLE_H__
