#include "StdAfx.h"
#include "VolumeControl.h"


CVolumeControl::CVolumeControl(void)
	: m_enVolumeDevice(ENUM_VOLUME_NONE)
{
}


CVolumeControl::~CVolumeControl(void)
{
	Close();
}

/// 打开音量控制
BOOL CVolumeControl::Open(ENUM_VOLUME_DEVICE enVolumeDevice)
{
	// 重复打开检查
	if(ENUM_VOLUME_NONE != m_enVolumeDevice)
		return FALSE;

	if(m_oMixer.SetActiveDevice(0))
	{
		m_enVolumeDevice = enVolumeDevice;
		return TRUE;
	}

	return FALSE;
}

/// 关闭音量控制
void CVolumeControl::Close(void)
{
	m_enVolumeDevice = ENUM_VOLUME_NONE;
	m_oMixer.Close();
}

/// 判断是否静音
BOOL CVolumeControl::IsMute(void) const
{
	BOOL bIsMute = FALSE;

	switch(m_enVolumeDevice)
	{
	case ENUM_VOLUME_NONE:
		break;
	case ENUM_VOLUME_CD:
		m_oMixer.GetCDMute(&bIsMute);
		break;
	case ENUM_VOLUME_WAVE:
		m_oMixer.GetWaveMute(&bIsMute);
		break;
	case ENUM_VOLUME_MIDI:
		m_oMixer.GetMidiMute(&bIsMute);
		break;
	case ENUM_VOLUME_LINEIN:
		m_oMixer.GetLineInMute(&bIsMute);
		break;
	case ENUM_VOLUME_SPEAKER:
		m_oMixer.GetSpeakerMute(&bIsMute);
		break;
	case ENUM_VOLUME_MICROPHONE:
		m_oMixer.GetMicrophoneMute(&bIsMute);
		break;
	case ENUM_VOLUME_WAVAIN_RECORD:
	case ENUM_VOLUME_LINEIN_RECORD:
	case ENUM_VOLUME_MICROPHONE_RECORD:
		break;
	default:
		break;
	}
	return bIsMute;
}

/// 设置静音
BOOL CVolumeControl::SetMutex(BOOL bIsMutex)
{
	BOOL bResult = FALSE;
	switch(m_enVolumeDevice)
	{
		case ENUM_VOLUME_NONE:
			break;
		case ENUM_VOLUME_CD:
			bResult = m_oMixer.SetCDMute(bIsMutex);
			break;
		case ENUM_VOLUME_WAVE:
			bResult = m_oMixer.SetWaveMute(bIsMutex);
			break;
		case ENUM_VOLUME_MIDI:
			bResult = m_oMixer.SetMidiMute(bIsMutex);
			break;
		case ENUM_VOLUME_LINEIN:
			bResult = m_oMixer.SetLineInMute(bIsMutex);
			break;
		case ENUM_VOLUME_SPEAKER:
			bResult = m_oMixer.SetSpeakerMute(bIsMutex);
			break;
		case ENUM_VOLUME_MICROPHONE:
			bResult = m_oMixer.SetMicrophoneMute(bIsMutex);
			break;
		case ENUM_VOLUME_WAVAIN_RECORD:
		case ENUM_VOLUME_LINEIN_RECORD:
		case ENUM_VOLUME_MICROPHONE_RECORD:
			break;
		default:
			break;
	}
	return bResult;
}

/// 设置音量
BOOL CVolumeControl::SetVolume(uint32_t nLValue, uint32_t nRValue)
{
	BOOL bResult = FALSE;
	switch(m_enVolumeDevice)
	{
	case ENUM_VOLUME_NONE:
		break;
	case ENUM_VOLUME_CD:
		bResult = m_oMixer.SetCDVolume(nLValue, nRValue);
		break;
	case ENUM_VOLUME_WAVE:
		bResult = m_oMixer.SetWaveVolume(nLValue, nRValue);
		break;
	case ENUM_VOLUME_MIDI:
		bResult = m_oMixer.SetMidiVolume(nLValue, nRValue);
		break;
	case ENUM_VOLUME_LINEIN:
		bResult = m_oMixer.SetLineInVolume(nLValue, nRValue);
		break;
	case ENUM_VOLUME_SPEAKER:
		bResult = m_oMixer.SetSpeakerVolume(nLValue, nRValue);
		break;
	case ENUM_VOLUME_MICROPHONE:
		bResult = m_oMixer.SetMicrophoneVolume(nLValue);
		break;
	case ENUM_VOLUME_WAVAIN_RECORD:
		bResult = m_oMixer.SetWaveInRecordVolume(nLValue, nRValue);
		break;
	case ENUM_VOLUME_LINEIN_RECORD:
		bResult = m_oMixer.SetLineInRecordVolume(nLValue, nRValue);
		break;
	case ENUM_VOLUME_MICROPHONE_RECORD:
		bResult = m_oMixer.SetMicrophoneRecordVolume(nLValue);
		break;
	default:
		break;
	}
	return bResult;
}

/// 获得音量
BOOL CVolumeControl::GetVolume(uint32_t* pLValue, uint32_t* pRValue) const
{
	DWORD nLValue = 0;
	DWORD nRValue = 0;

	BOOL bResult = FALSE;
	switch(m_enVolumeDevice)
	{
	case ENUM_VOLUME_NONE:
		break;
	case ENUM_VOLUME_CD:
		bResult = m_oMixer.GetCDVolume(&nLValue, &nRValue);
		break;
	case ENUM_VOLUME_WAVE:
		bResult = m_oMixer.GetWaveVolume(&nLValue, &nRValue);
		break;
	case ENUM_VOLUME_MIDI:
		bResult = m_oMixer.GetMidiVolume(&nLValue, &nRValue);
		break;
	case ENUM_VOLUME_LINEIN:
		bResult = m_oMixer.GetLineInVolume(&nLValue, &nRValue);
		break;
	case ENUM_VOLUME_SPEAKER:
		bResult = m_oMixer.GetSpeakerVolume(&nLValue, &nRValue);
		break;
	case ENUM_VOLUME_MICROPHONE:
		bResult = m_oMixer.GetMicrophoneVolume(&nLValue);
		nRValue = nLValue;
		break;
	case ENUM_VOLUME_WAVAIN_RECORD:
		bResult = m_oMixer.GetWaveInRecordVolume(&nLValue, &nRValue);
		break;
	case ENUM_VOLUME_LINEIN_RECORD:
		bResult = m_oMixer.GetLineInRecordVolume(&nLValue, &nRValue);
		break;
	case ENUM_VOLUME_MICROPHONE_RECORD:
		bResult = m_oMixer.GetMicrophoneRecordVolume(&nLValue);
		nRValue = nLValue;
		break;
	default:
		break;
	}

	if(bResult)
	{
		if(NULL != pLValue)
			*pLValue = nLValue;

		if(NULL != pRValue)
			*pRValue = nRValue;
	}

	return bResult;
}