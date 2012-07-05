#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include "ConfigFileIni.h"

//=============================================================================
CConfigFileIni::CConfigFileIni(void)
	: m_pFileHandle(NULL)
{
}


CConfigFileIni::~CConfigFileIni(void)
{
	CloseFile();
}

/// 打开
BOOL CConfigFileIni::OpenFile(const char* szPathName)
{
	// 参数检查
	if(NULL == szPathName)
		return FALSE;

	// 重复创建检查
	if(NULL != m_pFileHandle)
		return FALSE;

	BOOL bResult = FALSE;
	
	do 
	{
		m_pFileHandle = fopen(szPathName, "rb");
		if(NULL == m_pFileHandle)
			break;

		if(m_SimpleIni.LoadFile(m_pFileHandle) >= 0)
		{
			return TRUE;
		}
	} while (FALSE);


	return FALSE;
}

/// 关闭
BOOL CConfigFileIni::CloseFile(void)
{
	if(NULL != m_pFileHandle)
	{
		m_SimpleIni.Reset();
		fclose(m_pFileHandle);
		m_pFileHandle = NULL;
		return TRUE;
	}
	return FALSE;
}

/// 保存
BOOL CConfigFileIni::SaveFile(void)
{
	if(NULL != m_pFileHandle)
	{
		if(m_SimpleIni.SaveFile(m_pFileHandle) >= 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/// 获得配置
const char* CConfigFileIni::GetValueString(const char* szSection, 
	const char* szKey, const char* szDefaultValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return szDefaultValue;

	if(NULL == m_pFileHandle)
		return szDefaultValue;

	return m_SimpleIni.GetValue(szSection, szKey, szDefaultValue);
}

/// 设置配置
BOOL CConfigFileIni::SetValueString(const char* szSection, 
	const char* szKey, const char* szValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pFileHandle)
		return FALSE;

	return (m_SimpleIni.SetValue(szSection, szKey, szValue) >= 0);
}

/// 获得配置
int32_t CConfigFileIni::GetValueInt(const char* szSection, 
	const char* szKey, int32_t nDefaultValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pFileHandle)
		return nDefaultValue;

	return m_SimpleIni.GetLongValue(szSection, szKey, nDefaultValue);
}

/// 设置配置
BOOL CConfigFileIni::SetValueInt(const char* szSection, 
	const char* szKey, int32_t nValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pFileHandle)
		return FALSE;

	return (m_SimpleIni.SetLongValue(szSection, szKey, nValue) >= 0);
}

/// 获得配置
double CConfigFileIni::GetValueDouble(const char* szSection, 
	const char* szKey, double nDefaultValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pFileHandle)
		return nDefaultValue;

	return m_SimpleIni.GetDoubleValue(szSection, szKey, nDefaultValue);
}

/// 设置配置
BOOL CConfigFileIni::SetValueDouble(const char* szSection, 
	const char* szKey, double nValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pFileHandle)
		return FALSE;

	return (m_SimpleIni.SetDoubleValue(szSection, szKey, nValue) >= 0);
}