// VideoCapture.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AudioCapture.h"
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
CAudioCapture::CAudioCapture(void)
	: m_pCGBuilder(NULL)
	, m_pCaptureFilter(NULL)
	, m_enFrequency(ENUM_FREQUENCY_22KHZ)
	, m_enChannel(ENUM_CHANNEL_STEREO)
	, m_enSample(ENUM_SAMPLE_16BIT)
	, m_pAudioReander(NULL)
{
}


CAudioCapture::~CAudioCapture(void)
{
	Close();
}

/// 打开音频采集设备
BOOL CAudioCapture::Open(ICaptureEvent* pCaptureEvent, 
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

		if (FAILED(m_pGraphBulider->AddFilter(m_pCaptureFilter, L"Audio Capture")))
			break;

		// 设置视频信息
		if(!SetAudioFormat(ENUM_FREQUENCY_22KHZ, ENUM_CHANNEL_STEREO, ENUM_SAMPLE_16BIT))
			break;

		// 创建Read Filter
		m_pAudioReander = new CAudioRenderer(NULL, &hr);
		if(NULL == m_pAudioReander)
			break;
		m_pAudioReander->AddRef();
		m_pAudioReander->SetCaptureEvent(pCaptureEvent);

		if (FAILED(m_pGraphBulider->AddFilter(m_pAudioReander, L"Audio Render")))
			break;

		// 连接Filter
		IPin* pOutPin = GetOutputPin(m_pCaptureFilter, (uint16_t)0);
		IPin* pInPin  = GetInputPin(m_pAudioReander, (uint16_t)0);

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

/// 关闭音频采集设备
void CAudioCapture::Close(void)
{
	if(!IsStopped())
		Stop();

	SAFE_RELEASE(m_pCaptureFilter);
	SAFE_RELEASE(m_pAudioReander);
	SAFE_RELEASE(m_pCGBuilder);

	DestroyGraphBuilder();
}

/// 设备采集设备是否打开
BOOL CAudioCapture::IsOpened(void) const
{
	return (NULL != m_pGraphBulider);
}

/// 开始音频采集
BOOL CAudioCapture::StartCapture(void)
{
	return Run();
}

/// 暂停音频采集
BOOL CAudioCapture::PauseCapture(void)
{
	return Pause();
}

/// 停止音频采集
BOOL CAudioCapture::StopCapture(void)
{
	return Stop();
}

/// 设置音频信息
BOOL CAudioCapture::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency,
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
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

		if(pmt->formattype == FORMAT_WaveFormatEx) 
		{ 
			WAVEFORMATEX* pWF=(WAVEFORMATEX*)pmt->pbFormat;
			pWF->nSamplesPerSec = enFrequency;
			pWF->nChannels = enChannel;
			pWF->wBitsPerSample = enSample;
			pWF->nAvgBytesPerSec = enSample * enFrequency * enChannel / 8;

			if(SUCCEEDED(iconfig->SetFormat(pmt)))
			{
				m_enFrequency = enFrequency;
				m_enChannel = enChannel;
				m_enSample = enSample;
				bReturn = TRUE;
			}
		} 
		DeleteMediaType(pmt); 
	}while(FALSE);

	SAFE_RELEASE(iconfig);

	return bReturn; 
}

/// 获得音频信息
BOOL CAudioCapture::GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
	ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const
{
	enFrequency = m_enFrequency;
	enChannel = m_enChannel;
	enSample = m_enSample;

	return TRUE;
}

/// 获得设备列表 
uint16_t CAudioCapture::GetAudioDeviceInfo(
	device_info_t* pArrDeviceInfo, uint16_t nArrCount)
{
	return QueryAudioCaptureDevices(pArrDeviceInfo, nArrCount);
}

/// 底层事件通知
void CAudioCapture::OnNotify(int nNotifyCode)
{

}

//=============================================================================
IBaseFilter* CAudioCapture::CreateCaptureFiler(const TCHAR* szDeviceName)
{
	IBaseFilter* pCaptureFilter = NULL;
	// 创建音频采集设备
	if(NULL != szDeviceName)
	{
		pCaptureFilter = CreateAudioDeviceFilter(szDeviceName);
	}
	else
	{
		device_info_t arrDeviceInfo[16];
		uint16_t nDeviceCount = QueryAudioCaptureDevices(arrDeviceInfo, 16);
		for(uint16_t nIndex=0; nIndex<nDeviceCount; ++nIndex)
		{
			pCaptureFilter = CreateAudioDeviceFilter(
				arrDeviceInfo[nIndex].m_szDeviceName);
			if(NULL != pCaptureFilter)
				break;
		}
	}
	return pCaptureFilter;
}
