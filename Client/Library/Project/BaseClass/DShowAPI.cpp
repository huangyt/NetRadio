#include <string>
#include "DShowAPI.h"

#pragma warning(disable : 4996)

// 文件相关
//=============================================================================
// Filter是否已注册
BOOL IsFilterRegistered(CLSID FilterID)
{
	IBaseFilter * pFilter = NULL;
	if (SUCCEEDED(CoCreateInstance(FilterID, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void **)&pFilter)))
	{
		pFilter->Release();
		return TRUE;
	}
	return FALSE;
}

// 注册Filter
BOOL RegisterFilter(const WCHAR* szFileName)
{
	typedef (WINAPI * REGISTER_FUNC) (void);
	REGISTER_FUNC MyFunc = NULL;

	HMODULE hModule = ::LoadLibrary(szFileName);
	if (hModule)
	{
		MyFunc = (REGISTER_FUNC) GetProcAddress(hModule, "DllRegisterServer\r\n");
		BOOL pass = (MyFunc != NULL);
		if (pass)
		{
			MyFunc();
		}
		::FreeLibrary(hModule);
		return pass;
	}

	return FALSE;
}

// 注销Filter
BOOL UnRegisterFilter(const WCHAR* szFileName)
{
	typedef (WINAPI * REGISTER_FUNC) (void);
	REGISTER_FUNC   MyFunc = NULL;

	HMODULE hModule = ::LoadLibrary(szFileName);
	if (hModule)
	{
		MyFunc = (REGISTER_FUNC) GetProcAddress(hModule, "DllUnRegisterServer\r\n");
		BOOL pass = (MyFunc != NULL);
		if (pass)
		{
			MyFunc();
		}
		::FreeLibrary(hModule);
		return pass;
	}

	return FALSE;
}

//=============================================================================
//保存Graph到文件
HRESULT SaveGraphFile(IGraphBuilder* pGraphBuilder, const WCHAR* szPathName)
{
	const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 

	HRESULT hr;

	IStorage *pStorage = NULL;
	hr = StgCreateDocfile(szPathName,
		STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
		0, &pStorage);
	if(FAILED(hr)) 
	{
		return hr;
	}

	IStream *pStream;
	hr = pStorage->CreateStream(	wszStreamName,
		STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
		0, 0, &pStream);
	if (FAILED(hr)) 
	{
		pStorage->Release();    
		return hr;
	}

	IPersistStream *pPersist = NULL;
	pGraphBuilder->QueryInterface(IID_IPersistStream, 
		reinterpret_cast<void**>(&pPersist));
	hr = pPersist->Save(pStream, TRUE);
	pStream->Release();
	pPersist->Release();
	if (SUCCEEDED(hr)) 
	{
		hr = pStorage->Commit(STGC_DEFAULT);
	}
	pStorage->Release();
	return hr;
}

