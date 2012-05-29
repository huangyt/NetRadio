#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "UdpPackBuffer.h"

//=============================================================================
/// UDP数据包版本
#define UDP_PACK_VERSION			1
/// UDP数据包包头长度
#define TCP_PACK_HEAD_SIZE			14

/// 最大扩展数据长度
#define MAX_EXT_DATA_SIZE			32

/// TCP数据包头部标志
static uint16_t UDP_PACK_HEAD_TAG = 1317;

//=============================================================================
BOOL _udp_pack_header::Serialize(CNetSerialize & aoNetSerialize)
{
	try
	{
		// 包头标示
		aoNetSerialize.Serialize(m_nPackHeadTag);
		// 版本号
		aoNetSerialize.Serialize(m_nPackVersion);
		// 时间戳
		aoNetSerialize.Serialize(m_nPackTimeStamp);

		// 加密类型
		aoNetSerialize.Serialize(m_nEncryptType);
		// 加密后数据长度
		aoNetSerialize.Serialize(m_nEncryptSize);
		// 数据填充长度
		aoNetSerialize.Serialize(m_nExtFillSize);

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}

BOOL _udp_pack_header::IsValid(void) const
{
	// 检查包头
	if(m_nPackHeadTag != UDP_PACK_HEAD_TAG)
		return FALSE;

	// 加密类型检查
	if(m_nEncryptType < ENUM_ENCRYPT_NONE
		|| m_nEncryptType >= ENUM_ENCRYPT_COUNT)
	{
		return FALSE;
	}

	// 数据包长度检查
	if (m_nEncryptSize > MAX_PACK_BUFFER_SIZE || m_nExtFillSize > MAX_EXT_DATA_SIZE)
	{
		return FALSE;
	}

	return TRUE;
}

//=============================================================================
CUdpPackBuffer::CUdpPackBuffer(void)
	: m_pEncrypt(NULL)
{
	srand((unsigned) time(NULL));
	m_nTimeStamp = rand() % 2000;   

	m_enEncryptType = ENUM_ENCRYPT_NONE;
}


CUdpPackBuffer::~CUdpPackBuffer(void)
{
	try
	{
		Destroy();
	}
	catch (...)
	{
	}
}

BOOL CUdpPackBuffer::Create(ENUM_ENCRYPT_TYPE enEncryptType, 
	const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;

	do 
	{
		if(IsCreated())
			break;

		m_enEncryptType = enEncryptType;

		if(IFAILED(CreateEncrypt(CLSID_IEncrypt, (void**)&m_pEncrypt)))
			break;

		if(NULL != szEncryKey && 0 != nKeySize)
		{
			if(!m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize))
				break;
		}

		bResult = TRUE;

	} while (FALSE);

	if(!bResult)
	{
		if(NULL != m_pEncrypt)
		{
			DestroyEncrypt(CLSID_IEncrypt, m_pEncrypt);
			m_pEncrypt = NULL;
		}
	}

	return bResult;
}

void CUdpPackBuffer::Destroy(void)
{
	if(NULL != m_pEncrypt)
	{
		DestroyEncrypt(CLSID_IEncrypt, m_pEncrypt);
		m_pEncrypt = NULL;
	}

	m_enEncryptType = ENUM_ENCRYPT_NONE;
}

BOOL CUdpPackBuffer::IsCreated(void)
{
	return (NULL != m_pEncrypt);
}

/// 设置加密类型
BOOL CUdpPackBuffer::SetEncryptType(ENUM_ENCRYPT_TYPE enEncryptType)
{
	m_enEncryptType = enEncryptType;
	return TRUE;
}

BOOL CUdpPackBuffer::SetEncryptKey(const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;
	if(NULL != m_pEncrypt)
	{
		bResult = m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize);
	}
	return bResult;
}

/// 打包
uint32_t CUdpPackBuffer::Pack(const char* szInBuffer, uint16_t nInBufferSize, 
	char* szOutBuffer, uint16_t nOutBufferSize)
{
	// 检查是否创建
	if(!IsCreated())
	{
		return 0;
	}

	// 长度检查
	if(nInBufferSize > nOutBufferSize
		|| nInBufferSize + 100 > MAX_PACK_BUFFER_SIZE)
	{
		return 0;
	}

	// 数据加密
	char szEncryptBuffer[MAX_PACK_BUFFER_SIZE] = {0};
	uint32_t nEncryptSize = 0;
	if(!m_pEncrypt->Encrypt(m_enEncryptType, szInBuffer, nInBufferSize, 
		szEncryptBuffer, nEncryptSize))
	{
		return 0;
	}

	udp_pack_header loPackHeader;
	memset(&loPackHeader, 0, sizeof(loPackHeader));

	loPackHeader.m_nPackHeadTag = UDP_PACK_HEAD_TAG;
	loPackHeader.m_nPackVersion = UDP_PACK_VERSION;
	loPackHeader.m_nPackTimeStamp = ++m_nTimeStamp;
	loPackHeader.m_nEncryptType = (uint8_t)m_enEncryptType;
	loPackHeader.m_nEncryptSize = nEncryptSize;
	loPackHeader.m_nExtFillSize = nEncryptSize - nInBufferSize;

	// 开始打包
	int liResult = 0;
	try
	{
		CNetSerialize loNetSerialize(szOutBuffer, nOutBufferSize, 
			CNetSerialize::STORE); 

		// 序列化包头
		loPackHeader.Serialize(loNetSerialize);

		// 加密数据
		loNetSerialize.Serialize(szEncryptBuffer, nEncryptSize, 
			MAX_PACK_BUFFER_SIZE);										

		liResult = loNetSerialize.GetDataSize();						//得到打包后总长
	}
	catch(...)
	{
		liResult = 0;
	}
	return liResult;
}

/// 解包
uint32_t CUdpPackBuffer::UnPack(const char* szInBuffer, uint16_t nInBufferSize, 
	char* szOutBuffer, uint16_t& nOutBufferSize, uint16_t& nTimeStamp)
{
	// 检查是否创建
	if(!IsCreated())
	{
		return 0;
	}

	if(NULL == szInBuffer 
		|| NULL == szOutBuffer 
		|| nOutBufferSize < MAX_PACK_BUFFER_SIZE)
	{
		return 0;
	}

	try
	{
		char* pPackBuffer = const_cast<char*>(szInBuffer);
		uint32_t nPackBufferSize = nInBufferSize;

		CNetSerialize loSerialize(pPackBuffer, nPackBufferSize, 
			CNetSerialize::LOAD); 

		// 序列化包头
		udp_pack_header loPackHeader;
		loPackHeader.Serialize(loSerialize);
		// 判断包头是否正常
		if(!loPackHeader.IsValid())
		{
			// 包头不正常
			return 0;
		}

		// 序列化加密数据
		char szEncryBuff[MAX_PACK_BUFFER_SIZE] = {0};          
		loSerialize.Serialize(szEncryBuff, loPackHeader.m_nEncryptSize, 
			MAX_PACK_BUFFER_SIZE);

		// 解密
		uint32_t nBufferSize = nOutBufferSize;
		if (!m_pEncrypt->Decrypt((ENUM_ENCRYPT_TYPE)loPackHeader.m_nEncryptType, 
			szEncryBuff, loPackHeader.m_nEncryptSize, szOutBuffer, nBufferSize))
		{
			// 解密失败
			return 0;
		}

		nOutBufferSize = nBufferSize - loPackHeader.m_nExtFillSize;
		nTimeStamp = loPackHeader.m_nPackTimeStamp;

		return loSerialize.GetDataSize();
	}
	catch(...)
	{
		return 0;
	}
}