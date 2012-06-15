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
/// \file    : CaptureDevice.h
/// \brief   : 采集设备类
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-14
///============================================================================
#ifndef __CAPTURE_DEVICE_H__
#define __CAPTURE_DEVICE_H__

#include "ICaptureDevice.h"

//=============================================================================
// class CCaptureDevice
class CCaptureDevice : public ICaptureDevice
{
public:
	CCaptureDevice(void);
	~CCaptureDevice(void);

public:
	/// 获得设备数量
	virtual uint16_t GetDeviceCount(ENUM_DEVICE_TYPE enDeviceType) const;

	/// 获得设备列表 
	virtual uint16_t GetDeviceList(ENUM_DEVICE_TYPE enDeviceType, 
		capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const;

	/// 根据给定视频设备名称得到得到该设备的Filter
	virtual IBaseFilter* GetDeviceFilter(ENUM_DEVICE_TYPE enDeviceType, 
		TCHAR* pszDeviceName, uint16_t nNameSize) const;

	/// 得到选中Filter的设备类型
	virtual ENUM_VIDEO_DEVICE_TYPE GetDeviceType(IBaseFilter* apFilter) const;	

private:
	/// 获得音频输入设备列表
	uint16_t GetAudioInputList(capture_device_info_t* pArrDeviceInfo, 
		uint16_t nArrCount) const;
	/// 获得音频输出设备列表
	uint16_t GetAudioOutputList(capture_device_info_t* pArrDeviceInfo, 
		uint16_t nArrCount) const;
	/// 获得视频输入设备列表
	uint16_t GetVideoInputList(capture_device_info_t* pArrDeviceInfo, 
		uint16_t nArrCount) const;

	/// 获得音频输入设备Filter
	IBaseFilter* GetAudioInputFilter(TCHAR* pszDeviceName, uint16_t nNameSize) const;
	/// 获得音频输出设备Filter
	IBaseFilter* GetAudioOutputFilter(TCHAR* pszDeviceName, uint16_t nNameSize) const;
	/// 获得视频输入设备Filter
	IBaseFilter* GetVideoInputFilter(TCHAR* pszDeviceName, uint16_t nNameSize) const;
};

#endif //__CAPTURE_DEVICE_H__