// 加载Graph到文件
HRESULT LoadGraphFile(IGraphBuilder* pGraphBuilder, const WCHAR* szPathName)
{
	IStorage *pStorage = 0;
	if (S_OK != StgIsStorageFile(szPathName))
	{
		return E_FAIL;
	}

	HRESULT hr = StgOpenStorage(szPathName, 0, 
		STGM_TRANSACTED | STGM_READ | STGM_SHARE_DENY_WRITE, 
		0, 0, &pStorage);
	if (FAILED(hr))
	{
		return hr;
	}

	IPersistStream *pPersistStream = 0;
	hr = pGraphBuilder->QueryInterface(IID_IPersistStream,
		reinterpret_cast<void**>(&pPersistStream));
	if (SUCCEEDED(hr))
	{
		IStream *pStream = 0;
		hr = pStorage->OpenStream(L"ActiveMovieGraph", 0, 
			STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
		if(SUCCEEDED(hr))
		{
			hr = pPersistStream->Load(pStream);
			pStream->Release();
		}
		pPersistStream->Release();
	}
	pStorage->Release();
	return hr;
}

// 在GraphBuilder中查找指定Filter
HRESULT FindRenderer(IGraphBuilder* pGraphBuilder, const GUID* MediaTypeID, 
	IBaseFilter **ppFilter)
{
	HRESULT hr;
	IEnumFilters *pEnum = NULL;
	IBaseFilter *pFilter = NULL;
	IPin *pPin;
	ULONG ulFetched;
	BOOL bFound=FALSE;

	// Verify graph builder interface
	if (!pGraphBuilder)
		return E_NOINTERFACE;

	// Verify that a media type was passed
	if (!MediaTypeID)
		return E_POINTER;

	// Clear the filter pointer in case there is no match
	if (ppFilter)
		*ppFilter = NULL;

	// Get filter enumerator
	hr = pGraphBuilder->EnumFilters(&pEnum);
	if (FAILED(hr))
		return hr;

	pEnum->Reset();

	// Enumerate all filters in the graph
	while(!bFound && (pEnum->Next(1, &pFilter, &ulFetched) == S_OK))
	{
#ifdef DEBUG
		// Read filter name for debugging purposes
		FILTER_INFO FilterInfo;
		WCHAR szName[256] = {0};

		hr = pFilter->QueryFilterInfo(&FilterInfo);
		if (SUCCEEDED(hr))
		{
			FilterInfo.pGraph->Release();
		}
#endif

		uint16_t nInPinCount, nOutPinCount;
		// Find a filter with one input and no output pins
		hr = GetPinCount(pFilter, &nInPinCount, &nOutPinCount);
		if (FAILED(hr))
			break;

		if ((nInPinCount == 1) && (nOutPinCount == 0))
		{
			// Get the first pin on the filter
			pPin=0;
			pPin = GetInputPin(pFilter, (uint16_t)0);

			// Read this pin's major media type
			AM_MEDIA_TYPE type={0};
			hr = pPin->ConnectionMediaType(&type);
			if (FAILED(hr))
				break;

			// Is this pin's media type the requested type?
			// If so, then this is the renderer for which we are searching.
			// Copy the interface pointer and return.
			if (type.majortype == *MediaTypeID)
			{
				// Found our filter
				*ppFilter = pFilter;
				bFound = TRUE;
			}
			// This is not the renderer, so release the interface.
			else
				pFilter->Release();

			// Delete memory allocated by ConnectionMediaType()
			UtilFreeMediaType(type);
		}
		else
		{
			// No match, so release the interface
			pFilter->Release();
		}
	}

	pEnum->Release();
	return hr;
}

// 在GraphBuilder中查找音频播放Filter
HRESULT FindAudioRenderer(IGraphBuilder* pGraphBuilder, 
	IBaseFilter** ppFilter)
{
	return FindRenderer(pGraphBuilder, &MEDIATYPE_Audio, ppFilter);
}

// 在GraphBuilder中查找视频播放Filter
HRESULT FindVideoRenderer(IGraphBuilder* pGraphBuilder, 
	IBaseFilter **ppFilter)
{
	return FindRenderer(pGraphBuilder, &MEDIATYPE_Video, ppFilter);
}

// 在GraphBuilder中查找FILTER
IBaseFilter* FindFilterByCLSID(IGraphBuilder* pGraphBuilder, 
	REFCLSID inClsid)
{
	IBaseFilter *  pReturnFilter = NULL;
	IEnumFilters * pEnumFilters = NULL;

	pGraphBuilder->EnumFilters(&pEnumFilters);
	if (pEnumFilters)
	{
		IBaseFilter *  pFilter  = NULL;
		IPersist *     pPersist = NULL;
		CLSID          clsid;
		ULONG fetchCount = 0;
		while (S_OK == pEnumFilters->Next(1, &pFilter, &fetchCount) 
			&& fetchCount && !pReturnFilter)
		{
			HRESULT hr = pFilter->QueryInterface(IID_IPersist, (void**)&pPersist);
			if (SUCCEEDED(hr))
			{
				pPersist->GetClassID(&clsid);
				pPersist->Release();
				if (IsEqualCLSID(clsid, inClsid))
				{
					pFilter->AddRef();
					pReturnFilter = pFilter;
				}
			}
			pFilter->Release();
		}
		pEnumFilters->Release();
	}

	return pReturnFilter;
}

// 在GraphBuilder中删除指定Filter之后的所有Filter
void NukeDownstream(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter)
{
	if (NULL == pGraphBuilder || NULL == pFilter)
		return ;

	IEnumPins * pinEnum = NULL;
	if (SUCCEEDED(pFilter->EnumPins(&pinEnum)))
	{
		pinEnum->Reset();

		IPin * pin     = NULL;
		ULONG  fetched = 0;
		while (SUCCEEDED(pinEnum->Next(1, &pin, &fetched)) && fetched)
		{
			if (pin)
			{
				IPin * connectedPin = NULL;
				pin->ConnectedTo(&connectedPin);
				if (connectedPin) 
				{
					PIN_INFO pininfo;
					if (SUCCEEDED(connectedPin->QueryPinInfo(&pininfo)))
					{
						pininfo.pFilter->Release();
						if (pininfo.dir == PINDIR_INPUT) 
						{
							NukeDownstream(pGraphBuilder, pininfo.pFilter);
							pGraphBuilder->Disconnect(connectedPin);
							pGraphBuilder->Disconnect(pin);
							pGraphBuilder->RemoveFilter(pininfo.pFilter);
						}
					}
					connectedPin->Release();
				}
				pin->Release();
			}
		}
		pinEnum->Release();
	}
}

// 在GraphBuilder中删除指定Filter之前的所有Filter
void NukeUpstream(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter)
{
	if (NULL == pGraphBuilder || NULL == pFilter)
		return ;

	IEnumPins * pinEnum = NULL;
	if (SUCCEEDED(pFilter->EnumPins(&pinEnum)))
	{
		pinEnum->Reset();

		IPin * pin     = NULL;
		ULONG  fetched = 0;
		BOOL   pass    = TRUE;
		while (pass && SUCCEEDED(pinEnum->Next(1, &pin, &fetched)) && fetched)
		{
			if (pin)
			{
				IPin * connectedPin = NULL;
				pin->ConnectedTo(&connectedPin);
				if(connectedPin) 
				{
					PIN_INFO pininfo;
					if (SUCCEEDED(connectedPin->QueryPinInfo(&pininfo)))
					{
						if(pininfo.dir == PINDIR_OUTPUT) 
						{
							NukeUpstream(pGraphBuilder, pininfo.pFilter);
							pGraphBuilder->Disconnect(connectedPin);
							pGraphBuilder->Disconnect(pin);
							pGraphBuilder->RemoveFilter(pininfo.pFilter);
						}
						pininfo.pFilter->Release();
					}
					connectedPin->Release();
				}
				pin->Release();
			}
			else
			{
				pass = FALSE;
			}
		}
		pinEnum->Release();
	}
}

// 设备相关
//=============================================================================
// 得到捕获设备列表
uint16_t QueryDeviceCategory(GUID CategoryID, device_info_t* pInfoArr, 
	uint16_t nInfoCount)
{
	if(NULL == pInfoArr || 0 == nInfoCount)
		return 0;

	uint16_t nDeviceCount = 0;
	HRESULT hr = NOERROR;
	ICreateDevEnum * enumHardware = NULL;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_ALL,
		IID_ICreateDevEnum, (void**)&enumHardware);
	if (SUCCEEDED(hr))
	{
		IEnumMoniker*  enumMoniker = NULL;
		hr = enumHardware->CreateClassEnumerator(CategoryID, &enumMoniker, 0);
		if (enumMoniker)
		{
			enumMoniker->Reset();

			ULONG fetched = 0;
			IMoniker * moniker = NULL;

			while(SUCCEEDED(enumMoniker->Next(1, &moniker, &fetched)) && fetched)
			{
				if(nDeviceCount >= nInfoCount)
					break;

				if (moniker)
				{
					WCHAR * wzDisplayName      = NULL;
					IPropertyBag * propertyBag = NULL;
					IBaseFilter *  filter      = NULL;
					VARIANT        name;

					// Get display name
					hr = moniker->GetDisplayName(NULL, NULL, &wzDisplayName);
					if (SUCCEEDED(hr))
					{					
						wcsncpy(pInfoArr[nDeviceCount].m_szDisplayName, wzDisplayName, 
							MAX_DEVICE_NAME_SIZE);

						CoTaskMemFree(wzDisplayName);
						hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag);
					}	

					if (SUCCEEDED(hr))
					{
						name.vt = VT_BSTR;
						// Get friendly name
						hr = propertyBag->Read(L"FriendlyName", &name, NULL);

					}

					if (SUCCEEDED(hr))
					{
						wcsncpy(pInfoArr[nDeviceCount].m_szDeviceName, name.bstrVal, 
							MAX_DEVICE_NAME_SIZE);

						SysFreeString(name.bstrVal);
						hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&filter);
					}

					if (SUCCEEDED(hr))
					{
						++nDeviceCount;
					}

					// Release interfaces
					if (filter)
					{
						filter->Release();
						filter = NULL;
					}

					if (propertyBag)
					{
						propertyBag->Release();
						propertyBag = NULL;
					}
					moniker->Release();
				}
			}
			enumMoniker->Release();
		}
		enumHardware->Release();
	}

	return nDeviceCount;
}

