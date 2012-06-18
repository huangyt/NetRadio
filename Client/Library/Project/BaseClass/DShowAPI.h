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
/// \file    : DShowAPI.h
/// \brief   : dshow封装函数头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-06-17
///============================================================================
#ifndef __DSHOW_API_H__
#define __DSHOW_API_H__

#include <dshow.h>
#include <initguid.h>
#include "dshow\\streams.h"
#include "TypeDefine.h"
#include "DeviceDefine.h"

//=============================================================================
/// pin名称长度
#define MAX_PIN_NAME_SIZE		256

/// pin信息
typedef struct _pin_info
{
	WCHAR	 m_szPinName[MAX_PIN_NAME_SIZE];		///< 名称
	uint32_t m_nPinProperty;						///< 属性
}pin_info_t;

// 文件相关
//=============================================================================
/// \brief	Filter是否已注册
/// \param	inFilterId Filter CLSID
/// \return 成功-TRUE 失败-FALSE
BOOL IsFilterRegistered(CLSID FilterID);

/// \brief	注册Filter
/// \param	szFileName Filter文件路径和名称
/// \return 成功-TRUE 失败-FALSE
BOOL RegisterFilter(const WCHAR* szFileName);

/// \brief	注销Filter
/// \param	szFileName Filter文件路径和名称
/// \return 成功-TRUE 失败-FALSE
BOOL UnRegisterFilter(const WCHAR* szFileName);

// GraphBuilder相关
//=============================================================================
/// \brief	保存Graph到文件
/// \param  pGraphBuilder IGraphBuilder指针
/// \param  szPathName 文件名
/// \return HRESULT
HRESULT SaveGraphFile(IGraphBuilder* pGraphBuilder, const WCHAR* szPathName);

/// \brief	加载Graph到文件
/// \param  pGraphBuilder IGraphBuilder指针
/// \param  szPathName 文件名
/// \return HRESULT
HRESULT LoadGraphFile(IGraphBuilder* pGraphBuilder, const WCHAR* szPathName);

/// \brief	在GraphBuilder中查找指定Filter
/// \param  pGraphBuilder GraphBuilder指针
/// \param  MediaTypeID Filter类型
/// \param[OUT]  ppFilter Filter指针
/// \return HRESULT
HRESULT FindRenderer(IGraphBuilder* pGraphBuilder, const GUID* MediaTypeID, 
	IBaseFilter **ppFilter);

/// \brief	在GraphBuilder中查找音频播放Filter
/// \param  pGraphBuilder GraphBuilder指针
/// \param[OUT]  ppFilter Filter指针
/// \return HRESULT
HRESULT FindAudioRenderer(IGraphBuilder* pGraphBuilder, 
	IBaseFilter** ppFilter);

/// \brief	在GraphBuilder中查找视频播放Filter
/// \param  pGraphBuilder GraphBuilder指针
/// \param[OUT]  ppFilter Filter指针
/// \return HRESULT
HRESULT FindVideoRenderer(IGraphBuilder* pGraphBuilder, 
	IBaseFilter **ppFilter);

/// \brief	在GraphBuilder中查找FILTER
/// \param	pGraphBuilder GraphBuilder指针
/// \param	inClsid FilterID
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* FindFilterByCLSID(IGraphBuilder* pGraphBuilder, 
	REFCLSID inClsid);

/// \brief	在GraphBuilder中删除指定Filter之后的所有Filter
/// \param	pGraphBuilder GraphBuilder指针
/// \param	pFilter Filter指针
/// \return 无
void NukeDownstream(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter);

/// \brief	在GraphBuilder中删除指定Filter之前的所有Filter
/// \param	pGraphBuilder GraphBuilder指针
/// \param	pFilter Filter指针
/// \return 无
void NukeUpstream(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter);

