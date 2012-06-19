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
/// \file    : IAudioPlayer.h
/// \brief   : 音频播放接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-16
///============================================================================
#ifndef __I_AUDIO_PLAYER_H__
#define __I_AUDIO_PLAYER_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"
#include "DeviceDefine.h"

//=============================================================================
// {46DCB938-BB49-4A6F-9618-33E420126E9A}
DEFINE_GUID(CLSID_IAudioPlayer, 
	0x46dcb938, 0xbb49, 0x4a6f, 0x96, 0x18, 0x33, 0xe4, 0x20, 0x12, 0x6e, 0x9a);

//=============================================================================
class IAudioPlayer
{
public:
	/// 打开
	virtual BOOL Open(void) = 0;
	/// 关闭
	virtual void Close(void) = 0;

	/// 是否打开
	virtual BOOL IsOpened(void) const = 0;

	/// 开始播放
	virtual BOOL StartPlay(void) = 0;
	/// 暂停播放
	virtual BOOL PausePlay(void) = 0;
	/// 停止播放
	virtual BOOL StopPlay(void) = 0;

	/// 获得音频参数
	virtual BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const = 0;
	/// 设置音频参数
	virtual BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT) = 0;

	/// 接收音频数据
	virtual void OnAudioData(const char* pAudioData, uint32_t nDataSize, 
		uint64_t nTimeStamp) =0;
};

#endif //__I_AUDIO_PLAYER_H__