// 得到视频设备列表
uint16_t QueryVideoCaptureDevices(device_info_t* pInfoArr, 
	uint16_t nInfoCount)
{
	return QueryDeviceCategory(CLSID_VideoInputDeviceCategory, 
		pInfoArr, nInfoCount);
}

// 得到音频设备列表
uint16_t QueryAudioCaptureDevices(device_info_t* pInfoArr, 
	uint16_t nInfoCount)
{
	return QueryDeviceCategory(CLSID_AudioInputDeviceCategory, 
		pInfoArr, nInfoCount);
}

// 得到Filter类型
ENUM_DEVICE_TYPE DecideDeviceType(IBaseFilter* inFilter)
{
	// Check if DV device
	IAMExtTransport * pAMExtTransPost = NULL;
	inFilter->QueryInterface(IID_IAMExtTransport, (void **)&pAMExtTransPost);
	if (pAMExtTransPost)
	{
		pAMExtTransPost->Release();
		return ENUM_DEVICE_DV;
	}

	// Check if WDM analog
	IAMAnalogVideoDecoder * pDecoder = NULL;
	inFilter->QueryInterface(IID_IAMAnalogVideoDecoder, (void **)&pDecoder);
	if (pDecoder)
	{
		pDecoder->Release();
		return ENUM_DEVICE_WDM;
	}

	// Check if VFW analog
	IAMVfwCaptureDialogs * pVfwDlgs = NULL;
	inFilter->QueryInterface(IID_IAMVfwCaptureDialogs, (void **)&pVfwDlgs);
	if (pVfwDlgs)
	{
		pVfwDlgs->Release();
		return ENUM_DEVICE_VFW;
	}

	// Check if audio capture device
	IAMAudioInputMixer * pAudioMixer = NULL;
	inFilter->QueryInterface(IID_IAMAudioInputMixer, (void **)&pAudioMixer);
	if (pAudioMixer)
	{
		pAudioMixer->Release();
		return ENUM_DEVICE_AUDIO;
	}

	return ENUM_DEVICE_UNKNOWN;
}

