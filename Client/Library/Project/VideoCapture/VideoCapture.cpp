// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoCapture.h"
#include "DShowAPI.h"
#include "DeviceDefine.h"

#ifdef NDEBUG
	#ifndef _UNICODE
		#pragma comment(lib,"strmbase.lib")
		#pragma message("LINK strmbase.lib")
	#else
		#pragma comment(lib,"strmbaseu.lib")
		#pragma message("LINK strmbaseu.lib")
	#endif
#else
	#ifndef _UNICODE
		#pragma comment(lib,"strmbasd.lib")
		#pragma message("LINK strmbasd.lib")
	#else
		#pragma comment(lib,"strmbasdu.lib")
		#pragma message("LINK strmbasdu.lib")
	#endif
#endif


//#pragma comment(lib,"Version.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"strmiids.lib")


//=============================================================================
CVideoCapture::CVideoCapture(void)
	: m_pCGBuilder(NULL)
	, m_pCaptureFilter(NULL)
	, m_nVideoWidth(DEFAULT_VIDEO_WIDTH)
	, m_nVideoHeight(DEFAULT_VIDEO_HEIGHT)
	, m_nFrameRate(DEFAULT_FRAME_RATE)
	, m_pVideoRender(NULL)
{
}


CVideoCapture::~CVideoCapture(void)
{
	Close();
}

/// 打开视频采集设备
BOOL CVideoCapture::Open(ICaptureEvent* pCaptureEvent, 
	const TCHAR* szDeviceName)
{
	ASSERT(pCaptureEvent);

	// 参数检查
	if(NULL == pCaptureEvent)
		return FALSE;

	BOOL bResult = FALSE;
    do
    {
		// 创建GraphBuilder
        if(!CreateGraphBuilder())
            break;

		// 创建CaptureGraphBuilder
        HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, 
            CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&m_pCGBuilder);
        if(FAILED(hr))
		{
			break;
		}

        hr = m_pCGBuilder->SetFiltergraph(m_pGraphBulider);
        if(FAILED(hr))
		{
			break;
		}

		// 创建视频采集设备
		m_pCaptureFilter = CreateCaptureFiler(szDeviceName);
		if(NULL == m_pCaptureFilter)
			break;

		if (FAILED(m_pGraphBulider->AddFilter(m_pCaptureFilter, L"Video Capture")))
			break;

		// 设置视频信息
        if(!SetVideoFormat(m_nVideoWidth, m_nVideoHeight, m_nFrameRate))
            break;

		// 创建Read Filter
		m_pVideoRender = new CVideoRenderer(NULL, &hr);
		if(NULL == m_pVideoRender)
			break;
		m_pVideoRender->AddRef();
		m_pVideoRender->SetCaptureEvent(pCaptureEvent);

		if (FAILED(m_pGraphBulider->AddFilter(m_pVideoRender, L"Video Render")))
			break;

		// 连接Filter
		IPin* pOutPin = GetOutputPin(m_pCaptureFilter, (uint16_t)0);
		IPin* pInPin  = GetInputPin(m_pVideoRender, (uint16_t)0);

		hr = m_pGraphBulider->Connect(pOutPin, pInPin);
		SAFE_RELEASE(pOutPin);
		SAFE_RELEASE(pInPin);

		if(FAILED(hr))
			break;

        bResult = TRUE;
    }while(FALSE);

    if(!bResult)
        Close();

    return bResult;
}

/// 关闭视频采集设备
void CVideoCapture::Close(void)
{
    if(!IsStopped())
        Stop();

    SAFE_RELEASE(m_pCaptureFilter);
    SAFE_RELEASE(m_pVideoRender);
    SAFE_RELEASE(m_pCGBuilder);

    DestroyGraphBuilder();
}

/// 设备采集设备是否打开
BOOL CVideoCapture::IsOpened(void) const
{
	return (NULL != m_pGraphBulider);
}

/// 开始视频采集
BOOL CVideoCapture::StartCapture(void)
{
	return Run();
}

