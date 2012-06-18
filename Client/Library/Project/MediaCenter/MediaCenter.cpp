#include "StdAfx.h"
#include "MediaCenter.h"

//=============================================================================
// 模块信息
module_info_t CMediaCenter::m_arrModuleInfo[MAX_MEDIA_MODULE_COUNT] = 
{
	// AudioCapture
	{ L"AudioCapture.dll",  ENUM_MODULE_DLL, INVALID_HANDLE_VALUE, NULL, NULL }, 
	// VideoCapture
	{ L"VideoCapture.dll",  ENUM_MODULE_DLL, INVALID_HANDLE_VALUE, NULL, NULL }, 
	// AudioEffect
	{ L"AudioEffect.dll",	ENUM_MODULE_DLL, INVALID_HANDLE_VALUE, NULL, NULL }, 
	// VideoEffect
	{ L"VideoEffect.dll",	ENUM_MODULE_DLL, INVALID_HANDLE_VALUE, NULL, NULL }, 
	// AudioCodec
	{ L"AudioCodec.dll",	ENUM_MODULE_DLL, INVALID_HANDLE_VALUE, NULL, NULL },
	// VideoCodec
	{ L"VideoCodec.dll",	ENUM_MODULE_DLL, INVALID_HANDLE_VALUE, NULL, NULL },
};

interface_info_t CMediaCenter::m_arrInterfaceInfo[MAX_MEDIA_INTERFACE_COUNT] = 
{
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

	IRESULT liResult = I_FAIL;
	TCHAR* szModuleName = NULL;
	void* pInterface = NULL;

	// 取得接口信息
	for(uint16_t nIndex=0; nIndex<MAX_MEDIA_INTERFACE_COUNT; ++nIndex)
	{
		if(IsEqualCLSID(m_arrInterfaceInfo[nIndex].m_InterfaceID, oInterfaceID))
		{
			szModuleName = m_arrInterfaceInfo[nIndex].m_szModuleName;
			pInterface = m_arrInterfaceInfo[nIndex].m_pInterface;
			break;
		}
	}

	// 判断接口是否已经创建
	if(NULL == pInterface)
	{
		// 创建接口
		if(NULL == szModuleName)
		{
			liResult = I_NOINTERFACE;
		}
		else
		{

		}
	}
	else
	{
		// 直接赋值
		*ppInterface = pInterface;

		// 增加计数

		liResult = I_SUCCEED;
	}

	if(I_SUCCEED == liResult)
	{

	}

	return liResult;
}

/// 释放接口
IRESULT CMediaCenter::ReleaseInterface(const CLSID& oInterfaceID, 
	void* pInterface)
{

}

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