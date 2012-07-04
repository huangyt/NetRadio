#include "stdafx.h"
#include "Mixer.h"
#include <tchar.h>

CMixer::CMixer()
{
	m_nCurMixId = -1;
	m_hWnd  = NULL;
	m_hmx   = NULL;
	memset(m_arrDevName,0,sizeof(m_arrDevName));
}


CMixer::~CMixer()
{
	Close();
}


BOOL CMixer::Close()
{
	mixerClose(m_hmx);
	m_nCurMixId = -1;
	return TRUE ;
}

UINT CMixer::GetNumDevs() const
{
	return m_nNumMixers;
}

BOOL CMixer::GetDevCaps(LPMIXERCAPS pmxcaps) const
{
	if (MMSYSERR_NOERROR != mixerGetDevCaps(m_nCurMixId, pmxcaps, sizeof(MIXERCAPS)))
		return FALSE;
	return TRUE;
}

BOOL CMixer::InitMixer(HWND hWnd)
{
	if(m_hWnd != NULL)
	{
		Close() ;
	}

	UINT nDefID = 0 ;
	MIXERCAPS   mxcaps;
	//CString		strDevName;
	m_nNumMixers = mixerGetNumDevs();
	for(UINT i=0; i<m_nNumMixers; i++)
	{
		mixerGetDevCaps(i, &mxcaps, sizeof(MIXERCAPS)) ;
		//strDevName = mxcaps.szPname ;
		if(i<4) 
			_tcsncpy(m_arrDevName[i], mxcaps.szPname, 65);

		//if(strDevName.Find("USB Audio") != -1)
		//{
		//	nDefID = i ;
		//}
	}
	m_hWnd = hWnd;

	return SetActiveDevice(nDefID);
}

BOOL CMixer::SetActiveDevice(UINT nSetDevID)
{
	Close() ;
	if (MMSYSERR_NOERROR != mixerOpen(&m_hmx, nSetDevID,(DWORD)m_hWnd, 0, CALLBACK_WINDOW))
	{
		return FALSE;
	}

	if (MMSYSERR_NOERROR != mixerGetID((HMIXEROBJ)m_hmx, &nSetDevID, MIXER_OBJECTF_HMIXER))
	{
		return FALSE;
	}
	if(nSetDevID == -1)
		return FALSE;

	m_nCurMixId = nSetDevID ;

	return TRUE;
}


