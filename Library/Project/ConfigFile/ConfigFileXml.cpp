#include "ConfigFileXml.h"

CConfigFileXml::CConfigFileXml(void)
	: m_pXmlDocument(NULL)
{
}


CConfigFileXml::~CConfigFileXml(void)
{
	CloseFile();
}

/// 打开
BOOL CConfigFileXml::OpenFile(const char* szPathName)
{
	// 参数检查
	if(NULL == szPathName)
		return FALSE;

	if(NULL != m_pXmlDocument)
		return FALSE;

	m_pXmlDocument = new TiXmlDocument;
	if(NULL == m_pXmlDocument)
		return FALSE;
	
	return m_pXmlDocument->LoadFile(szPathName, TIXML_ENCODING_UTF8);
}

/// 关闭
BOOL CConfigFileXml::CloseFile(void)
{
	if(NULL != m_pXmlDocument)
	{
		delete m_pXmlDocument;
		m_pXmlDocument = NULL;
		
		return TRUE;
	}
	return FALSE;
}

/// 保存
BOOL CConfigFileXml::SaveFile(void)
{
	if(NULL != m_pXmlDocument)
	{
		return m_pXmlDocument->SaveFile();
	}
	return TRUE;
}

/// 获得配置
const char* CConfigFileXml::GetValueString(const char* szSection, 
	const char* szKey, const char* szDefaultValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return szDefaultValue;

	if(NULL == m_pXmlDocument)
		return szDefaultValue;

	// 获得根节点
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// 获得段节点
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// 获得key节点
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				return pElement->Attribute(szKey);
			}
		}
	}
	return szDefaultValue;
}

/// 设置配置
BOOL CConfigFileXml::SetValueString(const char* szSection, 
	const char* szKey, const char* szValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pXmlDocument)
		return FALSE;

	// 获得根节点
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// 获得段节点
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// 获得key节点
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->SetAttribute(szKey, szValue);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/// 获得配置
int32_t CConfigFileXml::GetValueInt(const char* szSection, 
	const char* szKey, int32_t nDefaultValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pXmlDocument)
		return nDefaultValue;

	int32_t nValue = nDefaultValue;

	// 获得根节点
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// 获得段节点
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// 获得key节点
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->Attribute(szKey, (int*)&nValue);
			}
		}
	}
	return nValue;
}

/// 设置配置
BOOL CConfigFileXml::SetValueInt(const char* szSection, 
	const char* szKey, int32_t nValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pXmlDocument)
		return FALSE;

	// 获得根节点
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// 获得段节点
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// 获得key节点
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->SetAttribute(szKey, nValue);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/// 获得配置
double CConfigFileXml::GetValueDouble(const char* szSection, 
	const char* szKey, double nDefaultValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return nDefaultValue;

	if(NULL == m_pXmlDocument)
		return nDefaultValue;

	double nValue = nDefaultValue;

	// 获得根节点
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// 获得段节点
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// 获得key节点
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->Attribute(szKey, (double*)&nValue);
			}
		}
	}
	return nValue;
}

/// 设置配置
BOOL CConfigFileXml::SetValueDouble(const char* szSection, 
	const char* szKey, double nValue)
{
	// 参数检查
	if(NULL == szSection || NULL == szKey)
		return FALSE;

	if(NULL == m_pXmlDocument)
		return FALSE;

	// 获得根节点
	TiXmlElement* pRootElement = m_pXmlDocument->RootElement();
	if(NULL != pRootElement)
	{
		// 获得段节点
		TiXmlNode* pNode = pRootElement->FirstChild(szSection);
		if(NULL != pNode)
		{
			// 获得key节点
			TiXmlElement* pElement = pNode->ToElement();
			if(NULL != pElement)
			{
				pElement->SetAttribute(szKey, nValue);
				return TRUE;
			}
		}
	}
	return FALSE;
}