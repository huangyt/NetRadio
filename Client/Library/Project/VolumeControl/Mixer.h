/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : Mixer.h
/// \brief   : Mixer类头文件
/// \author  : letion
/// \version : 2.0
/// \date    : 2012-06-24
///============================================================================
#ifndef __MIXER_H__
#define __MIXER_H__

#include <Windows.h>
#include <mmsystem.h>

//=============================================================================
#pragma comment(lib,"winmm.lib")

//=============================================================================
class CMixer  
{
public:
	CMixer();
	virtual ~CMixer();

public:
	BOOL Close();
	UINT GetNumDevs() const;
	BOOL InitMixer(HWND hWnd);
	BOOL SetActiveDevice(UINT nSetDevID);
	BOOL GetDevCaps(LPMIXERCAPS pmxcaps) const;
	BOOL GetLineInfo(LPMIXERLINE pmxl, DWORD fdwInfo) const;
	BOOL GetLineInfo(LPMIXERLINE pmxl, DWORD dwDstType, DWORD dwSrcType) const;
	BOOL GetLineControls(LPMIXERLINECONTROLS pmxlc, DWORD fdwControls) const;
	BOOL GetLineControl(LPMIXERCONTROL pmxc, LPMIXERLINE pmxl, 	DWORD dwType) const;
	BOOL GetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails) const;
	BOOL SetControlDetails(LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
	BOOL SetVolume(DWORD dwSrcType, DWORD dwLValue, DWORD dwRValue, BOOL bMono = FALSE);
	BOOL GetVolume(DWORD dwSrcType, DWORD* pdwLValue, DWORD* pdwRValue, BOOL bMono = FALSE) const;
	BOOL SetMute(DWORD dwSrcType, BOOL bValue);
	BOOL GetMute(DWORD dwSrcType, BOOL* pbValue) const;

	BOOL SetCDVolume(DWORD dwLValue,DWORD dwRValue);				//set two channels volume of CD
	BOOL GetCDVolume(DWORD* pdwLValue,DWORD* pdwRValue) const;		//get two channels volume of CD
	BOOL SetSpeakerVolume(DWORD dwLValue,DWORD dwRValue);			//set two channels volume of Speaker
	BOOL GetSpeakerVolume(DWORD* pdwLValue,DWORD* pdwRValue) const;	//get two channels volume of Speaker
	BOOL SetLineInVolume(DWORD dwLValue,DWORD dwRValue);			//set two channels volume of Line-In
	BOOL GetLineInVolume(DWORD* pdwLValue,DWORD* pdwRValue) const;	//get two channels volume of Line-In
	BOOL SetMicrophoneVolume(DWORD dwLValue);						//set two channels volume of Microphone
	BOOL GetMicrophoneVolume(DWORD* pdwLValue) const;				//get two channels volume of Microphone
	BOOL SetMidiVolume(DWORD dwLValue,DWORD dwRValue);				//set two channels volume of Midi
	BOOL GetMidiVolume(DWORD* pdwLValue,DWORD* pdwRValue) const;	//get two channels volume of Midi
	BOOL SetWaveVolume(DWORD dwLValue,DWORD dwRValue);				//set two channels volume of Wave
	BOOL GetWaveVolume(DWORD* pdwLValue,DWORD* pdwRValue) const;	//get two channels volume of Wave

	BOOL GetCDMute(BOOL *bMute) const;				//get mute value of CD
	BOOL SetCDMute(BOOL bValue);					//set mute value of CD
	BOOL GetWaveMute(BOOL *bMute) const;			//get mute value of Wave
	BOOL SetWaveMute(BOOL bValue);					//set mute value of Wave
	BOOL GetMidiMute(BOOL *bMute) const;			//get mute value of Midi
	BOOL SetMidiMute(BOOL bValue);					//set mute value of Midi
	BOOL GetLineInMute(BOOL *bMute) const;			//get mute value of Line-In
	BOOL SetLineInMute(BOOL bValue);				//set mute value of Line-In
	BOOL GetSpeakerMute(BOOL *bMute) const;			//get mute value of Speaker
	BOOL SetSpeakerMute(BOOL bValue);				//set mute value of Speaker
	BOOL GetMicrophoneMute(BOOL *bMute) const;		//get mute value of Microphone
	BOOL SetMicrophoneMute(BOOL bValue);			//set mute value of Microphone

	BOOL SetRecordVolume(DWORD dwSrcType, DWORD dwLValue,DWORD dwRValue, BOOL bMono = FALSE);
	BOOL GetRecordVolume(DWORD dwSrcType, DWORD* pdwLValue,DWORD* pdwRValue, BOOL bMono = FALSE) const;
	BOOL SetWaveInRecordVolume(DWORD dwLValue,DWORD dwRValue);
	BOOL GetWaveInRecordVolume(DWORD* pdwLValue,DWORD* pdwRValue) const;
	BOOL SetLineInRecordVolume(DWORD dwLValue,DWORD dwRValue);
	BOOL GetLineInRecordVolume(DWORD* pdwLValue,DWORD* pdwRValue) const;
	BOOL SetMicrophoneRecordVolume(DWORD dwLValue);
	BOOL GetMicrophoneRecordVolume(DWORD* pdwLValue) const;

	HWND   SetWindow(HWND hWnd) { m_hWnd = hWnd; }
	HMIXER GetMixerHandle() {  return m_hmx;     }
	TCHAR   m_arrDevName[4][66] ;

private:
	UINT m_nCurMixId;
	UINT m_nNumMixers ;
	HWND m_hWnd;
	HMIXER m_hmx;
};

#endif //__MIXER_H__
