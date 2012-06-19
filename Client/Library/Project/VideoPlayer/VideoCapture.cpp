#include "VideoCapture.h"
#include "DeviceDefine.h"

//=============================================================================
// {F21A1FA2-B5A4-4022-95DA-D4A132804610}
DEFINE_GUID(CLSID_VideoCapture, 
0xf21a1fa2, 0xb5a4, 0x4022, 0x95, 0xda, 0xd4, 0xa1, 0x32, 0x80, 0x46, 0x10);


//=============================================================================
CVideoOutputPin::CVideoOutputPin(CVideoCapture* pFilter, 
	HRESULT* phr, LPCWSTR pName)
	: CBaseOutputPin(L"Video_Stream", pFilter, &pFilter->m_FilterLock, phr, pName)
{
	m_MediaType.InitMediaType();
    m_MediaType.SetType(&MEDIATYPE_Video) ;
	m_MediaType.SetSubtype(&MEDIASUBTYPE_RGB24) ;
	m_MediaType.SetFormatType(&FORMAT_VideoInfo);

	VIDEOINFO *pvi = (VIDEOINFO *) m_MediaType.AllocFormatBuffer(sizeof(VIDEOINFO));
	ZeroMemory(pvi, sizeof(VIDEOINFO));

	pvi->AvgTimePerFrame = UNITS / DEFAULT_FRAME_RATE;
	//pvi->bmiHeader.biCompression = FOURCC_XVID;
	pvi->bmiHeader.biBitCount    = DEFAULT_COLOR_BIT;
	pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
	pvi->bmiHeader.biWidth      = DEFAULT_VIDEO_WIDTH;
	pvi->bmiHeader.biHeight     = DEFAULT_VIDEO_HEIGHT;
	pvi->bmiHeader.biSizeImage  = 
		DEFAULT_VIDEO_WIDTH*DEFAULT_VIDEO_HEIGHT*DEFAULT_COLOR_BIT/8;

	SetRectEmpty(&(pvi->rcSource));
	SetRectEmpty(&(pvi->rcTarget));
	m_MediaType.SetSampleSize(pvi->bmiHeader.biSizeImage); 
	m_MediaType.SetTemporalCompression(FALSE);
	m_MediaType.cbFormat = sizeof(VIDEOINFOHEADER);
	m_MediaType.bFixedSizeSamples = FALSE;
}

CVideoOutputPin::~CVideoOutputPin(void)
{
}

HRESULT CVideoOutputPin::CheckMediaType(const CMediaType* pMediaType)
{
	if (*pMediaType == m_MediaType)
	{
		return NOERROR;
	}
	return E_FAIL;
}

HRESULT CVideoOutputPin::DecideBufferSize(IMemAllocator* pAlloc, 
	ALLOCATOR_PROPERTIES * pprop)
{
	ASSERT(pAlloc);
	ASSERT(pprop);
	HRESULT hr = NOERROR;

	VIDEOINFOHEADER * info = (VIDEOINFOHEADER *) m_MediaType.pbFormat;
	pprop->cbBuffer = info->bmiHeader.biSizeImage;
	
	pprop->cBuffers  = 1;
	pprop->cbAlign   = 1;

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

HRESULT CVideoOutputPin::GetMediaType(int iPosition, CMediaType* pMediaType)
{
	if (iPosition == 0)
	{
		*pMediaType = m_MediaType;
		return NOERROR;
	}
	return E_INVALIDARG;
}

BOOL CVideoOutputPin::SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
	uint16_t nFrameRate)
{
	CMediaType MediaType;

	HRESULT hr = GetMediaType(0, &MediaType);
	if(SUCCEEDED(hr))
	{
		VIDEOINFO *pvi = (VIDEOINFO *) MediaType.Format();
		pvi->bmiHeader.biBitCount    = DEFAULT_COLOR_BIT;
		pvi->bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biWidth      = nVideoWidth;
		pvi->bmiHeader.biHeight     = nVideoHeight;
		pvi->bmiHeader.biSizeImage  = nVideoWidth*nVideoHeight*DEFAULT_COLOR_BIT/8;

		if(IsConnected())
		{
			// ¶¯Ì¬ÖØ½¨
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
CVideoCapture::CVideoCapture(LPUNKNOWN punk, HRESULT *phr)
	: CBaseFilter(L"Video Capture", punk, &m_FilterLock, CLSID_VideoCapture)
{
    ASSERT(phr);
    CAutoLock lck(&m_FilterLock);
	m_OutPin  = new CVideoOutputPin(this, phr, L"Out\r\n");
    if(m_OutPin == NULL)
    {
        if(phr)
            *phr = E_OUTOFMEMORY;
    }
	m_Sample  = NULL;
}


CVideoCapture::~CVideoCapture(void)
{
    CAutoLock lck(&m_FilterLock);
    SAFE_RELEASE(m_Sample);
	SAFE_DELETE(m_OutPin);
}

int CVideoCapture::GetPinCount()
{
	return 1;
}

CBasePin* CVideoCapture::GetPin(int n)
{
	if (n == 0) 
	{
		if(m_OutPin)
            return m_OutPin;
	}
	return NULL;
}

STDMETHODIMP CVideoCapture::Stop()
{
	CAutoLock lck(&m_FilterLock);
	SAFE_RELEASE(m_Sample);
	return CBaseFilter::Stop();
}

BOOL CVideoCapture::GetSampleBuffer(PBYTE* pBuffer, uint32_t* pBufferSize)
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

BOOL CVideoCapture::DeliverHoldingSample(long inSampleSize)
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
		return SUCCEEDED(hr);
	}
	return TRUE;
}

void CVideoCapture::OnCaptureData(const char* buffer, int len)
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

BOOL CVideoCapture::SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
	uint16_t nFrameRate)
{
	if(NULL != m_OutPin)
	{
		return m_OutPin->SetVideoFormat(nVideoWidth, nVideoHeight, nFrameRate);
	}
	return FALSE;
}