// 创建视频设备Filter
IBaseFilter* CreateVideoDeviceFilter(const WCHAR* szFriendlyName)
{
	return CreateHardwareFilter(CLSID_VideoInputDeviceCategory, szFriendlyName);
}

// 创建音频设备Filter
IBaseFilter* CreateAudioDeviceFilter(const WCHAR* szFriendlyName)
{
	return CreateHardwareFilter(CLSID_AudioInputDeviceCategory, szFriendlyName);
}

// 创建视频设备Filter
IBaseFilter* CreateVideoDeviceFilter(const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize)
{
	return CreateHardwareFilter(CLSID_VideoInputDeviceCategory, szDisplayName, 
		szFriendlyName, nFriendlyNameSize);
}

// 创建音频设备Filter
IBaseFilter* CreateAudioDeviceFilter(const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize)
{
	return CreateHardwareFilter(CLSID_AudioInputDeviceCategory, szDisplayName, 
		szFriendlyName, nFriendlyNameSize);
}

// 创建视频编码器Filter
IBaseFilter* CreateVideoCompressorFilter(const WCHAR* szFilterName)
{
	return CreateHardwareFilter(CLSID_VideoCompressorCategory, szFilterName);
}

// 创建音频编码器Filter
IBaseFilter* CreateAudioCompressorFilter(const WCHAR* szFilterName)
{
	return CreateHardwareFilter(CLSID_AudioCompressorCategory, szFilterName);
}

// 创建硬件Filter
IBaseFilter* CreateHardwareFilter(GUID CategoryID, 
	const WCHAR* szFriendlyName)
{
	ICreateDevEnum * enumHardware = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_ALL,
		IID_ICreateDevEnum, (void**)&enumHardware);
	if (FAILED(hr))
	{
		return NULL;
	}

	IBaseFilter * hardwareFilter = NULL;
	IEnumMoniker * enumMoniker   = NULL;
	hr = enumHardware->CreateClassEnumerator(CategoryID, &enumMoniker, 0);
	if (enumMoniker)
	{
		enumMoniker->Reset();

		ULONG      fetched = 0;
		IMoniker * moniker = NULL;
		char friendlyName[256];

		while (!hardwareFilter && SUCCEEDED(enumMoniker->Next(1, &moniker, &fetched)) && fetched)
		{
			if (moniker)
			{
				IPropertyBag * propertyBag = NULL;
				VARIANT        name;
				friendlyName[0] = 0;
				hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag);
				if (SUCCEEDED(hr))
				{					
					name.vt = VT_BSTR;
					hr = propertyBag->Read(L"FriendlyName", &name, NULL);
				}

				if (SUCCEEDED(hr))
				{
					if (wcscmp(name.bstrVal, szFriendlyName) == 0) 
					{
						moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&hardwareFilter);
					}
					SysFreeString(name.bstrVal);
				}

				// Release interfaces
				if (propertyBag)
				{
					propertyBag->Release();
					propertyBag = NULL;
				}
				moniker->Release();
			}			
		}
		enumMoniker->Release();
	}
	enumHardware->Release();

	return hardwareFilter;
}

