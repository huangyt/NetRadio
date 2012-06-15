#include <atlbase.h>
#include "CaptureDevice.h"


CCaptureDevice::CCaptureDevice(void)
{
}


CCaptureDevice::~CCaptureDevice(void)
{
}

//=============================================================================
/// 获得设备数量
uint16_t CCaptureDevice::GetDeviceCount(ENUM_DEVICE_TYPE enDeviceType) const
{
	capture_device_info_t arrDeviceInfo[256];
	uint16_t nDeviceCount = GetDeviceList(enDeviceType, arrDeviceInfo, 256);
	return nDeviceCount;
}

/// 获得设备列表 
uint16_t CCaptureDevice::GetDeviceList(ENUM_DEVICE_TYPE enDeviceType, 
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	uint16_t nDeviceCount = 0;
	switch(enDeviceType)
	{
	case ENUM_DEVICE_UNKNOWN:
		break;
	case ENUM_DEVICE_AUDIO_INPUT:
		nDeviceCount = GetAudioInputList(pArrDeviceInfo, nArrCount);
		break;
	case ENUM_DEVICE_AUDIO_OUTPUT:
		nDeviceCount = GetAudioOutputList(pArrDeviceInfo, nArrCount);
		break;
	case ENUM_DEVIDE_VIDEO_INPUT:
		nDeviceCount = GetVideoInputList(pArrDeviceInfo, nArrCount);
		break;
	default:
		break;
	}
	return nDeviceCount;
}

/// 根据给定视频设备名称得到得到该设备的Filter
IBaseFilter* CCaptureDevice::GetDeviceFilter(ENUM_DEVICE_TYPE enDeviceType, 
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	IBaseFilter* pFilter = NULL;
	switch(enDeviceType)
	{
	case ENUM_DEVICE_UNKNOWN:
		break;
	case ENUM_DEVICE_AUDIO_INPUT:
		pFilter = GetAudioInputFilter(pszDeviceName, nNameSize);
		break;
	case ENUM_DEVICE_AUDIO_OUTPUT:
		pFilter = GetAudioOutputFilter(pszDeviceName, nNameSize);
		break;
	case ENUM_DEVIDE_VIDEO_INPUT:
		pFilter = GetVideoInputFilter(pszDeviceName, nNameSize);
		break;
	default:
		break;
	}
	return pFilter;
}

/// 得到选中Filter的设备类型
ENUM_VIDEO_DEVICE_TYPE CCaptureDevice::GetDeviceType(IBaseFilter* apFilter) const
{
	HRESULT hr = S_OK;
	IEnumPins* lpEnumPins = NULL ;
	IPin* lpPin = NULL; 
	IEnumMediaTypes* lpEnumMediaTypes = NULL;
	PIN_DIRECTION lPinDir;
	AM_MEDIA_TYPE* lpMediaType = NULL;
	ULONG lulFetched;

	hr = apFilter->EnumPins(&lpEnumPins);  
	if (hr != S_OK)
	{
		return ENUM_VIDEO_DEVICE_UNKNOWN;
	}

	lpEnumPins->Reset();
	hr = lpEnumPins->Next(1, &lpPin, &lulFetched);
	if (hr != S_OK)
	{
		lpEnumPins->Release();						//取pin信息失败要释放lpEnumPins对象
		return ENUM_VIDEO_DEVICE_VFW;
	}
	else
	{
		lpPin->Release();							//取pin信息成功要释放lpPin对象
	}

	lpEnumPins->Reset();
	while(S_OK == lpEnumPins->Next(1, &lpPin, &lulFetched))
	{
		//只查输出的类型，不查输入的类型
		hr = lpPin->QueryDirection(&lPinDir); 
		if (hr != S_OK || PINDIR_INPUT == lPinDir)
		{
			lpPin->Release(); 
			continue;
		}

		hr = lpPin->EnumMediaTypes(&lpEnumMediaTypes);
		if (hr == S_OK)
		{
			while (S_OK == lpEnumMediaTypes->Next(1, &lpMediaType, &lulFetched))   
			{
				if ((MEDIASUBTYPE_DVSD == lpMediaType->subtype) ||
					(MEDIASUBTYPE_dvsd == lpMediaType->subtype)) 
				{
					DeleteMediaType(lpMediaType);
					return ENUM_VIDEO_DEVICE_DV;
				}
				DeleteMediaType(lpMediaType);
			}
			lpEnumMediaTypes->Release();  
		}
		lpPin->Release(); 
	}
	lpEnumPins->Release();  
	return ENUM_VIDEO_DEVICE_WDM;
}

