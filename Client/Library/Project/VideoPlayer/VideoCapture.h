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
/// \file    : VideoCapture.h
/// \brief   : 视频采集类
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-16
///============================================================================
#ifndef __VIDEO_CAPTURE_H__
#define __VIDEO_CAPTURE_H__

#include "dshow\\streams.h"
#include "CriticalSection.h"
#include "GraphBuilder.h"
#include "TypeDefine.h"

class CVideoCapture;
//=============================================================================
class CVideoOutputPin : public CBaseOutputPin
{
public:
	CVideoOutputPin(CVideoCapture* pFilter, HRESULT* phr, LPCWSTR pName);
	~CVideoOutputPin(void);

public:
	// check if the pin can support this specific proposed type and format
	virtual HRESULT CheckMediaType(const CMediaType *);
	virtual HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES * pprop);
	// returns the preferred formats for a pin
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    STDMETHODIMP_(ULONG) AddRef()       { return 2UL; }
    STDMETHODIMP_(ULONG) Release()      { return 2UL; }

public:
	BOOL SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
		uint16_t nFrameRate);

private:
	CVideoCapture* m_Filter;
	CMediaType	   m_MediaType;
};

//=============================================================================
class CVideoCapture : public CBaseFilter
{
	friend class CVideoOutputPin;
public:
	CVideoCapture(LPUNKNOWN punk, HRESULT *phr);
	~CVideoCapture(void);

public:
	virtual int GetPinCount();
	virtual CBasePin * GetPin(int n);
	STDMETHODIMP Stop();

	BOOL GetSampleBuffer(PBYTE* pBuffer, uint32_t* pBufferSize);
	BOOL DeliverHoldingSample(long inSampleSize);

public:
	void OnCaptureData(const char* buffer, int len);

	BOOL SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
		uint16_t nFrameRate);

private:
	CCritSec m_FilterLock;			///< Filter临界
	CVideoOutputPin*	m_OutPin;
	IMediaSample*		m_Sample;
};

#endif //__VIDEO_CAPTURE_H__
