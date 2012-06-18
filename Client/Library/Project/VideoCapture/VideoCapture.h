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
/// \file    : VideoCaputre.h
/// \brief   : 视频采集类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-17
///============================================================================
#ifndef __VIDEO_CAPTURE_H__
#define __VIDEO_CAPTURE_H__

#include "dshow\\streams.h"
#include "IVideoCapture.h"
#include "GraphBuilder.h"
#include "VideoRenderer.h"

//=============================================================================
class CVideoCapture : public IVideoCapture, CGraphBuilder
{
public:
	CVideoCapture(void);
	~CVideoCapture(void);

public:
	/// 打开视频采集设备
	virtual BOOL Open(ICaptureEvent* pCaptureEvent, 
		const TCHAR* szDeviceName = NULL);
	/// 关闭视频采集设备
	virtual void Close(void);

	/// 设备采集设备是否打开
	virtual BOOL IsOpened(void) const;

	/// 开始视频采集
	virtual BOOL StartCapture(void);
	/// 暂停视频采集
	virtual BOOL PauseCapture(void);
	/// 停止视频采集
	virtual BOOL StopCapture(void);

	/// 设置视频信息
	virtual BOOL SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
		uint16_t pColorBit);
	/// 获得视频信息
	virtual BOOL GetVideoFormat(uint16_t* pVideoWidth, uint16_t* pVideoHeight, 
		uint16_t* pColorBit) const;

	/// 设置帧率
	virtual BOOL SetFrameRate(uint16_t nFrameRate);
	/// 获得指针
	virtual uint16_t GetFrameRate(void) const;

	/// 获得设备列表 
	virtual uint16_t GetVideoDeviceInfo(
		device_info_t* pArrDeviceInfo, uint16_t nArrCount);

protected:
	/// 底层事件通知
	virtual void OnNotify(int nNotifyCode);

private:
	/// 创建采集Filter
	IBaseFilter* CreateCaptureFiler(const TCHAR* szDeviceName);

private:
	ICaptureGraphBuilder2* m_pCGBuilder;	///< ICaptureGraphBuilder2
	IBaseFilter* m_pCaptureFilter;			///< 视频捕获设备Filter
	CVideoRenderer* m_pVideoReander;		///< Video Render Filter

	uint16_t m_nVideoWidth;					///< 视频宽度
	uint16_t m_nVideoHeight;				///< 视频高度
	uint16_t m_nColorBit;					///< 色彩
	uint16_t m_nFrameRate;					///< 帧率
};

#endif //__VIDEO_CAPTURE_H__
