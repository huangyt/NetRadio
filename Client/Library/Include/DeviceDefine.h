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
/// \file    : DeviceDefine.h
/// \brief   : 
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-16
///============================================================================
#ifndef __DEVICE_DEFINE_H__
#define __DEVICE_DEFINE_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"
#include "ICaptureEvent.h"
//=============================================================================
/// 设备类型
enum ENUM_DEVICE_TYPE
{
	ENUM_DEVICE_UNKNOWN = 0,		///< 未知的设备类型
	ENUM_DEVICE_AUDIO   = 1,		///< 音频采集设备
	ENUM_DEVICE_WDM	  = 2,			///< WDM视频输入设备
	ENUM_DEVICE_VFW	  = 3,			///< VFW视频输入设备
	ENUM_DEVICE_DV	  = 4,			///< DV设备
};

/// 设备名称长度
#define MAX_DEVICE_NAME_SIZE	1024

/// 设备信息
typedef struct _device_info
{
	WCHAR m_szDeviceName[MAX_DEVICE_NAME_SIZE];		///< 设备名称
	WCHAR m_szDisplayName[MAX_DEVICE_NAME_SIZE];	///< 显示名称
	uint32_t m_nDeviceProperty;						///< 设备属性
}device_info_t;

//=============================================================================
// 视频默认宽度
#define DEFAULT_VIDEO_WIDTH				320
// 视频默认高度
#define DEFAULT_VIDEO_HEIGHT			240
// 视频默认色彩
#define DEFAULT_COLOR_BIT				24
// 视频默认帧率
#define DEFAULT_FRAME_RATE				15

//=============================================================================
/// 音频采样频率
enum ENUM_FREQUENCY_TYPE
{
	ENUM_FREQUENCY_11KHZ = 11025,
	ENUM_FREQUENCY_22KHZ = 22050,
	ENUM_FREQUENCY_44KHZ = 44100,
};

/// 音频通道数
enum ENUM_CHANNEL_TYPE
{
	ENUM_CHANNEL_MONO	= 1,
	ENUM_CHANNEL_STEREO = 2,
};

/// 音频采样位数
enum ENUM_SAMPLE_TYPE
{
	ENUM_SAMPLE_8BIT	= 1,
	ENUM_SAMPLE_16BIT	= 2,
};


#endif //__DEVICE_DEFINE_H__
