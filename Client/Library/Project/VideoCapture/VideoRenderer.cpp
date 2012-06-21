#include "StdAfx.h"
#include <InitGuid.h>
#include <Guiddef.h>
#include "VideoRenderer.h"

//=============================================================================
// {05640D5D-3AC5-45D0-96B1-B1455E475538}
DEFINE_GUID(CLSID_CaptureVideoRender, 
	0x5640d5d, 0x3ac5, 0x45d0, 0x96, 0xb1, 0xb1, 0x45, 0x5e, 0x47, 0x55, 0x38);


//=============================================================================
CVideoRenderer::CVideoRenderer(LPUNKNOWN lpunk, HRESULT *phr)
	: CBaseRenderer(CLSID_CaptureVideoRender, L"CaptureVideoRender", lpunk, phr)
	, m_pCaptureEvent(NULL)
{
}


CVideoRenderer::~CVideoRenderer(void)
{
}

HRESULT CVideoRenderer::DoRenderSample(IMediaSample *pMediaSample)
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

HRESULT CVideoRenderer::CheckMediaType(const CMediaType * pMediaType)
{
	if(pMediaType == NULL)
		return E_FAIL;

	if (pMediaType->formattype == FORMAT_VideoInfo)
	{
		const GUID* SubType = pMediaType->Subtype();
		if(*SubType != MEDIASUBTYPE_RGB24)
		{
			return E_INVALIDARG;
		}

		VIDEOINFOHEADER* lpVideoInfoHeader = (VIDEOINFOHEADER*)(pMediaType->pbFormat);

		m_MediaType = *pMediaType;
		return S_OK;
	}
	return E_FAIL;
}

CMediaType* CVideoRenderer::GetMediaType()
{
	return &m_MediaType;
}

/// 设置回调接口
void CVideoRenderer::SetCaptureEvent(ICaptureEvent* pCaptureEvent)
{
	m_pCaptureEvent = pCaptureEvent;
}