BOOL CMixer::GetLineInfo(LPMIXERLINE pmxl, DWORD fdwInfo) const
{
	if (MMSYSERR_NOERROR != mixerGetLineInfo((HMIXEROBJ)m_nCurMixId, pmxl, fdwInfo))
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CMixer::GetLineInfo(LPMIXERLINE pmxl, DWORD dwDstType,DWORD dwSrcType) const
{
	MIXERCAPS   mxcaps;

	if (! GetDevCaps(&mxcaps))
		return FALSE;
	UINT u=0;
	do
	{
		pmxl->cbStruct      = sizeof(*pmxl);
		pmxl->dwDestination = u;
		u++;
		if (! GetLineInfo(pmxl, MIXER_GETLINEINFOF_DESTINATION))
			continue;
	} while ((u < mxcaps.cDestinations) && (pmxl->dwComponentType != dwDstType));

	if (u > mxcaps.cDestinations)
		return FALSE;

	if (dwDstType == dwSrcType)
		return TRUE;

	pmxl->dwDestination = u;
	u--;
	UINT cConnections = (UINT)pmxl->cConnections;
	UINT  v=0;
	do
	{
		pmxl->cbStruct      = sizeof(*pmxl);
		pmxl->dwDestination = u;
		pmxl->dwSource      = v;
		v++;
		if (! GetLineInfo(pmxl, MIXER_GETLINEINFOF_SOURCE))
			continue;
	} while ((v < cConnections) && (pmxl->dwComponentType != dwSrcType));

	if((v > cConnections) || (pmxl->dwComponentType !=dwSrcType))
		return FALSE;

	return TRUE;
}


BOOL CMixer::GetLineControls(LPMIXERLINECONTROLS pmxlc, DWORD fdwControls) const
{
	if (MMSYSERR_NOERROR != mixerGetLineControls((HMIXEROBJ)m_nCurMixId, pmxlc, fdwControls))
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CMixer::GetLineControl(LPMIXERCONTROL pmxc, LPMIXERLINE pmxl, DWORD dwType) const
{
	LPMIXERCONTROL  pamxctrl;
	DWORD cbmxctrls = sizeof(*pamxctrl) * (UINT)pmxl->cControls;
	pamxctrl = (LPMIXERCONTROL)LocalAlloc(LPTR, cbmxctrls);

	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct       = sizeof(mxlc);
	mxlc.dwLineID       = pmxl->dwLineID;
	mxlc.dwControlType  = dwType;
	mxlc.cControls      = pmxl->cControls;
	mxlc.cbmxctrl       = sizeof(*pamxctrl);
	mxlc.pamxctrl       = pamxctrl;
	if (! GetLineControls(&mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE))
		return FALSE;

	memcpy(pmxc, pamxctrl, sizeof(*pamxctrl));
	return TRUE;
}


BOOL CMixer::GetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails) const
{
	if (MMSYSERR_NOERROR != mixerGetControlDetails((HMIXEROBJ)m_nCurMixId, pmxcd, fdwDetails))
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CMixer::SetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
{
	if (MMSYSERR_NOERROR != mixerSetControlDetails((HMIXEROBJ)m_nCurMixId, pmxcd, fdwDetails))
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CMixer::GetVolume(DWORD dwSrcType, DWORD* pdwLValue, DWORD* pdwRValue, BOOL bMono) const
{
	MIXERLINE   mxl;
	if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, dwSrcType))
		return FALSE;
	MIXERCONTROL mxc;
	if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_VOLUME))
		return FALSE;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED    mxcd_u1;
	MIXERCONTROLDETAILS_UNSIGNED    mxcd_u[2];

	mxcd.cbStruct       = sizeof(mxcd);
	mxcd.dwControlID    = mxc.dwControlID;
	mxcd.cMultipleItems = 0;

	if (bMono)
	{
		mxcd.cChannels      = 1;
		mxcd.cbDetails      = sizeof(mxcd_u1);
		mxcd.paDetails      = &mxcd_u1;

		if (! GetControlDetails(&mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
			return FALSE;

		*pdwLValue = mxcd_u1.dwValue;
	}
	else
	{
		mxcd.cChannels      =  mxl.cChannels;
		mxcd.cbDetails      = sizeof(*mxcd_u);
		mxcd.paDetails      = mxcd_u;
		if (! GetControlDetails(&mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
			return FALSE;
		*pdwLValue = mxcd_u[0].dwValue;
		*pdwRValue = mxcd_u[1].dwValue;
	}
	return TRUE;
}



BOOL CMixer::SetVolume(DWORD dwSrcType, DWORD dwLValue, DWORD dwRValue, BOOL bMono)
{
	MIXERLINE mxl;
	if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,dwSrcType))
		return FALSE;

	MIXERCONTROL mxc;
	if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_VOLUME))
		return FALSE;

	MIXERCONTROLDETAILS   mxcd;
	MIXERCONTROLDETAILS_UNSIGNED  mxcd_u1;
	MIXERCONTROLDETAILS_UNSIGNED  mxcd_u[2];
	mxcd.cbStruct       = sizeof(mxcd);
	mxcd.dwControlID    = mxc.dwControlID;
	mxcd.cMultipleItems = 0;

	if (bMono)
	{
		mxcd.cChannels   = 1;
		mxcd.cbDetails   = sizeof(mxcd_u1);
		mxcd.paDetails   = &mxcd_u1;
		mxcd_u1.dwValue  = dwLValue;
	}
	else
	{
		mxcd.cChannels     = mxl.cChannels;
		mxcd.cbDetails     = sizeof(*mxcd_u);
		mxcd.paDetails     = mxcd_u;
		mxcd_u[0].dwValue  = dwLValue;
		mxcd_u[1].dwValue  = dwRValue;
	}
	if (! SetControlDetails(&mxcd, MIXER_OBJECTF_MIXER))
		return FALSE;
	return TRUE;
}


