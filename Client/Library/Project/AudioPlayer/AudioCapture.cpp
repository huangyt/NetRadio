#include "StdAfx.h"
#include "AudioCapture.h"
#include "DeviceDefine.h"

//=============================================================================
// {83B13E88-1DA0-4F37-BC4B-5B4A60EC7354}
DEFINE_GUID(CLSID_AudioCapture, 
	0x83b13e88, 0x1da0, 0x4f37, 0xbc, 0x4b, 0x5b, 0x4a, 0x60, 0xec, 0x73, 0x54);

//=============================================================================
CAudioOutputPin::CAudioOutputPin(CAudioCapture* pFilter, 
	HRESULT* phr, LPCWSTR pName)
	: CBaseOutputPin(L"Audio_Stream", pFilter, &pFilter->m_FilterLock, phr, pName)
{
	m_MediaType.InitMediaType();
	m_MediaType.SetType(&MEDIATYPE_Audio);
	m_MediaType.SetSubtype(&MEDIASUBTYPE_PCM);
	m_MediaType.SetFormatType(&FORMAT_WaveFormatEx);

	WAVEFORMATEX *pWF=(WAVEFORMATEX*)m_MediaType.AllocFormatBuffer(sizeof(WAVEFORMATEX));
	ZeroMemory(pWF, sizeof(WAVEFORMATEX));

	pWF->cbSize = sizeof(WAVEFORMATEX);
	pWF->nChannels = ENUM_CHANNEL_STEREO;
	pWF->nSamplesPerSec = ENUM_FREQUENCY_22KHZ;
	pWF->nAvgBytesPerSec = ENUM_SAMPLE_16BIT * ENUM_FREQUENCY_22KHZ * ENUM_CHANNEL_STEREO;
	pWF->wBitsPerSample = ENUM_SAMPLE_16BIT * 8;
	pWF->nBlockAlign = ENUM_SAMPLE_16BIT * ENUM_CHANNEL_STEREO;
	pWF->wFormatTag = 1;  // 日你个祖宗，折腾老子整整半天
	m_MediaType.SetTemporalCompression(FALSE);
}

CAudioOutputPin::~CAudioOutputPin(void)
{
}

HRESULT CAudioOutputPin::CheckMediaType(const CMediaType* pMediaType)
{
	if (*pMediaType == m_MediaType)
	{
		return NOERROR;
	}
	return E_FAIL;
}

HRESULT CAudioOutputPin::DecideBufferSize(IMemAllocator* pAlloc, 
	ALLOCATOR_PROPERTIES * pprop)
{
	ASSERT(pAlloc);
	ASSERT(pprop);
	HRESULT hr = NOERROR;

	WAVEFORMATEX * info = (WAVEFORMATEX *) m_MediaType.pbFormat;
	pprop->cbBuffer = info->nAvgBytesPerSec;

	pprop->cBuffers  = 1;
	pprop->cbAlign   = info->nBlockAlign;

	ASSERT(pprop->cbBuffer);

	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pprop, &Actual);
	if (FAILED(hr)) 
	{
		return hr;
	}

	ASSERT( Actual.cBuffers == 1 );

	if (pprop->cBuffers > Actual.cBuffers ||
		pprop->cbBuffer > Actual.cbBuffer) 
	{
		return E_FAIL;
	}
	return NOERROR;
}

HRESULT CAudioOutputPin::GetMediaType(int iPosition, CMediaType* pMediaType)
{
	if (iPosition == 0)
	{
		*pMediaType = m_MediaType;
		return NOERROR;
	}
	return E_INVALIDARG;
}