// 创建设备Filter
IBaseFilter* CreateHardwareFilter(GUID CategoryID, const WCHAR* szDisplayName, 
	WCHAR* szFriendlyName, uint16_t nFriendlyNameSize)
{
	ICreateDevEnum * enumHardware = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_ALL,
		IID_ICreateDevEnum, (void**)&enumHardware);
	if (FAILED(hr))
	{
		return NULL;
	}

	IBaseFilter * hardwareFilter = NULL;
	IEnumMoniker * enumMoniker   = NULL;
	hr = enumHardware->CreateClassEnumerator(CategoryID, &enumMoniker, 0);
	if (enumMoniker)
	{
		enumMoniker->Reset();

		ULONG      fetched = 0;
		IMoniker * moniker = NULL;

		while (!hardwareFilter && SUCCEEDED(enumMoniker->Next(1, &moniker, &fetched)) && fetched)
		{
			if (moniker)
			{
				WCHAR* pDisplayName = NULL;
				// Get display name
				hr = moniker->GetDisplayName(NULL, NULL, &pDisplayName);
				if (SUCCEEDED(hr))
				{					
					CoTaskMemFree(pDisplayName);

					if (IsSameDevice(pDisplayName, szDisplayName)) 
					{
						moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&hardwareFilter);
					}
				}

				// Get the friendly name
				if (szFriendlyName && hardwareFilter)
				{
					IPropertyBag * propertyBag = NULL;
					if (SUCCEEDED(moniker->BindToStorage(0, 0, IID_IPropertyBag, 
						(void **)&propertyBag)))
					{
						VARIANT name;
						name.vt = VT_BSTR;
						if (SUCCEEDED(propertyBag->Read(L"FriendlyName", &name, NULL)))
						{
							wcsncpy(szFriendlyName, name.bstrVal, nFriendlyNameSize);
							SysFreeString(name.bstrVal);
						}
						propertyBag->Release();
					}
				}
				moniker->Release();
			}			
		}
		enumMoniker->Release();
	}
	enumHardware->Release();

	return hardwareFilter;
}

// 两个设备是否相同
BOOL IsSameDevice(const WCHAR* szDeviceName1, const WCHAR* szDeviceName2)
{
	std::wstring temp1, temp2, deviceName1, deviceName2;

	temp1 = szDeviceName1;
	temp2 = szDeviceName2;

	int backSlash1 = temp1.find(L"\\\\", 0);
	int backSlash2 = temp2.find(L"\\\\", 0);

	if (backSlash1 >= 0 && backSlash2 >= 0)
	{
		deviceName1 = temp1.substr(temp1.length() - backSlash1);
		deviceName2 = temp2.substr(temp2.length() - backSlash2);

		return (_wcsicmp(deviceName1.c_str(), deviceName2.c_str()) == 0);
	}
	else
	{
		return (_wcsicmp(temp1.c_str(), temp2.c_str()) == 0);
	}
}

// Filter 相关
//=============================================================================
BOOL IsSupportPropertyPage(IBaseFilter* pFilter)
{
    HRESULT hr;
    ISpecifyPropertyPages *pSpecify;

    // Discover if this filter contains a property page
    hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
    if (SUCCEEDED(hr)) 
    {
        pSpecify->Release();
        return TRUE;
    }
    return FALSE;
}

// 显示Filter属性页
HRESULT ShowFilterPropertyPage(IBaseFilter* pFilter, HWND hwndParent)
{
    HRESULT hr;
    ISpecifyPropertyPages *pSpecify=0;

    if (!pFilter)
        return E_NOINTERFACE;

    // Discover if this filter contains a property page
    hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
    if (SUCCEEDED(hr)) 
    {
        do 
        {
            FILTER_INFO FilterInfo;
            hr = pFilter->QueryFilterInfo(&FilterInfo);
            if (FAILED(hr))
                break;

            CAUUID caGUID;
            hr = pSpecify->GetPages(&caGUID);
            if (FAILED(hr))
                break;
      
            // Display the filter's property page
            OleCreatePropertyFrame(
                hwndParent,             // Parent window
                0,                      // x (Reserved)
                0,                      // y (Reserved)
                FilterInfo.achName,     // Caption for the dialog box
                1,                      // Number of filters
                (IUnknown **)&pFilter,  // Pointer to the filter 
                caGUID.cElems,          // Number of property pages
                caGUID.pElems,          // Pointer to property page CLSIDs
                0,                      // Locale identifier
                0,                      // Reserved
                NULL                    // Reserved
            );

            CoTaskMemFree(caGUID.pElems);
            FilterInfo.pGraph->Release(); 

        } while(0);
    }

    // Release interfaces
    if (pSpecify)
        pSpecify->Release();
        
    pFilter->Release();
    return hr;
}


// Filter是否完成连接
BOOL IsCompletelyConnected(IBaseFilter* pFilter)
{
	if (pFilter == NULL)
		return FALSE;

	BOOL allConnected   = FALSE;
	IEnumPins * pinEnum = NULL;
	if (SUCCEEDED(pFilter->EnumPins(&pinEnum)))
	{
		pinEnum->Reset();

		IPin * pin = NULL;
		ULONG fetchCount = 0;
		while (SUCCEEDED(pinEnum->Next(1, &pin, &fetchCount)) && 
			fetchCount && allConnected)
		{
			if (pin)
			{
				pin->Release();

				IPin * connected = NULL;
				pin->ConnectedTo(&connected);
				if (connected)
				{
					connected->Release();
                    allConnected = TRUE;
				}
			}
		}
		pinEnum->Release();
	}
	return allConnected;
}

