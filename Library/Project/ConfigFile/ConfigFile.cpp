#include "ConfigFile.h"

CConfigFile::CConfigFile(void)
	: m_pConfigFile(NULL)
{
}


CConfigFile::~CConfigFile(void)
{
	CloseFile();
}

/// 打开
BOOL CConfigFile::OpenFile(const char* szPathName, 
	ENUM_CONFIG_FILE_TYPE enFileType)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	// 参数检查
	if(NULL == szPathName)
		return FALSE;

	// 重复创建检查
	if(NULL != m_pConfigFile)
		return FALSE;

	switch(enFileType)
	{
	case CONFIG_FILE_INI:
		m_pConfigFile = new CConfigFileIni;
		break;
	case CONFIG_FILE_XML:
		m_pConfigFile = new CConfigFileXml;
		break;
	default:
		break;
	}

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->OpenFile(szPathName);
	}
	return FALSE;
}

// 关闭
BOOL CConfigFile::CloseFile(void)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->CloseFile();
	}
	return FALSE;
}

/// 保存
BOOL CConfigFile::SaveFile(void)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SaveFile();
	}
	return FALSE;
}

/// 获得配置
const char* CConfigFile::GetValueString(const char* szSection, 
	const char* szKey, const char* szDefaultValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->GetValueString(szSection, szKey, szDefaultValue);
	}

	return szDefaultValue;
}

/// 设置配置
BOOL CConfigFile::SetValueString(const char* szSection, 
	const char* szKey, const char* szValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SetValueString(szSection, szKey, szValue);
	}
	return FALSE;
}

/// 获得配置
int32_t CConfigFile::GetValueInt(const char* szSection, 
	const char* szKey, int32_t nDefaultValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->GetValueInt(szSection, szKey, nDefaultValue);
	}
	return nDefaultValue;
}

/// 设置配置
BOOL CConfigFile::SetValueInt(const char* szSection, 
	const char* szKey, int32_t nValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SetValueInt(szSection, szKey, nValue);
	}
	return FALSE;
}

/// 获得配置
double CConfigFile::GetValueDouble(const char* szSection, 
	const char* szKey, double nDefaultValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->GetValueDouble(szSection, szKey, nDefaultValue);
	}
	return nDefaultValue;
}

/// 设置配置
BOOL CConfigFile::SetValueDouble(const char* szSection, 
	const char* szKey, double nValue)
{
	CCriticalAutoLock loAutoLock(m_oConfigLock);

	if(NULL != m_pConfigFile)
	{
		return m_pConfigFile->SetValueDouble(szSection, szKey, nValue);
	}
	return FALSE;
}