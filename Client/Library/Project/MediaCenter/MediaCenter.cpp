#include "StdAfx.h"
#include <TCHAR.h>
#include "MediaCenter.h"

//=============================================================================
// 模块信息
module_info_t CMediaCenter::m_arrModuleInfo[MAX_MEDIA_MODULE_COUNT] = 
{
	{ L"AudioCapture.dll",  NULL, 0 }, 
	{ L"AudioEffect.dll",	NULL, 0 }, 
	{ L"AudioCodec.dll",	NULL, 0 },
	{ L"AudioPlayer.dll",	NULL, 0 },
	{ L"AudioResample.dll",	NULL, 0 },
	{ L"AudioMix.dll",		NULL, 0 },
	{ L"VolumeControl.dll",	NULL, 0 },

	{ L"VideoCapture.dll",  NULL, 0 }, 
	{ L"VideoEffect.dll",	NULL, 0 }, 
	{ L"VideoCodec.dll",	NULL, 0 },
	{ L"VideoPlayer.dll",	NULL, 0 },
	{ L"VideoResize.dll",	NULL, 0 },
	{ L"ColorSpace.dll",	NULL, 0 },

};

interface_info_t CMediaCenter::m_arrInterfaceInfo[MAX_MEDIA_INTERFACE_COUNT] = 
{
	{ CLSID_IAudioCaputre,		L"AudioCapture.dll"		},
	//{ CLSID_IAudioEffect,		L"AudioEffect.dll"		},
	{ CLSID_IAudioEncoder,		L"AudioCodec.dll"		},
	{ CLSID_IAudioDecoder,		L"AudioCodec.dll"		},
	{ CLSID_IAudioPlayer,		L"AudioPlayer.dll"		},
	{ CLSID_IAudioResample,		L"AudioResample.dll"	},
	//{ CLSID_IAudioMix,			L"AudioMix.dll"			},
	{ CLSID_IVolumeControl,		L"VolumeControl.dll"	},

	{ CLSID_IVideoCapture,		L"VideoCapture.dll"		},
	//{ CLSID_IVideoEffect,		L"VideoEffect.dll"		},
	{ CLSID_IVideoEncoder,		L"VideoCodec.dll"		},
	{ CLSID_IVideoDecoder,		L"VideoCodec.dll"		},
	{ CLSID_IVideoPlayer,		L"VideoPlayer.dll"		},
	{ CLSID_IVideoResize,		L"VideoResize.dll"		},
	{ CLSID_IColorSpaceYV12,	L"ColorSpace.dll"		},	
};

//=============================================================================
CMediaCenter::CMediaCenter(void)
{
}


CMediaCenter::~CMediaCenter(void)
{
}

/// 查询接口
IRESULT CMediaCenter::QueryInterface(const CLSID& oInterfaceID, 
	void** ppInterface)
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	// 参数检查
	if(NULL == ppInterface)
		return I_INVALIDARG;

	// 检查接口是否支持
	if(!IsSupportInterface(oInterfaceID))
		return I_INVALIDARG;

	IRESULT nResult = I_NOINTERFACE;

	// 获得模块句柄
	HMODULE hModule = GetModuleHandle(oInterfaceID);
	if(NULL == hModule)
	{
		// 加载模块
		TCHAR* szModuleName = GetModuleName(oInterfaceID);
		if(NULL != szModuleName)
		{
			hModule = LoadLibrary(szModuleName);
		}
	}

	// 创建接口
	if(NULL != hModule)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			hModule, "CreateInterface");
		if(NULL != CreateInterface)
		{
			nResult = CreateInterface(oInterfaceID, ppInterface);
		}
	}

	if(ISUCCEEDED(nResult))
	{
		// 设置句柄
		SetModuleHandle(oInterfaceID, hModule);
		// 增加计数
		AddRefCount(hModule);
	}
	else
	{
		if(NULL != hModule)
		{
			FreeLibrary(hModule);
			hModule = NULL;
		}
	}
	return nResult;
}