BOOL CMixer::GetMute(DWORD dwSrcType, BOOL* pbValue) const
{
	MIXERLINE mxl;
	if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,dwSrcType))
		return FALSE;

	MIXERCONTROL mxc;

	if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_MUTE))
		return FALSE;

	MIXERCONTROLDETAILS   mxcd;
	MIXERCONTROLDETAILS_BOOLEAN   mxcd_f;
	mxcd.cbStruct       = sizeof(mxcd);
	mxcd.dwControlID    = mxc.dwControlID;
	mxcd.cChannels      = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails      = sizeof(mxcd_f);
	mxcd.paDetails      = &mxcd_f;

	if (! GetControlDetails(&mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
		return FALSE;

	*pbValue = mxcd_f.fValue;
	return TRUE;
}


BOOL CMixer::SetMute(DWORD dwSrcType, BOOL bValue)
{
	MIXERLINE mxl;
	if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,dwSrcType))
		return FALSE;

	MIXERCONTROL mxc;

	if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_MUTE))
		return FALSE;
	MIXERCONTROLDETAILS   mxcd;
	MIXERCONTROLDETAILS_BOOLEAN   mxcd_f;

	mxcd.cbStruct       = sizeof(mxcd);
	mxcd.dwControlID    = mxc.dwControlID;
	mxcd.cChannels      = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails      = sizeof(mxcd_f);
	mxcd.paDetails      = &mxcd_f;
	mxcd_f.fValue		= bValue;

	if (! SetControlDetails(&mxcd, MIXER_OBJECTF_MIXER))
		return FALSE;

	return TRUE;
}

BOOL CMixer::GetRecordVolume(DWORD dwSrcType, DWORD* pdwLValue, DWORD* pdwRValue, BOOL bMono) const
{
	MIXERLINE mxl;

	if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_WAVEIN,dwSrcType))
		return FALSE;

	MIXERCONTROL mxc;
	if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_VOLUME))
		return FALSE;

	MIXERCONTROLDETAILS   mxcd;
	MIXERCONTROLDETAILS_UNSIGNED  mxcd_u[2];

	mxcd.cbStruct       = sizeof(mxcd);
	mxcd.dwControlID    = mxc.dwControlID;
	mxcd.cChannels      = bMono ? 1 : mxl.cChannels;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails      = sizeof(*mxcd_u);
	mxcd.paDetails      = mxcd_u;

	if (! GetControlDetails(&mxcd, MIXER_GETCONTROLDETAILSF_VALUE))
		return FALSE;
	*pdwLValue = mxcd_u[0].dwValue;
	*pdwRValue = mxcd_u[1].dwValue;
	return TRUE;
}

BOOL CMixer::SetRecordVolume(DWORD dwSrcType, DWORD dwLValue,DWORD dwRValue, BOOL bMono)
{
	MIXERLINE mxl;
	if (! GetLineInfo(&mxl, MIXERLINE_COMPONENTTYPE_DST_WAVEIN,dwSrcType))
		return FALSE;

	MIXERCONTROL mxc;
	if (! GetLineControl(&mxc, &mxl, MIXERCONTROL_CONTROLTYPE_VOLUME))
		return FALSE;

	MIXERCONTROLDETAILS   mxcd;
	MIXERCONTROLDETAILS_UNSIGNED  mxcd_u[2];

	mxcd.cbStruct       = sizeof(mxcd);
	mxcd.dwControlID    = mxc.dwControlID;
	mxcd.cChannels      = bMono ? 1 : mxl.cChannels;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails      = sizeof(*mxcd_u);
	mxcd.paDetails      = mxcd_u;
	mxcd_u[0].dwValue   = dwLValue;
	mxcd_u[1].dwValue   = dwRValue;
	if (! SetControlDetails(&mxcd, MIXER_OBJECTF_MIXER))
		return FALSE;
	return TRUE;
}

BOOL CMixer::SetCDVolume(DWORD dwLValue, DWORD dwRValue)
{
	return SetVolume(MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, dwLValue, dwRValue);
}

BOOL CMixer::GetCDVolume(DWORD* pdwLValue, DWORD* pdwRValue) const
{
	return GetVolume(MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, pdwLValue, pdwRValue);
}

BOOL CMixer::SetSpeakerVolume(DWORD dwLValue,DWORD dwRValue)
{
	return SetVolume(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, dwLValue, dwRValue);
}

BOOL CMixer::GetSpeakerVolume(DWORD* pdwLValue,DWORD* pdwRValue) const
{
	return GetVolume(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, pdwLValue, pdwRValue);
}

BOOL CMixer::SetLineInVolume(DWORD dwLValue,DWORD dwRValue)
{
	return SetVolume(MIXERLINE_COMPONENTTYPE_SRC_LINE, dwLValue, dwRValue);
}

