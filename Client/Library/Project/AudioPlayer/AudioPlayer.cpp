// VideoPlayer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AudioPlayer.h"
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
CAudioPlayer::CAudioPlayer(void)
	: m_pAudioCapture(NULL)
	, m_pAudioRender(NULL)
	, m_pBasicAudio(NULL)
	, m_enFrequency(ENUM_FREQUENCY_22KHZ)
	, m_enChannel(ENUM_CHANNEL_STEREO)
	, m_enSample(ENUM_SAMPLE_16BIT)
{
}

CAudioPlayer::~CAudioPlayer(void)
{
	Close();
}

/// 打开
BOOL CAudioPlayer::Open(void)
{
	BOOL bResult = FALSE;
	do
	{
		// 创建GraphBuilder
		if(!CreateGraphBuilder())
			break;

		HRESULT hr = NOERROR;
		if(FAILED(m_pGraphBulider->QueryInterface(IID_IBasicAudio, (void **)&m_pBasicAudio)))
			break;

		// 创建AudioCaptreu
		m_pAudioCapture = new CAudioCapture(NULL, &hr);
		if(NULL == m_pAudioCapture)
			break;
		m_pAudioCapture->AddRef();
		if (FAILED(m_pGraphBulider->AddFilter(m_pAudioCapture, L"Audio Capture")))
			break;

		// 设置音频信息
		if(!SetAudioFormat(m_enFrequency, m_enChannel, m_enSample))
			break;

		// 创建AudioReander
		hr = CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, 
			IID_IBaseFilter, (void **)&m_pAudioRender);
		if (FAILED(hr))
			break;
		if (FAILED(m_pGraphBulider->AddFilter(m_pAudioRender, L"Audio Renderer")))
			break;

		// 连接Filter
		IPin* pOutPin = GetOutputPin(m_pAudioCapture, (uint16_t)0);
		IPin* pInPin  = GetInputPin(m_pAudioRender, (uint16_t)0);

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

/// 关闭
void CAudioPlayer::Close(void)
{
	if(!IsStopped())
		Stop();

	SAFE_RELEASE(m_pAudioCapture);
	SAFE_RELEASE(m_pAudioRender);
	SAFE_RELEASE(m_pBasicAudio);

	DestroyGraphBuilder();
}

/// 是否打开
BOOL CAudioPlayer::IsOpened(void) const
{
	return (NULL != m_pGraphBulider);
}

/// 开始
BOOL CAudioPlayer::StartPlay(void)
{
	return Run();
}

/// 暂停
BOOL CAudioPlayer::PausePlay(void)
{
	return Pause();
}

/// 停止
BOOL CAudioPlayer::StopPlay(void)
{
	return Stop();
}

/// 设置音频信息
BOOL CAudioPlayer::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency,
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
{
	ASSERT(IsStopped());
	if(!IsStopped())
		return FALSE;

	BOOL bReturn = FALSE;
	if(NULL != m_pAudioCapture)
	{
		bReturn = m_pAudioCapture->SetAudioFormat(enFrequency, enChannel,
			enSample);
	}
	else
	{
		m_enFrequency = enFrequency;
		m_enChannel = enChannel;
		m_enSample = enSample;
		bReturn = TRUE;
	}

	return bReturn; 
}

/// 获得音频信息
BOOL CAudioPlayer::GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
	ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const
{
	enFrequency = m_enFrequency;
	enChannel = m_enChannel;
	enSample = m_enSample;
	return TRUE;
}

/// 接收音频数据
void CAudioPlayer::OnAudioData(const char* pVideoData, uint32_t nDataSize, 
	uint64_t nTimeStamp)
{
	if(NULL != m_pAudioCapture)
	{
		m_pAudioCapture->OnCaptureData(pVideoData, nDataSize);
	}
}

void CAudioPlayer::OnNotify(int nNotifyCode)
{

}