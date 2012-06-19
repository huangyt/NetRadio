// VideoPlayer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "VideoPlayer.h"
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
CVideoPlayer::CVideoPlayer(void)
	: m_pVideoCapture(NULL)
	, m_pVideoRender(NULL)
	, m_pBasicVideo(NULL)
	, m_pVideoWindow(NULL)
	, m_nVideoWidth(DEFAULT_VIDEO_WIDTH)
	, m_nVideoHeight(DEFAULT_VIDEO_HEIGHT)
	, m_nFrameRate(DEFAULT_FRAME_RATE)
{
}


CVideoPlayer::~CVideoPlayer(void)
{
	Close();
}

/// 打开
BOOL CVideoPlayer::Open(HWND hWndVideo)
{
	BOOL bResult = FALSE;
	do
	{
		// 创建GraphBuilder
        if(!CreateGraphBuilder())
            break;

		if(FAILED(m_pGraphBulider->QueryInterface(IID_IBasicVideo, (void **)&m_pBasicVideo)))
			break;

		if(FAILED(m_pGraphBulider->QueryInterface(IID_IVideoWindow, (void **)&m_pVideoWindow)))
			break;

		HRESULT hr = NOERROR;
		// 创建VideoCaptreu
		m_pVideoCapture = new CVideoCapture(NULL, &hr);
		if(NULL == m_pVideoCapture)
			break;
		m_pVideoCapture->AddRef();
		if (FAILED(m_pGraphBulider->AddFilter(m_pVideoCapture, L"Video Capture")))
			break;

		// 设置视频信息
		if(!SetVideoFormat(m_nVideoWidth, m_nVideoHeight, m_nFrameRate))
			break;

		// 创建VideoReander
		hr = CoCreateInstance(CLSID_VideoRendererDefault, NULL, CLSCTX_INPROC_SERVER, 
            IID_IBaseFilter, (void **)&m_pVideoRender);
		if (FAILED(hr))
			break;
		if (FAILED(m_pGraphBulider->AddFilter(m_pVideoRender, L"Video Renderer")))
			break;

		// 连接Filter
		IPin* pOutPin = GetOutputPin(m_pVideoCapture, (uint16_t)0);
		IPin* pInPin  = GetInputPin(m_pVideoRender, (uint16_t)0);

		hr = m_pGraphBulider->Connect(pOutPin, pInPin);
		SAFE_RELEASE(pOutPin);
		SAFE_RELEASE(pInPin);

		if(FAILED(hr))
			break;

		// 设置显示窗口
		if(!SetDisplayWindow(hWndVideo))
			break;

		bResult = TRUE;
	}while(FALSE);

	if(!bResult)
		Close();

    return bResult;
}

/// 关闭
void CVideoPlayer::Close(void)
{
    if(!IsStopped())
        Stop();

    SAFE_RELEASE(m_pVideoCapture);
    SAFE_RELEASE(m_pVideoRender);
    SAFE_RELEASE(m_pBasicVideo);
	SAFE_RELEASE(m_pVideoWindow);

    DestroyGraphBuilder();
}

/// 是否打开
BOOL CVideoPlayer::IsOpened(void) const
{
	return (NULL != m_pGraphBulider);
}

/// 开始
BOOL CVideoPlayer::StartPlay(void)
{
	return Run();
}

/// 暂停
BOOL CVideoPlayer::PausePlay(void)
{
	return Pause();
}

/// 停止视频采集
BOOL CVideoPlayer::StopPlay(void)
{
	return Stop();
}

/// 设置全屏
BOOL CVideoPlayer::SetFullScreen(BOOL bIsFullScreen)
{
	if (m_pVideoWindow)
	{
		HRESULT hr = m_pVideoWindow->put_FullScreenMode(bIsFullScreen ? OATRUE : OAFALSE);
		return SUCCEEDED(hr);
	}
	return FALSE;
}

/// 设置视频的显示窗口
BOOL CVideoPlayer::SetDisplayWindow(HWND hWndVideo)
{
	if(m_pVideoWindow)
	{
		// Hide the video window first
		m_pVideoWindow->put_Visible(OAFALSE);
		m_pVideoWindow->put_Owner((OAHWND)hWndVideo);

		RECT windowRect;
		::GetClientRect(hWndVideo, &windowRect);
		m_pVideoWindow->put_Left(0);
		m_pVideoWindow->put_Top(0);
		m_pVideoWindow->put_Width(windowRect.right - windowRect.left);
		m_pVideoWindow->put_Height(windowRect.bottom - windowRect.top);
		m_pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		m_pVideoWindow->put_MessageDrain((OAHWND) hWndVideo);

		// Restore the video window
		if (hWndVideo != NULL)
			m_pVideoWindow->put_Visible(OATRUE);
		else
			m_pVideoWindow->put_Visible(OAFALSE);
		return TRUE;
	}
	return FALSE;
}

/// 是否全屏
BOOL CVideoPlayer::IsFullScreen(void) const
{
	if (m_pVideoWindow)
	{
		long  fullScreenMode = OAFALSE;
		m_pVideoWindow->get_FullScreenMode(&fullScreenMode);
		return (fullScreenMode == OATRUE);
	}
	return FALSE;
}

/// 设置视频信息
BOOL CVideoPlayer::SetVideoFormat(uint16_t nVideoWidth, uint16_t nVideoHeight, 
	uint16_t nFrameRate)
{
	ASSERT(IsStopped());
	if(!IsStopped())
		return FALSE;

	BOOL bReturn = FALSE;
	if(NULL != m_pVideoCapture)
	{
		bReturn = m_pVideoCapture->SetVideoFormat(nVideoWidth, nVideoHeight,
			nFrameRate);
	}
	else
	{
		m_nVideoWidth = nVideoWidth;
		m_nVideoHeight = nVideoHeight;
		m_nFrameRate = nFrameRate;
		bReturn = TRUE;
	}

	return bReturn; 
}

/// 获得视频信息
BOOL CVideoPlayer::GetVideoFormat(uint16_t* pVideoWidth, uint16_t* pVideoHeight, 
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

/// 接收视频数据
void CVideoPlayer::OnVideoData(const char* pVideoData, uint32_t nDataSize, 
		uint64_t nTimeStamp)
{
	if(NULL != m_pVideoCapture)
	{
		m_pVideoCapture->OnCaptureData(pVideoData, nDataSize);
	}
}

void CVideoPlayer::OnNotify(int nNotifyCode)
{

}