/// 暂停视频采集
BOOL CVideoCapture::PauseCapture(void)
{
	return Pause();
}

/// 停止视频采集
BOOL CVideoCapture::StopCapture(void)
{
	return Stop();
}

/// 设置视频信息
BOOL CVideoCapture::SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
		uint16_t nFrameRate)
{
    if(NULL == m_pCGBuilder)
	{
		m_nVideoWidth = nVideoWidth;
		m_nVideoHeight = nVideoHeight;
		m_nFrameRate = nFrameRate;
		return TRUE;
	}

    ASSERT(IsStopped());
    if(!IsStopped())
        return FALSE;

    IAMStreamConfig* iconfig = NULL; 
    BOOL bReturn = FALSE;

    do
    {
        HRESULT hr = NOERROR;   
        hr = m_pCGBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, 
            m_pCaptureFilter, IID_IAMStreamConfig, (void**)&iconfig); 
        if(hr != NOERROR)   
        { 
            hr = m_pCGBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                m_pCaptureFilter, IID_IAMStreamConfig, (void**)&iconfig); 
        } 

        if(FAILED(hr))
            break;

        AM_MEDIA_TYPE* pmt; 
        if(iconfig->GetFormat(&pmt) != S_OK) 
            break;

        if(pmt->formattype == FORMAT_VideoInfo) 
        { 
            VIDEOINFOHEADER* phead =(VIDEOINFOHEADER*)pmt->pbFormat; 
            phead->bmiHeader.biWidth = nVideoWidth; 
            phead->bmiHeader.biHeight = nVideoHeight;
            phead->bmiHeader.biBitCount = DEFAULT_COLOR_BIT; 
            phead->bmiHeader.biSizeImage = nVideoWidth * nVideoHeight * DEFAULT_COLOR_BIT / 8; 
			phead->AvgTimePerFrame =  UNITS / nFrameRate; 

			hr = iconfig->SetFormat(pmt);
            if(SUCCEEDED(hr))
			{
				m_nVideoWidth = nVideoWidth;
				m_nVideoHeight = nVideoHeight;
				m_nFrameRate = nFrameRate;
                bReturn = TRUE;
			}
        } 
        DeleteMediaType(pmt); 
    }while(FALSE);

    SAFE_RELEASE(iconfig);

    return bReturn; 
}

/// 获得视频信息
BOOL CVideoCapture::GetVideoFormat(uint16_t* pVideoWidth, uint16_t* pVideoHeight, 
		uint16_t* pFrameRate) const
{
	if(NULL != pVideoWidth)
		*pVideoWidth = m_nVideoWidth;

	if(NULL != pVideoHeight)
		*pVideoHeight = m_nVideoHeight;

	if(NULL != pFrameRate)
		*pFrameRate = m_nFrameRate;

	return TRUE;
}

/// 获得设备列表 
uint16_t CVideoCapture::GetVideoDeviceInfo(
		device_info_t* pArrDeviceInfo, uint16_t nArrCount)
{
	return QueryVideoCaptureDevices(pArrDeviceInfo, nArrCount);
}

/// 底层事件通知
void CVideoCapture::OnNotify(int nNotifyCode)
{

}

//=============================================================================
IBaseFilter* CVideoCapture::CreateCaptureFiler(const TCHAR* szDeviceName)
{
	IBaseFilter* pCaptureFilter = NULL;
	// 创建视频采集设备
	if(NULL != szDeviceName)
	{
		pCaptureFilter = CreateVideoDeviceFilter(szDeviceName);
	}
	else
	{
		device_info_t arrDeviceInfo[16];
		uint16_t nDeviceCount = QueryVideoCaptureDevices(arrDeviceInfo, 16);
		for(uint16_t nIndex=0; nIndex<nDeviceCount; ++nIndex)
		{
			pCaptureFilter = CreateVideoDeviceFilter(
				arrDeviceInfo[nIndex].m_szDeviceName);
			if(NULL != pCaptureFilter)
				break;
		}
	}
	return pCaptureFilter;
}
