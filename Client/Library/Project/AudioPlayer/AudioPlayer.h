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
/// \file    : AudioPlayer.h
/// \brief   : 音频播放类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-19
///============================================================================
#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

#include "dshow\\streams.h"
#include "IAudioPlayer.h"
#include "GraphBuilder.h"
#include "AudioCapture.h"

//=============================================================================
class CAudioPlayer : public IAudioPlayer, CGraphBuilder
{
public:
	CAudioPlayer(void);
	~CAudioPlayer(void);

public:
	/// 打开
	virtual BOOL Open(void);
	/// 关闭
	virtual void Close(void);

	/// 是否打开
	virtual BOOL IsOpened(void) const;

	/// 开始播放
	virtual BOOL StartPlay(void);
	/// 暂停播放
	virtual BOOL PausePlay(void);
	/// 停止播放
	virtual BOOL StopPlay(void);

	/// 获得音频参数
	virtual BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const;
	/// 设置音频参数
	virtual BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT);

	/// 接收音频数据
	virtual void OnAudioData(const char* pAudioData, uint32_t nDataSize, 
		uint64_t nTimeStamp);

protected:
	virtual void OnNotify(int nNotifyCode);

private:
	CAudioCapture* m_pAudioCapture;			///< 音频采集Filter
	IBaseFilter*  m_pAudioRender;           ///< 音频播放Filter
	IBasicAudio*  m_pBasicAudio;            ///< IBasicAudio

	ENUM_FREQUENCY_TYPE m_enFrequency;		///< 采样频率
	ENUM_CHANNEL_TYPE m_enChannel;			///< 声道数
	ENUM_SAMPLE_TYPE m_enSample;			///< 采样位数
};

#endif //__I_VIDEO_CAPTURE_H__
