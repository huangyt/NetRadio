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
/// \file    : VolumeControl.h
/// \brief   : 音量控制接口
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-01
///============================================================================
#ifndef __VOLUME_CONTROL_H__
#define __VOLUME_CONTROL_H__

#include <mmsystem.h>
#include "IVolumeControl.h"
#include "Mixer.h"

//=============================================================================
#pragma comment(lib,"winmm.lib")

//=============================================================================
class CVolumeControl : public IVolumeControl
{
public:
	CVolumeControl(void);
	~CVolumeControl(void);

public:
	/// 打开音量控制
	virtual BOOL Open(ENUM_VOLUME_DEVICE enVolumeDevice);
	/// 关闭音量控制
	virtual void Close(void);

	/// 判断是否静音
	virtual BOOL IsMute(void) const;
	/// 设置静音
	virtual BOOL SetMutex(BOOL bIsMutex);

	/// 设置音量
	virtual BOOL SetVolume(uint32_t nLValue, uint32_t nRValue);
	/// 获得音量
	virtual BOOL GetVolume(uint32_t* pLValue, uint32_t* pRValue) const;

private:
	ENUM_VOLUME_DEVICE m_enVolumeDevice;
	CMixer m_oMixer;
};

#endif //__VOLUME_CONTROL_H__
