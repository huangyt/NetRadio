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
/// \file    : ICaptureEvent.h
/// \brief   : 采集事件回调接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-17
///============================================================================
#ifndef __I_CAPTURE_EVENT_H__
#define __I_CAPTURE_EVENT_H__

#include "TypeDefine.h"

//=============================================================================
/// 事件类型
enum ENUM_EVENT_TYPE
{
	ENUM_EVENT_AUDIO = 0,					///< 音频事件
	ENUM_EVENT_VIDEO = 1,					///< 视频事件
};

//=============================================================================
// class ICaptureEvent
class ICaptureEvent
{
public:
	/// 事件响应函数
	virtual void OnCaptureEvent(ENUM_EVENT_TYPE enType, 
		const char* szEventData, uint16_t nDataSize, uint64_t nTimeStamp) = 0;
};

#endif //__I_CAPTURE_EVENT_H__