// 得到上一个/下一个Filter
HRESULT GetNextFilter(IBaseFilter* pFilter, PIN_DIRECTION enPinType, 
	IBaseFilter** ppNext)
{
    if(NULL == pFilter || NULL == ppNext)
        return E_POINTER;

    IEnumPins* pEnum = NULL;
    IPin* pPin = NULL;
    HRESULT hr = pFilter->EnumPins(&pEnum);         // 枚举Filter上所有的Pin
    if(FAILED(hr))
        return hr;

    while(S_OK == pEnum->Next(1, &pPin, 0))
    {
        PIN_DIRECTION ThisPinDir;
        hr = pPin->QueryDirection(&ThisPinDir);
        if(FAILED(hr))
        {
            hr = E_UNEXPECTED;
            pPin->Release();
            break;
        }
        if(ThisPinDir == enPinType)
        {
            IPin* pPinNext = NULL;
            hr = pPin->ConnectedTo(&pPinNext);
            if(SUCCEEDED(hr))
            {
                PIN_INFO PinInfo;
                hr = pPinNext->QueryPinInfo(&PinInfo);
                pPinNext->Release();
                pPin->Release();
                pEnum->Release();
                if(FAILED(hr) || (PinInfo.pFilter == NULL))
                    return E_UNEXPECTED;

                *ppNext = PinInfo.pFilter;      // Client Must Release
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    return E_FAIL;
}

// pin相关
//=============================================================================
// 得到Filter中Pin的数量
BOOL GetPinCount(IBaseFilter* pFilter, uint16_t* pInPinCount, 
	uint16_t* pOutPinCount)
{
    HRESULT hr = S_OK;
    IEnumPins *pEnum=0;
    ULONG ulFound;
    IPin *pPin;

    // Verify input
    if (!pFilter || !pInPinCount || !pOutPinCount)
        return E_POINTER;

    // Clear number of pins found
    *pInPinCount = 0;
    *pOutPinCount = 0;

    // Get pin enumerator
    hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    pEnum->Reset();

    // Count every pin on the filter
    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION) 3;

        hr = pPin->QueryDirection(&pindir);

        if(pindir == PINDIR_INPUT)
            (*pInPinCount)++;
        else
            (*pOutPinCount)++;

        pPin->Release();
    } 

    pEnum->Release();
    return hr;
}

// 得到Filter中输入Pin信息列表
uint16_t GetInputPinInfo(IBaseFilter* pFilter, pin_info_t* pInfoArr, 
	uint16_t nInfoCount)
{
    if(NULL == pInfoArr || 0 == nInfoCount)
        return 0;

    uint16_t nPinCount = 0;
	if (pFilter)
	{
		IEnumPins * pinEnum = NULL;
		if (SUCCEEDED(pFilter->EnumPins(&pinEnum)))
		{
			pinEnum->Reset();

			BOOL pass  = TRUE;
			IPin * pin = NULL;
			ULONG fetchCount = 0;
            UINT nPinIndex = 0;
			while (pass && SUCCEEDED(pinEnum->Next(1, &pin, &fetchCount)) && fetchCount)
			{
				if (pin)
				{
					PIN_INFO pinInfo;
					if (SUCCEEDED(pin->QueryPinInfo(&pinInfo)))
					{
						pinInfo.pFilter->Release();

						if (pinInfo.dir == PINDIR_INPUT)
						{
							wcsncpy(pInfoArr[nPinCount].m_szPinName, pinInfo.achName, 
								MAX_PIN_NAME_SIZE);
							nPinCount++;
						}
					}
					pin->Release();
					pin = NULL;
				}
				else
				{
					pass = FALSE;
				}
			}
			pinEnum->Release();
		}
	}
    return nPinCount;
}

// 得到Filter中输出Pin信息列表
uint16_t GetOutputPinInfo(IBaseFilter* pFilter, pin_info_t* pInfoArr, 
	uint16_t nInfoCount)
{
    if(NULL == pInfoArr || 0 == nInfoCount)
        return 0;

    uint16_t nPinCount = 0;
	if (pFilter)
	{
		IEnumPins * pinEnum = NULL;
		if (SUCCEEDED(pFilter->EnumPins(&pinEnum)))
		{
			pinEnum->Reset();

			BOOL pass  = TRUE;
			IPin * pin = NULL;
			ULONG fetchCount = 0;
            UINT nPinIndex = 0;
			while (pass && SUCCEEDED(pinEnum->Next(1, &pin, &fetchCount)) && fetchCount)
			{
				if (pin)
				{
					PIN_INFO pinInfo;
					if (SUCCEEDED(pin->QueryPinInfo(&pinInfo)))
					{
						pinInfo.pFilter->Release();

						if (pinInfo.dir == PINDIR_OUTPUT)
						{
							wcsncpy(pInfoArr[nPinCount].m_szPinName, pinInfo.achName, 
								MAX_PIN_NAME_SIZE);
							nPinCount++;
						}
					}
					pin->Release();
					pin = NULL;
				}
				else
				{
					pass = FALSE;
				}
			}
			pinEnum->Release();
		}
	}
    return nPinCount;
}

// 得到Filter中的接口指针
static IPin* GetPin(IBaseFilter* pFilter, PIN_DIRECTION enPinType, 
	uint16_t nIndex)
{
	if(NULL == pFilter)
		return NULL;

	IPin* foundPin = NULL;
    IEnumPins* pinEnum = NULL;
    HRESULT hr = pFilter->EnumPins(&pinEnum);
    if(SUCCEEDED(hr)) 
	{
		ULONG ulFound;
		IPin *pPin;
		hr = E_FAIL;

		while(S_OK == pinEnum->Next(1, &pPin, &ulFound))
		{
			PIN_DIRECTION pindir = (PIN_DIRECTION)3;
			pPin->QueryDirection(&pindir);
			if(pindir == enPinType)
			{
				if(nIndex == 0)
				{
					pPin->AddRef();
					foundPin = pPin;  // Return the pin's interface
					break;
				}
				nIndex--;
			} 
			pPin->Release();
		}
		pinEnum->Release();
	}

	// We don't keep outstanding reference count
	if (foundPin)
	{
		foundPin->Release();
	}
    return foundPin;
}

// 得到Filter的Pin
static IPin* GetPin(IBaseFilter* pFilter, PIN_DIRECTION pinDir, 
	const WCHAR* szPinName)
{
	IPin* foundPin = NULL;

	if(pFilter)
	{
		IEnumPins* pinEnum = NULL;
		if(SUCCEEDED(pFilter->EnumPins(&pinEnum)))
		{
			pinEnum->Reset();

			IPin * pin = NULL;
			ULONG fetchCount = 0;
			while (!foundPin && SUCCEEDED(pinEnum->Next(1, &pin, &fetchCount)) && fetchCount)
			{
				if (pin)
				{
					PIN_INFO pinInfo;
					if (SUCCEEDED(pin->QueryPinInfo(&pinInfo)))
					{
						if (pinInfo.dir == pinDir)
						{
							// Ignore the pin name
							if (NULL == szPinName)
							{
								pin->AddRef();
								foundPin = pin;
							}
							else
							{
								if (NULL != wcsstr(pinInfo.achName, szPinName))
								{
									pin->AddRef();
									foundPin = pin;
								}
							}
						}
						pinInfo.pFilter->Release();
					}
					pin->Release();
				}
			}
			pinEnum->Release();
		}
	}

	// We don't keep outstanding reference count
	if (foundPin)
	{
		foundPin->Release();
	}
	return foundPin;
}

// 得到Filter的输入Pin
IPin* GetInputPin(IBaseFilter* pFilter, const WCHAR* szPinName)
{
	return GetPin(pFilter, PINDIR_INPUT, szPinName);
}

// 得到Filter中的输入接口指针
IPin* GetInputPin(IBaseFilter* pFilter, uint16_t nIndex)
{
	return GetPin(pFilter, PINDIR_INPUT, nIndex);
}

// 得到Filter的输出Pin
IPin* GetOutputPin(IBaseFilter* pFilter, const WCHAR* szPinName)
{
	return GetPin(pFilter, PINDIR_OUTPUT, szPinName);
}

// 得到Filter中的输出接口指针
IPin* GetOutputPin(IBaseFilter* pFilter, uint16_t nIndex)
{
	return GetPin(pFilter, PINDIR_OUTPUT, nIndex);
}

// 得到Filter未连接的Pin
static IPin* GetUnconnectPin(IBaseFilter* pFilter, PIN_DIRECTION pinDir)
{
	IPin* foundPin = NULL;

	if (pFilter)
	{
		IEnumPins* pinEnum = NULL;
		if (SUCCEEDED(pFilter->EnumPins(&pinEnum)))
		{
			pinEnum->Reset();

			IPin* pin = NULL;
			ULONG fetchCount = 0;
			while (!foundPin && SUCCEEDED(pinEnum->Next(1, &pin, &fetchCount)) && fetchCount)
			{
				if (pin)
				{
					PIN_INFO pinInfo;
					if (SUCCEEDED(pin->QueryPinInfo(&pinInfo)))
					{
						pinInfo.pFilter->Release();
						if (pinInfo.dir == pinDir)
						{
							IPin* connected = NULL;
							pin->ConnectedTo(&connected);
							if (connected)
							{
								connected->Release();
							}
							else
							{
								pin->AddRef();
								foundPin = pin;
							}
						}						
					}
					pin->Release();
				}
			}
			pinEnum->Release();
		}
	}

	// We don't keep outstanding reference count
	if (foundPin)
	{
		foundPin->Release();
	}
	return foundPin;
}

// 得到Filter未连接的输入Pin
IPin* GetUnconnectInputPin(IBaseFilter* pFilter)
{
	return GetUnconnectPin(pFilter, PINDIR_INPUT);
}

// 得到Filter未连接的输出Pin
IPin* GetUnconnectOutputPin(IBaseFilter* pFilter)
{
	return GetUnconnectPin(pFilter, PINDIR_OUTPUT);
}

// 得到音频设备输入Pin名称
BOOL GetAudioInputPinName(uint16_t nLineType, 
	WCHAR* szPinName, uint16_t nPinNameSize)
{
	if(NULL == szPinName || 0 == nPinNameSize)
		return FALSE;

    DWORD ComponentType = nLineType;  
    HMIXER hMixer;   
    MIXERLINE mxl;   
    HRESULT hr;   
    hr = mixerOpen(&hMixer, 0, 0, 0, 0);   
    if(FAILED(hr))
        return FALSE;

    mxl.cbStruct = sizeof(mxl);   
    mxl.dwComponentType = ComponentType;   
    hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);   

	if(SUCCEEDED(hr))
	{
		wcsncpy(szPinName, mxl.szName, nPinNameSize);
		return TRUE;
	}
	return FALSE;
}

