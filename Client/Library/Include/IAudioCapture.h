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
/// \file    : IAudioCaputre.h
/// \brief   : 音频采集接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-16
///============================================================================
#ifndef __I_AUDIO_CAPTURE_H__
#define __I_AUDIO_CAPTURE_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"
#include "ICaptureEvent.h"
#include "DeviceDefine.h"

//=============================================================================
// {F77D4DFC-558D-47FA-8342-B2C1A46C934C}
DEFINE_GUID(CLSID_IAudioCaputre, 
0xf77d4dfc, 0x558d, 0x47fa, 0x83, 0x42, 0xb2, 0xc1, 0xa4, 0x6c, 0x93, 0x4c);

//=============================================================================
class IAudioCapture
{
public:
	/// 打开视频采集设备
	virtual BOOL Open(ICaptureEvent* pCaptureEvent, 
		const TCHAR* szDeviceName = NULL) = 0;
	/// 关闭视频采集设备
	virtual void Close(void) = 0;

	/// 设备采集设备是否打开
	virtual BOOL IsOpened(void) const = 0;

	/// 开始视频采集
	virtual BOOL StartCapture(void) = 0;
	/// 暂停视频采集
	virtual BOOL PauseCapture(void) = 0;
	/// 停止视频采集
	virtual BOOL StopCapture(void) = 0;

	/// 获得音频参数
	virtual BOOL GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
		ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const = 0;
	/// 设置音频参数
	virtual BOOL SetAudioFormat
		(ENUM_FREQUENCY_TYPE enFrequency = ENUM_FREQUENCY_22KHZ, 
		ENUM_CHANNEL_TYPE enChannel = ENUM_CHANNEL_STEREO,
		ENUM_SAMPLE_TYPE enSample = ENUM_SAMPLE_16BIT) = 0;

	/// 获得设备列表 
	virtual uint16_t GetAudioDeviceInfo(
		device_info_t* pArrDeviceInfo, uint16_t nArrCount) = 0;
};

#endif //__I_AUDIO_CAPTURE_H__