// 设备相关
//=============================================================================
/// \brief	得到捕获设备列表
/// \param[IN]	CategoryID 设备种类GUID
/// \param[OUT]	pInfoArr 设备信息数组指针
/// \param		nInfoCount 设备信息数组数量
/// \return 设备数量
uint16_t QueryDeviceCategory(GUID CategoryID, device_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	得到视频设备列表
/// \param[OUT]	pInfoArr 设备信息数组指针
/// \param		nInfoCount 设备信息数组数量
/// \return 设备数量
uint16_t QueryVideoCaptureDevices(device_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	得到音频设备列表
/// \param[OUT]	pInfoArr 设备信息数组指针
/// \param		nInfoCount 设备信息数组数量
/// \return 设备数量
uint16_t QueryAudioCaptureDevices(device_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	得到Filter类型
/// \param	inFilter Filter指针
/// \return 捕获设备类型 
ENUM_DEVICE_TYPE DecideDeviceType(IBaseFilter* inFilter);

// DV: "Microsoft DV Camera and VCR"
/// \brief	创建视频设备Filter
/// \param	szFriendlyName Filter名称
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateVideoDeviceFilter(const WCHAR* szFriendlyName);

/// \brief	创建音频设备Filter
/// \param	szFriendlyName Filter名称
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateAudioDeviceFilter(const WCHAR* szFriendlyName);

// Create device according to the display name As a bonus, return the friendly name too.
/// \brief	创建视频设备Filter
/// \param	szDisplayName Filter显示名称
/// \param[OUT]	szFriendlyName Filter名称
/// \param	nFriendlyNameSize Filter名称长度
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateVideoDeviceFilter(const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize);

/// \brief	创建音频设备Filter
/// \param	szDisplayName Filter显示名称
/// \param[OUT]	szFriendlyName Filter名称
/// \param	nFriendlyNameSize Filter名称长度
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateAudioDeviceFilter(const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize);

/// \brief	创建视频编码器Filter
/// \param	bIsVideo 是否是视频设备
/// \param	szFilterName Filter名称
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateVideoCompressorFilter(const WCHAR* szFilterName);

/// \brief	创建音频编码器Filter
/// \param	bIsVideo 是否是视频设备
/// \param	szFilterName Filter名称
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateAudioCompressorFilter(const WCHAR* szFilterName);

/// \brief	创建硬件Filter
/// \param	CategoryID CategoryID
/// \param	szFriendlyName Filter名称
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateHardwareFilter(GUID CategoryID, 
	const WCHAR* szFriendlyName);

/// \brief	创建设备Filter
/// \param	inCategory 分类ID
/// \param	szDisplayName Filter显示名称
/// \param[OUT]	szFriendlyName Filter名称
/// \param	nFriendlyNameSize Filter名称长度
/// \return 成功-IBaseFilter指针 失败-NULL
IBaseFilter* CreateHardwareFilter(GUID CategoryID, const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize);

/// \brief	两个设备是否相同
/// \param	inDeviceName1 设备1名称
/// \param	inDeviceName2 设备2名称
/// \return 相同-TRUE 不同-FALSE
BOOL IsSameDevice(const WCHAR* szDeviceName1, const WCHAR* szDeviceName2);

// Filter相关
//=============================================================================
/// \brief	Filter是否支持属性页
/// \param  pFilter Filter指针
/// \return 支持-TRUE 不支持-FALSE
BOOL IsSupportPropertyPage(IBaseFilter* pFilter);

/// \brief	显示Filter属性页
/// \param  pFilter Filter指针
/// \param  hwndParent 父窗口句柄
/// \return HRESULT
HRESULT ShowFilterPropertyPage(IBaseFilter* pFilter, HWND hwndParent);

/// \brief	Filter是否完成连接
/// \param	pFilter Filter指针
/// \return 是-TRUE 否-FALSE
BOOL IsCompletelyConnected(IBaseFilter* pFilter);

/// \brief	得到上一个/下一个Filter
/// \param	pFilter 遍历开始Filter
/// \param	dir 遍历方向
/// \param[OUT]	ppNext 相邻的Filter
/// \return HRESULT
HRESULT GetNextFilter(IBaseFilter* pFilter, PIN_DIRECTION enPinType, 
	IBaseFilter** ppNext);

// pin相关
//=============================================================================
/// \brief	得到Filter中Pin的数量
/// \param	pFilter Filter指针
/// \param[OUT]	outInputPin 输入Pin数量
/// \param[OUT]	outOutputPin 输出Pin数量
/// \return 成功-TRUE 失败-FALSE
BOOL GetPinCount(IBaseFilter* pFilter, uint16_t* pInPinCount, 
	uint16_t* pOutPinCount);

/// \brief	得到Filter中输入Pin信息列表
/// \param	pFilter Filter指针
/// \param[OUT]	pInfoArr Pin信息数组指针
/// \param	nInfoCount Pin信息数组数量
/// \return pin数量
uint16_t GetInputPinInfo(IBaseFilter* pFilter, pin_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	得到Filter中输出Pin信息列表
/// \param	pFilter Filter指针
/// \param[OUT]	pInfoArr Pin信息数组指针
/// \param	nInfoCount Pin信息数组数量
/// \return pin数量
uint16_t GetOutputPinInfo(IBaseFilter* pFilter, pin_info_t* pInfoArr, 
	uint16_t nInfoCount);

/// \brief	得到Filter的输入Pin
/// \param	pFilter Filter指针
/// \param	szPinName Pin名称（默认NULL）
/// \return 成功-IPin指针 失败-NULL
IPin* GetInputPin(IBaseFilter* pFilter, const WCHAR* szPinName);

/// \brief	得到Filter中的输入接口指针
/// \param  pFilter Filter指针
/// \param  iNum 接口序号
/// \return 接口指针 
IPin* GetInputPin(IBaseFilter* pFilter, uint16_t nIndex);

/// \brief	得到Filter的输出Pin
/// \param	pFilter Filter指针
/// \param	szPinName Pin名称（默认NULL）
/// \return 成功-IPin指针 失败-NULL
IPin* GetOutputPin(IBaseFilter* pFilter, const WCHAR* szPinName);

/// \brief	得到Filter中的输出接口指针
/// \param  pFilter Filter指针
/// \param  iNum 接口序号
/// \return 接口指针 
IPin* GetOutputPin(IBaseFilter* pFilter, uint16_t nIndex);

/// \brief	得到Filter未连接的输入Pin
/// \param	pFilter Filter指针
/// \return 成功-IPin指针 失败-NULL
IPin* GetUnconnectInputPin(IBaseFilter* pFilter);

/// \brief	得到Filter未连接的输出Pin
/// \param	pFilter Filter指针
/// \return 成功-IPin指针 失败-NULL
IPin* GetUnconnectOutputPin(IBaseFilter* pFilter);

/// \brief	得到音频设备输入Pin名称
/// \param	inLineType 
///				line in    -> MIXERLINE_COMPONENTTYPE_SRC_LINE
///             microphone -> MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
///             CD Player  -> MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC
/// \param[OUT]	szPinName Pin名称
/// \param		nPinNameSize Pin名称长度
/// \return 成功-TRUE 失败-FALSE
BOOL GetAudioInputPinName(uint16_t nLineType, 
	WCHAR* szPinName, uint16_t nPinNameSize);

//其他函数
//=============================================================================
/// \brief	删除媒体类型
/// \param  pMediaType 媒体类型
/// \return 无
void UtilDeleteMediaType(AM_MEDIA_TYPE* pMediaType);

/// \brief	释放媒体类型
/// \param  MediaType 媒体类型
/// \return 无
void UtilFreeMediaType(AM_MEDIA_TYPE& MediaType);

/// \brief	得到媒体播放信息
/// \param  pMediaSeek IMediaSeeking指针
/// \param[OUT]  szDuration 信息
/// \param	nDurationSize 信息长度
/// \return HRESULT
HRESULT GetDurationString(IMediaSeeking* pMediaSeek, 
	WCHAR* szDuration, uint16_t nDurationSize);

#endif //__DSHOW_API_H__