//其他函数
//=============================================================================
// 删除媒体类型
void UtilDeleteMediaType(AM_MEDIA_TYPE* pMediaType)
{
    // Allow NULL pointers for coding simplicity
    if (pMediaType == NULL) {
        return;
    }

    // Free media type's format data
    if (pMediaType->cbFormat != 0) 
    {
        CoTaskMemFree((PVOID)pMediaType->pbFormat);

        // Strictly unnecessary but tidier
        pMediaType->cbFormat = 0;
        pMediaType->pbFormat = NULL;
    }

    // Release interface
    if (pMediaType->pUnk != NULL) 
    {
        pMediaType->pUnk->Release();
        pMediaType->pUnk = NULL;
    }

    // Free media type
    CoTaskMemFree((PVOID)pMediaType);
}

// 释放媒体类型
void UtilFreeMediaType(AM_MEDIA_TYPE& MediaType)
{
    if(MediaType.cbFormat != 0)
    {
        CoTaskMemFree((PVOID)MediaType.pbFormat);

        // Strictly unnecessary but tidier
        MediaType.cbFormat = 0;
        MediaType.pbFormat = NULL;
    }
    if(MediaType.pUnk != NULL)
    {
        MediaType.pUnk->Release();
        MediaType.pUnk = NULL;
    }
}

