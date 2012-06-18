// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoCapture.h"
#include "DShowAPI.h"

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
// 视频默认宽度
#define DEFAULT_VIDEO_WIDTH				320
// 视频默认高度
#define DEFAULT_VIDEO_HEIGHT			240
// 视频默认色彩
#define DEFAULT_COLOR_BIT				12
// 视频默认帧率
#define DEFAULT_FRAME_RATE				15

//=============================================================================
CVideoCapture::CVideoCapture(void)
	: m_pCGBuilder(NULL)
	, m_pCaptureFilter(NULL)
	, m_nVideoWidth(DEFAULT_VIDEO_WIDTH)
	, m_nVideoHeight(DEFAULT_VIDEO_HEIGHT)
	, m_nColorBit(DEFAULT_COLOR_BIT)
	, m_nFrameRate(DEFAULT_FRAME_RATE)
	, m_pVideoReander(NULL)
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
        if(!SetVideoFormat(DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT, DEFAULT_COLOR_BIT))
            break;

		// 设置帧率
		if(!SetFrameRate(DEFAULT_FRAME_RATE))
			break;

		// 创建Read Filter
		m_pVideoReander = new CVideoRenderer(NULL, &hr);
		if(NULL == m_pVideoReander)
			break;
		m_pVideoReander->AddRef();
		m_pVideoReander->SetCaptureEvent(pCaptureEvent);

		if (FAILED(m_pGraphBulider->AddFilter(m_pVideoReander, L"Video Render")))
			break;

		// 连接Filter
		IPin* pOutPin = GetOutputPin(m_pCaptureFilter, (uint16_t)0);
		IPin* pInPin  = GetInputPin(m_pVideoReander, (uint16_t)0);

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
    SAFE_RELEASE(m_pVideoReander);
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
		uint16_t nColorBit)
{
    if(NULL == m_pCGBuilder)
        return FALSE;

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
            phead->bmiHeader.biBitCount = nColorBit; 
            phead->bmiHeader.biSizeImage = nVideoWidth * nVideoHeight * nColorBit / 8; 

            if(SUCCEEDED(iconfig->SetFormat(pmt)))
			{
				m_nVideoWidth = nVideoWidth;
				m_nVideoHeight = nVideoHeight;
				m_nColorBit = nColorBit;
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
		uint16_t* pColorBit) const
{
	if(NULL != pVideoWidth)
		*pVideoWidth = m_nVideoWidth;

	if(NULL != pVideoHeight)
		*pVideoHeight = m_nVideoHeight;

	if(NULL != pColorBit)
		*pColorBit = m_nColorBit;

	return TRUE;
}

/// 设置帧率
BOOL CVideoCapture::SetFrameRate(uint16_t nFrameRate)
{
    if(NULL == m_pCGBuilder)
        return FALSE;

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
            phead->AvgTimePerFrame =  UNITS / nFrameRate; 

            if(SUCCEEDED(iconfig->SetFormat(pmt)))
			{
				m_nFrameRate = nFrameRate;
                bReturn = TRUE;
			}
        } 
        DeleteMediaType(pmt); 
    }while(FALSE);

    SAFE_RELEASE(iconfig);

    return bReturn; 
}

/// 获得指针
uint16_t CVideoCapture::GetFrameRate(void) const
{
	return m_nFrameRate;
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