//=============================================================================
/// 获得音频输入设备列表
uint16_t CCaptureDevice::GetAudioInputList(
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//创建某类设备枚举器

	if(nArrCount <= 0)
	{
		return 0;
	}

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return 0;	
	}

	//为指定设备类型创建设备枚举器
	IEnumMoniker* lpEnumMoniker = NULL;			//枚举名称接口
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return 0;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IMoniker* lpMoniker = NULL;					//名称接口
	uint16_t nIndex = 0;

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		// 重置设备属性
		pArrDeviceInfo[nIndex].m_nDeviceProperty = 0;

		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				_tcscpy(pArrDeviceInfo[nIndex].m_szDeviceName, W2T(var.bstrVal));
				
				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
			nIndex++;
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return nIndex;
}

/// 获得音频输出设备列表
uint16_t CCaptureDevice::GetAudioOutputList(
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//创建某类设备枚举器

	if(nArrCount <= 0)
	{
		return 0;
	}

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return 0;	
	}

	//为指定设备类型创建设备枚举器
	IEnumMoniker* lpEnumMoniker = NULL;			//枚举名称接口
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioRendererCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return 0;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IMoniker* lpMoniker = NULL;					//名称接口
	uint16_t nIndex = 0;

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		// 重置设备属性
		pArrDeviceInfo[nIndex].m_nDeviceProperty = 0;

		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				_tcscpy(pArrDeviceInfo[nIndex].m_szDeviceName, W2T(var.bstrVal));

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
			nIndex++;
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return nIndex;
}

/// 获得视频输入设备列表
uint16_t CCaptureDevice::GetVideoInputList(
	capture_device_info_t* pArrDeviceInfo, uint16_t nArrCount) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//创建某类设备枚举器

	if(nArrCount <= 0)
	{
		return 0;
	}

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return 0;	
	}

	//为指定设备类型创建设备枚举器
	IEnumMoniker* lpEnumMoniker = NULL;			//枚举名称接口
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return 0;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IMoniker* lpMoniker = NULL;					//名称接口
	uint16_t nIndex = 0;

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		// 重置设备属性
		pArrDeviceInfo[nIndex].m_nDeviceProperty = 0;

		// 判断是否属于USB设备
		//WCHAR* wzDisplayName = NULL;
		//hr = lpMoniker->GetDisplayName(NULL,NULL,&wzDisplayName);
		//if(SUCCEEDED(hr))
		//{
		//	if(wcsstr(wzDisplayName, L"@device:pnp:\\\\?\\usb") != NULL)
		//	{
		//		pArrDeviceInfo[nIndex].m_bIsUsbDevice = TRUE;
		//	}
		//	CoTaskMemFree(wzDisplayName);
		//}

		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				_tcscpy(pArrDeviceInfo[nIndex].m_szDeviceName, W2T(var.bstrVal));

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
			nIndex++;
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return nIndex;
}

/// 获得音频输入设备Filter
IBaseFilter* CCaptureDevice::GetAudioInputFilter(
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//创建某类设备枚举器

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return NULL;	
	}

	//为指定设备类型创建设备枚举器
	IEnumMoniker* lpEnumMoniker = NULL;			//枚举名称接口
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return NULL;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	IMoniker* lpMoniker = NULL;					//名称接口

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				if(_tcsncmp(pszDeviceName, W2T(var.bstrVal), nNameSize) == 0)
				{
					lpMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter); 
				}

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return pFilter;
}

/// 获得音频输出设备Filter
IBaseFilter*  CCaptureDevice::GetAudioOutputFilter(
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//创建某类设备枚举器

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return NULL;	
	}

	//为指定设备类型创建设备枚举器
	IEnumMoniker* lpEnumMoniker = NULL;			//枚举名称接口
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_AudioRendererCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return NULL;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	IMoniker* lpMoniker = NULL;					//名称接口

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				if(_tcsncmp(pszDeviceName, W2T(var.bstrVal), nNameSize) == 0)
				{
					lpMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter); 
				}

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return pFilter;
}

/// 获得视频输入设备Filter
IBaseFilter*  CCaptureDevice::GetVideoInputFilter(
	TCHAR* pszDeviceName, uint16_t nNameSize) const
{
	HRESULT hr = S_OK;
	CComPtr<ICreateDevEnum> lpCreateDevEnum;	//创建某类设备枚举器

	hr = lpCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum); 
	if(!SUCCEEDED(hr))
	{
		return NULL;	
	}

	//为指定设备类型创建设备枚举器
	IEnumMoniker* lpEnumMoniker = NULL;			//枚举名称接口
	hr = lpCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
		&lpEnumMoniker, 0);
	if(!SUCCEEDED(hr))
	{
		return NULL;
	}

	lpEnumMoniker->Reset();

	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	IMoniker* lpMoniker = NULL;					//名称接口

	while(hr = lpEnumMoniker->Next(1, &lpMoniker, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag = NULL;
		hr = lpMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				if(_tcsncmp(pszDeviceName, W2T(var.bstrVal), nNameSize) == 0)
				{
					lpMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter); 
				}

				SysFreeString(var.bstrVal);
				lpMoniker->AddRef();
			}
			pBag->Release();
		}
		lpMoniker->Release();
	}
	lpEnumMoniker->Release();
	return pFilter;
}