// 得到媒体播放信息
HRESULT GetDurationString(IMediaSeeking* pMediaSeek, 
	WCHAR* szDuration, uint16_t nDurationSize)
{
    HRESULT hr;

    if (!pMediaSeek)
        return E_NOINTERFACE;
    if (!szDuration)
        return E_POINTER;

    // Initialize the display in case we can't read the duration
    wcscpy(szDuration, L"<00:00.000>\0");

    // Is media time supported for this file?
    if (S_OK != pMediaSeek->IsFormatSupported(&TIME_FORMAT_MEDIA_TIME))
        return E_NOINTERFACE;

    // Read the time format to restore later
    GUID guidOriginalFormat;
    hr = pMediaSeek->GetTimeFormat(&guidOriginalFormat);
    if (FAILED(hr))
        return hr;

    // Ensure media time format for easy display
    hr = pMediaSeek->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
    if (FAILED(hr))
        return hr;

    // Read the file's duration
    LONGLONG llDuration;
    hr = pMediaSeek->GetDuration(&llDuration);
    if (FAILED(hr))
        return hr;

    // Return to the original format
    if (guidOriginalFormat != TIME_FORMAT_MEDIA_TIME)
    {
        hr = pMediaSeek->SetTimeFormat(&guidOriginalFormat);
        if (FAILED(hr))
            return hr;
    }

    // Convert the LONGLONG duration into human-readable format
    unsigned long nTotalMS = (unsigned long) llDuration / 10000; // 100ns -> ms
    int nMS = nTotalMS % 1000;
    int nSeconds = nTotalMS / 1000;
    int nMinutes = nSeconds / 60;
    nSeconds %= 60;

    // Update the string
    swprintf(szDuration, L"%02dm:%02d.%03ds\0", nMinutes, nSeconds, nMS);

    return hr;
}
