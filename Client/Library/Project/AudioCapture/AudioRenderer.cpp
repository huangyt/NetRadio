#include "StdAfx.h"
#include <InitGuid.h>
#include <Guiddef.h>
#include "AudioRenderer.h"

//=============================================================================
// {4CCE3E4B-2F69-4418-B20D-91838F0FB0D0}
DEFINE_GUID(CLSID_CaptureAudioRender, 
	0x4cce3e4b, 0x2f69, 0x4418, 0xb2, 0xd, 0x91, 0x83, 0x8f, 0xf, 0xb0, 0xd0);

//=============================================================================
CAudioRenderer::CAudioRenderer(LPUNKNOWN lpunk, HRESULT *phr)
	: CBaseRenderer(CLSID_CaptureAudioRender, L"CaptureAudioRender", lpunk, phr)
	, m_pCaptureEvent(NULL)
{
}


CAudioRenderer::~CAudioRenderer(void)
{
}

HRESULT CAudioRenderer::DoRenderSample(IMediaSample *pMediaSample)
{
	if(pMediaSample == NULL)
		return E_FAIL;

	if(m_pCaptureEvent != NULL)
	{
		PBYTE pData = NULL;
		pMediaSample->GetPointer(&pData);
		long length = pMediaSample->GetActualDataLength();
		m_pCaptureEvent->OnCaptureEvent(ENUM_EVENT_VIDEO, 
			(const char*)pData, length, 0);
	}
	return S_OK;
}

HRESULT CAudioRenderer::CheckMediaType(const CMediaType * pMediaType)
{
	if(pMediaType == NULL)
		return E_FAIL;

	if (pMediaType->formattype == FORMAT_WaveFormatEx)
	{
		//const GUID* SubType = pMediaType->
		//if(*SubType != MEDIASUBTYPE_RGB24)
		//{
		//	return E_INVALIDARG;
		//}

		WAVEFORMATEX * info = (WAVEFORMATEX *) pMediaType->pbFormat;

		m_MediaType = *pMediaType;
		return S_OK;
	}
	return E_FAIL;
}

CMediaType* CAudioRenderer::GetMediaType()
{
	return &m_MediaType;
}

/// 设置回调接口
void CAudioRenderer::SetCaptureEvent(ICaptureEvent* pCaptureEvent)
{
	m_pCaptureEvent = pCaptureEvent;
}