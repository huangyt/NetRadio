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
/// \file    : IMediaCenter.h
/// \brief   : 媒体中心
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-15
///============================================================================
#ifndef __I_MEDIA_CENTER_H__
#define __I_MEDIA_CENTER_H__

#include "TypeDefine.h"
#include "InterfaceDefine.h"

#include "IAudioCapture.h"
#include "IAudioCodec.h"
//#include "IAudioEffect.h"
//#include "IAudioMix.h"
#include "IAudioPlayer.h"
#include "IAudioResample.h"
#include "IVolumeControl.h"

#include "IColorSpace.h"
//#include "IMediaCapture.h"
#include "IVideoCapture.h"
#include "IVideoCodec.h"
//#include "IVideoEffect.h"
#include "IVideoPlayer.h"
#include "IVideoResize.h"

//=============================================================================
// {38E0679D-C123-4EDB-96BA-CE4D8DD026ED}
DEFINE_GUID(CLSID_IMediaCenter, 
	0x38e0679d, 0xc123, 0x4edb, 0x96, 0xba, 0xce, 0x4d, 0x8d, 0xd0, 0x26, 0xed);

//=============================================================================
// class IMediaCenter
class IMediaCenter
{
public:
	/// 查询接口
	virtual IRESULT QueryInterface(const CLSID& oInterfaceID, 
		void** ppInterface) = 0;

	/// 释放接口
	virtual IRESULT ReleaseInterface(const CLSID& oInterfaceID, 
		void* pInterface) = 0;
};

#endif //__I_MEDIA_CENTER_H__
