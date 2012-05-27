#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "TcpPackBuffer.h"

//=============================================================================
/// TCP数据包版本
#define TCP_PACK_VERSION		1
/// TCP数据包包头长度
#define TCP_PACK_HEAD_SIZE		14
/// TCP数据包包尾长度
#define TCP_PACK_TAIL_SIZE		3

/// 最大扩展数据长度
#define MAX_EXT_DATA_SIZE		32

/// TCP数据包头部标志
static char TCP_PACK_HEAD_TAG[] = "\03\02\01\0";
/// TCP数据包尾部标志
static char TCP_PACK_TAIL_TAG[] = "\06\05\04\0";

//=============================================================================
BOOL _tcp_pack_header::Serialize(CNetSerialize & aoNetSerialize)
{
	try
	{
		// 包头标示
		aoNetSerialize.Serialize(m_szPackHeadTag, 3, 3);
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

BOOL _tcp_pack_header::IsValid(void) const
{
	// 检查包头
	if(memcmp(m_szPackHeadTag, TCP_PACK_HEAD_TAG, 3) != 0)
		return FALSE;

	// 加密类型检查
	if(m_nEncryptType <= ENUM_ENCRYPT_NONE
		|| m_nEncryptType > ENUM_ENCRYPT_COUNT)
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
CTcpPackBuffer::CTcpPackBuffer(void)
	: m_pEncrypt(NULL)
{
	srand((unsigned) time(NULL));
	m_nTimeStamp = rand() % 2000;   

	memset(m_szBuffer, 0, MAX_PACK_BUFFER_SIZE*2);
	m_nDataSize = 0;
}


CTcpPackBuffer::~CTcpPackBuffer(void)
{
	try
	{
		Destroy();
	}
	catch (...)
	{
	}
}

BOOL CTcpPackBuffer::Create(const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;
	
	do 
	{
		if(IsCreated())
			break;
			
		if(NULL == szEncryKey || 0 == nKeySize)
			break;

		if(IFAILED(CreateEncrypt(CLSID_IEncrypt, (void**)&m_pEncrypt)))
			break;

		if(!m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize))
			break;

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

void CTcpPackBuffer::Destroy(void)
{
	if(NULL != m_pEncrypt)
	{
		DestroyEncrypt(CLSID_IEncrypt, m_pEncrypt);
		m_pEncrypt = NULL;
	}
}

BOOL CTcpPackBuffer::IsCreated(void)
{
	return (NULL != m_pEncrypt);
}

BOOL CTcpPackBuffer::SetEncryptKey(const char* szEncryKey, uint16_t nKeySize)
{
	BOOL bResult = FALSE;
	if(NULL != m_pEncrypt)
	{
		bResult = m_pEncrypt->SetEncryptKey(szEncryKey, nKeySize);
	}
	return bResult;
}

/// 打包
uint32_t CTcpPackBuffer::Pack(const char* szInBuffer, uint16_t nInBufferSize, 
	char* szOutBuffer, uint16_t nOutBufferSize, 
	ENUM_ENCRYPT_TYPE enEncryptType)
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
	uint32_t nEncryptSize = MAX_PACK_BUFFER_SIZE;
    if(!m_pEncrypt->Encrypt(enEncryptType, szInBuffer, nInBufferSize, 
		szEncryptBuffer, nEncryptSize))
    {
        return 0;
    }

	tcp_pack_header loPackHeader;
	memset(&loPackHeader, 0, sizeof(loPackHeader));

	memcpy(loPackHeader.m_szPackHeadTag, TCP_PACK_HEAD_TAG, 4);
	loPackHeader.m_nPackVersion = TCP_PACK_VERSION;
	loPackHeader.m_nPackTimeStamp = ++m_nTimeStamp;
	loPackHeader.m_nEncryptType = (uint8_t)enEncryptType;
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

		// 包尾
        loNetSerialize.Serialize(TCP_PACK_TAIL_TAG, TCP_PACK_TAIL_SIZE, 
			TCP_PACK_TAIL_SIZE); 

        liResult = loNetSerialize.GetDataSize();						//得到打包后总长
    }
    catch(...)
    {
        liResult = 0;
    }
    return liResult;
}

/// 解包
uint32_t CTcpPackBuffer::UnPack(const char* szInBuffer, uint16_t nInBufferSize, 
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
		uint32_t nUnpackSize = m_nDataSize;
		uint32_t nPackBufferSize = nInBufferSize+m_nDataSize;

		// 如果有尚未解压的数据
		if(m_nDataSize > 0)
		{
			pPackBuffer = m_szBuffer;
			memcpy(m_szBuffer+m_nDataSize, szInBuffer, nInBufferSize);
		}

		CNetSerialize loSerialize(pPackBuffer, nPackBufferSize, 
			CNetSerialize::LOAD); 

		// 序列化包头
		tcp_pack_header loPackHeader;
		loPackHeader.Serialize(loSerialize);
		// 判断包头是否正常
		if(!loPackHeader.IsValid())
		{
			// 包头不正常
			return 0;
		}

		// 检查数据包是否接收完毕
		if (nPackBufferSize < (loPackHeader.m_nEncryptSize + 
			loSerialize.GetDataSize() + TCP_PACK_TAIL_SIZE))
		{
			//数据包接收不全
			memcpy(m_szBuffer, pPackBuffer, nPackBufferSize);
			m_nDataSize = nPackBufferSize;
			return 0;
		}

		// 检查包尾是否正常
		char szPackTailTag[4] = {0};
		memcpy(szPackTailTag, 
			szInBuffer+loSerialize.GetDataSize()+ loPackHeader.m_nEncryptSize, 
			TCP_PACK_TAIL_SIZE);
		if (memcmp(szPackTailTag, TCP_PACK_TAIL_TAG, TCP_PACK_TAIL_SIZE) != 0)
		{
			// 包尾不正常
			m_nDataSize = 0;
			return 0;
		}

		// 序列化加密数据
		char szEncryBuff[MAX_PACK_BUFFER_SIZE] = {0};          
        loSerialize.Serialize(szEncryBuff, loPackHeader.m_nEncryptSize, 
			MAX_PACK_BUFFER_SIZE);

		// 序列化包尾
		loSerialize.Serialize(szPackTailTag, TCP_PACK_TAIL_SIZE, 4);

		// 解密
		uint32_t nBufferSize = nOutBufferSize;
		if (!m_pEncrypt->Decrypt((ENUM_ENCRYPT_TYPE)loPackHeader.m_nEncryptType, 
			szEncryBuff, loPackHeader.m_nEncryptSize, szOutBuffer, nBufferSize)) //解密失败
		{
			// 解密失败
			m_nDataSize = 0;
			return 0;
		}

		nOutBufferSize = nBufferSize - loPackHeader.m_nExtFillSize;
		nTimeStamp = loPackHeader.m_nPackTimeStamp;

		return loSerialize.GetDataSize()-nUnpackSize;
	}
	catch(...)
	{
		return 0;
	}
}