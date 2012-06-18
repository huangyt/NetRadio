// MediaCenter.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "IMediaCenter.h"
#include "MediaCenter.h"

//=============================================================================
/// 创建媒体中心接口
IRESULT CreateInterface(const CLSID& oInterfaceID, void** ppInterface)
{
	IRESULT liResult = I_FAIL;
	if(IsEqualCLSID(CLSID_IMediaCenter, oInterfaceID))
	{
		*ppInterface = (IMediaCenter*)new CMediaCenter;
		liResult = I_SUCCEED;
	}
	else
	{
		liResult = I_NOINTERFACE;
	}
	return liResult;
}

/// 释放媒体中心接口
IRESULT DestroyInterface(const CLSID& oInterfaceID, void* pInterface)
{
	if(NULL == pInterface)
		return I_INVALIDARG;

	IRESULT liResult = I_FAIL;
	if(IsEqualCLSID(CLSID_IMediaCenter, oInterfaceID))
	{
		CMediaCenter* pMediaCenter = (CMediaCenter*)pInterface;
		delete pMediaCenter;
		pMediaCenter = NULL;
		liResult = I_SUCCEED;
	}
	else
	{
		liResult = I_NOINTERFACE;
	}

	return liResult;
}