BOOL CMixer::GetLineInVolume(DWORD* pdwLValue,DWORD* pdwRValue) const
{
	return GetVolume(MIXERLINE_COMPONENTTYPE_SRC_LINE, pdwLValue, pdwRValue);
}

BOOL CMixer::SetMicrophoneVolume(DWORD dwLValue)
{
	return SetVolume(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, dwLValue, 0, TRUE);
}

BOOL CMixer::GetMicrophoneVolume(DWORD* pdwLValue) const
{
	DWORD dwRValue;
	return GetVolume(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, pdwLValue, &dwRValue, TRUE);
}

BOOL CMixer::SetMidiVolume(DWORD dwLValue,DWORD dwRValue)
{
	return SetVolume(MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER, dwLValue, dwRValue);
}

BOOL CMixer::GetMidiVolume(DWORD* pdwLValue,DWORD* pdwRValue) const
{
	return GetVolume(MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,pdwLValue, pdwRValue);
}

BOOL CMixer::SetWaveVolume(DWORD dwLValue,DWORD dwRValue)
{
	return SetVolume(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, dwLValue, dwRValue);
}

BOOL CMixer::GetWaveVolume(DWORD* pdwLValue,DWORD* pdwRValue) const
{
	return GetVolume(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, pdwLValue, pdwRValue);
}

BOOL CMixer::SetCDMute(BOOL bValue)
{
	return SetMute(MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, bValue);
}

BOOL CMixer::GetCDMute(BOOL* pbValue) const
{
	return GetMute(MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, pbValue);
}

BOOL CMixer::SetSpeakerMute(BOOL bValue)
{
	return SetMute(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, bValue);
}

BOOL CMixer::GetSpeakerMute(BOOL* pbValue) const
{
	return GetMute(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, pbValue);
}

BOOL CMixer::SetLineInMute(BOOL bValue)
{
	return SetMute(MIXERLINE_COMPONENTTYPE_SRC_LINE, bValue);
}

BOOL CMixer::GetLineInMute(BOOL* pbValue) const
{
	return GetMute(MIXERLINE_COMPONENTTYPE_SRC_LINE, pbValue);
}

BOOL CMixer::SetMicrophoneMute(BOOL bValue)
{
	return SetMute(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, bValue);
}

BOOL CMixer::GetMicrophoneMute(BOOL* pbValue) const
{
	return GetMute(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, pbValue);
}

BOOL CMixer::SetMidiMute(BOOL bValue)
{
	return SetMute(MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER, bValue);
}

BOOL CMixer::GetMidiMute(BOOL* pbValue) const
{
	return GetMute(MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER, pbValue);
}

BOOL CMixer::SetWaveMute(BOOL bValue)
{
	return SetMute(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, bValue);
}

BOOL CMixer::GetWaveMute(BOOL* pbValue) const
{
	return GetMute(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, pbValue);
}

BOOL CMixer::SetLineInRecordVolume(DWORD dwLValue,DWORD dwRValue)
{
	return SetRecordVolume(MIXERLINE_COMPONENTTYPE_SRC_LINE,dwLValue, dwRValue);
}

BOOL CMixer::SetWaveInRecordVolume(DWORD dwLValue,DWORD dwRValue)
{
	return SetRecordVolume(MIXERLINE_COMPONENTTYPE_DST_WAVEIN,dwLValue,dwRValue);
}

BOOL CMixer::GetWaveInRecordVolume(DWORD* pdwLValue,DWORD* pdwRValue) const
{
	return GetRecordVolume(MIXERLINE_COMPONENTTYPE_DST_WAVEIN,pdwLValue,pdwRValue);
}

BOOL CMixer::GetLineInRecordVolume(DWORD* pdwLValue,DWORD* pdwRValue) const
{
	return GetRecordVolume(MIXERLINE_COMPONENTTYPE_SRC_LINE, pdwLValue, pdwRValue);
}

BOOL CMixer::SetMicrophoneRecordVolume(DWORD dwLValue)
{
	return SetRecordVolume(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, dwLValue, 0, TRUE);
}

BOOL CMixer::GetMicrophoneRecordVolume(DWORD* pdwLValue) const
{
	DWORD dwRValue;
	return GetRecordVolume(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE,pdwLValue, &dwRValue, TRUE);
}