/// 释放接口
IRESULT CMediaCenter::ReleaseInterface(const CLSID& oInterfaceID, 
	void* pInterface)
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	// 参数检查
	if(NULL == pInterface)
		return I_INVALIDARG;

	// 检查接口是否支持
	if(!IsSupportInterface(oInterfaceID))
		return I_INVALIDARG;

	IRESULT nResult = I_NOINTERFACE;

	// 获得模块句柄
	HMODULE hModule = GetModuleHandle(oInterfaceID);
	if(NULL != hModule)
	{
		typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
		DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
			hModule, "DestroyInterface");
		if(NULL != DestroyInterface)
		{
			nResult = DestroyInterface(oInterfaceID, pInterface);
		}
	}

	if(ISUCCEEDED(nResult))
	{
		// 减少计数
		uint32_t nRefCount = ReleaseRefCount(hModule);
		if(0 == nRefCount)
		{
			// 设置句柄
			SetModuleHandle(oInterfaceID, NULL);
			
			// 释放句柄
			FreeLibrary(hModule);
			hModule = NULL;
		}
	}
	return nResult;
}

//private======================================================================
/// 判断接口是否支持
BOOL CMediaCenter::IsSupportInterface(const CLSID& oInterfaceID) const
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	BOOL bResult = FALSE;
	for(uint16_t nIndex=0; nIndex<MAX_MEDIA_INTERFACE_COUNT; ++nIndex)
	{
		if(IsEqualCLSID(m_arrInterfaceInfo[nIndex].m_InterfaceID, oInterfaceID))
		{
			bResult = TRUE;
			break;
		}
	}
	return bResult;
}

/// 获得接口所属的模块名称
TCHAR* CMediaCenter::GetModuleName(const CLSID& oInterfaceID) const
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	TCHAR* szModuleName = NULL;
	for(uint16_t nIndex=0; nIndex<MAX_MEDIA_INTERFACE_COUNT; ++nIndex)
	{
		if(IsEqualCLSID(m_arrInterfaceInfo[nIndex].m_InterfaceID, oInterfaceID))
		{
			szModuleName = m_arrInterfaceInfo[nIndex].m_szModuleName;
			break;
		}
	}
	return szModuleName;
}

/// 获得接口所属模块句柄
HMODULE CMediaCenter::GetModuleHandle(const CLSID& oInterfaceID) const
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	HMODULE hModule = NULL;
	TCHAR* szModuleName = GetModuleName(oInterfaceID);

	if(NULL != szModuleName)
	{
		for(uint16_t nIndex=0; nIndex<MAX_MEDIA_MODULE_COUNT; ++nIndex)
		{
			const module_info_t& Info = m_arrModuleInfo[nIndex];
			if(_tcscmp(Info.m_szModuleName, szModuleName) == 0)
			{
				hModule = Info.m_hModuleHandle;
				break;
			}
		}
	}
	return hModule;
}

/// 设置接口所属模块句柄
BOOL CMediaCenter::SetModuleHandle(const CLSID& oInterfaceID, HMODULE hModule)
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	BOOL bResult = FALSE;
	TCHAR* szModuleName = GetModuleName(oInterfaceID);

	if(NULL != szModuleName)
	{
		for(uint16_t nIndex=0; nIndex<MAX_MEDIA_MODULE_COUNT; ++nIndex)
		{
			module_info_t& Info = m_arrModuleInfo[nIndex];
			if(_tcscmp(Info.m_szModuleName, szModuleName) == 0)
			{
				Info.m_hModuleHandle = hModule;
				bResult = TRUE;
				break;
			}
		}
	}
	return bResult;
}

/// 增加模块接口计数
uint32_t CMediaCenter::AddRefCount(HMODULE hModule)
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	uint32_t nRefCount = 0;

	for(uint16_t nIndex=0; nIndex<MAX_MEDIA_MODULE_COUNT; ++nIndex)
	{
		module_info_t& Info = m_arrModuleInfo[nIndex];
		if(Info.m_hModuleHandle == hModule)
		{
			++Info.nRefCount;
			nRefCount = Info.nRefCount;
			break;
		}
	}
	return nRefCount;
}

/// 减少模块接口计数
uint32_t CMediaCenter::ReleaseRefCount(HMODULE hModule)
{
	CCriticalAutoLock loAutoLock(m_ModuleLock);

	uint32_t nRefCount = 0;

	for(uint16_t nIndex=0; nIndex<MAX_MEDIA_MODULE_COUNT; ++nIndex)
	{
		module_info_t& Info = m_arrModuleInfo[nIndex];
		if(Info.m_hModuleHandle == hModule)
		{
			--Info.nRefCount;
			nRefCount = Info.nRefCount;
			break;
		}
	}
	return nRefCount;
}