/// 设置音频参数
BOOL CAudioOutputPin::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency, 
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
{
	CMediaType MediaType;

	HRESULT hr = GetMediaType(0, &MediaType);
	if(SUCCEEDED(hr))
	{
		WAVEFORMATEX* pWF = (WAVEFORMATEX *) m_MediaType.pbFormat;
		pWF->nChannels = enChannel;
		pWF->nSamplesPerSec = enFrequency;
		pWF->nAvgBytesPerSec = enSample * enFrequency * enChannel;
		pWF->wBitsPerSample = enSample * 8;
		pWF->nBlockAlign = (WORD) (enSample * enChannel);

		if(IsConnected())
		{
			// 动态重建
			IPin* pPin = GetConnected();
			if(NULL != pPin)
			{
				hr = pPin->QueryAccept(&MediaType);
				if(FAILED(hr))
				{
					return FALSE;
				}

				hr = ReceiveConnection(pPin, &MediaType);
				if(FAILED(hr))
				{
					return FALSE;
				}
			}
		}
		m_MediaType = MediaType;
		return TRUE;
	}
	return FALSE;
}

//=============================================================================
CAudioCapture::CAudioCapture(LPUNKNOWN punk, HRESULT *phr)
	: CBaseFilter(L"Audio Capture", punk, &m_FilterLock, CLSID_AudioCapture)
{
	ASSERT(phr);
	CAutoLock lck(&m_FilterLock);
	m_OutPin  = new CAudioOutputPin(this, phr, L"Out\r\n");
	if(m_OutPin == NULL)
	{
		if(phr)
			*phr = E_OUTOFMEMORY;
	}
	m_Sample  = NULL;
}


CAudioCapture::~CAudioCapture(void)
{
	CAutoLock lck(&m_FilterLock);
	SAFE_RELEASE(m_Sample);
	SAFE_DELETE(m_OutPin);
}

int CAudioCapture::GetPinCount()
{
	return 1;
}

CBasePin* CAudioCapture::GetPin(int n)
{
	if (n == 0) 
	{
		if(m_OutPin)
			return m_OutPin;
	}
	return NULL;
}

STDMETHODIMP CAudioCapture::Stop()
{
	CAutoLock lck(&m_FilterLock);
	SAFE_RELEASE(m_Sample);
	return CBaseFilter::Stop();
}

BOOL CAudioCapture::GetSampleBuffer(PBYTE* pBuffer, uint32_t* pBufferSize)
{
	CAutoLock lck(&m_FilterLock);
	SAFE_RELEASE(m_Sample);

	HRESULT hr = m_OutPin->GetDeliveryBuffer(&m_Sample,NULL,NULL,0);
	if (m_Sample)
	{
		m_Sample->GetPointer(pBuffer);
		*pBufferSize = m_Sample->GetSize();
	}
	return SUCCEEDED(hr);
}

BOOL CAudioCapture::DeliverHoldingSample(long inSampleSize)
{
	CAutoLock lck(&m_FilterLock);
	if (m_Sample)
	{
		m_Sample->SetActualDataLength(inSampleSize);
		m_Sample->SetSyncPoint(FALSE);
		m_Sample->SetTime(NULL, NULL);

		HRESULT hr = m_OutPin->Deliver(m_Sample);
		m_Sample->Release();
		m_Sample = NULL;

		if(FAILED(hr))
		{
			WCHAR szError[256] = {0};
			AMGetErrorText(hr, szError, 256);
		}

		return SUCCEEDED(hr);
	}
	return TRUE;
}

void CAudioCapture::OnCaptureData(const char* buffer, int len)
{
	PBYTE pSampleBuffer;
	uint32_t nBufferSize;
	BOOL pass = FALSE;
	do
	{
		pass = GetSampleBuffer(&pSampleBuffer, &nBufferSize);
		if (!pass)
		{
			Sleep(100);
		}
		if(m_Sample == NULL)
			return;

	} while (!pass); 

	if(nBufferSize < len)
	{
		ASSERT(FALSE);
	}

	CopyMemory(pSampleBuffer, buffer, len);
	DeliverHoldingSample(len);
}

/// 设置音频参数
BOOL CAudioCapture::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency, 
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
{
	if(NULL != m_OutPin)
	{
		return m_OutPin->SetAudioFormat(enFrequency, enChannel, enSample);
	}
	return FALSE;
}
