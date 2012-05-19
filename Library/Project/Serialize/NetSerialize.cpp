#include "Endianness.h"
#include "NetSerialize.h"

CNetSerialize::CNetSerialize(char* apBuffer, uint32_t alBufferSize, 
	ENUM_TYPE aenType)
	: CStdSerialize(apBuffer, alBufferSize, aenType)
{
}

CNetSerialize::~CNetSerialize(void)
{
}

//序列化int8_t类型的值
void CNetSerialize::Serialize(int8_t& nValue)
{
	CStdSerialize::Serialize(nValue);
}

//序列化uint8_t类型的值
void CNetSerialize::Serialize(uint8_t& nValue)
{
	CStdSerialize::Serialize(nValue);
}

//序列化int16_t类型的值
void CNetSerialize::Serialize(int16_t& nValue)
{
	if(GetType() == LOAD)
	{
		int16_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_16(nValue);
	}
	else
	{
		int16_t nNetValue = nValue;
		TO_NETWORK_16(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//序列化uint16_t类型的值
void CNetSerialize::Serialize(uint16_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint16_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_16(nValue);
	}
	else
	{
		uint16_t nNetValue = nValue;
		TO_NETWORK_16(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//序列化int32_t类型的值
void CNetSerialize::Serialize(int32_t& nValue)
{
	if(GetType() == LOAD)
	{
		int32_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_32(nValue);
	}
	else
	{
		int32_t nNetValue = nValue;
		TO_NETWORK_32(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//序列化uint32_t类型的值
void CNetSerialize::Serialize(uint32_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint32_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_32(nValue);
	}
	else
	{
		uint32_t nNetValue = nValue;
		TO_NETWORK_32(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//序列化int64_t类型的值
void CNetSerialize::Serialize(int64_t& nValue)
{
	if(GetType() == LOAD)
	{
		int64_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_64(nValue);
	}
	else
	{
		int64_t nNetValue = nValue;
		TO_NETWORK_64(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//序列化uint64_t类型的值
void CNetSerialize::Serialize(uint64_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint64_t nNetValue = 0;
		CStdSerialize::Serialize(nNetValue);
		nValue = nNetValue;
		TO_LOCAL_64(nValue);
	}
	else
	{
		uint64_t nNetValue = nValue;
		TO_NETWORK_64(nNetValue);
		CStdSerialize::Serialize(nNetValue);
	}
}

//序列化float类型的值
void CNetSerialize::Serialize(float& fValue)
{
	if(GetType() == LOAD)
	{
		uint32_t nValue = 0;
		CStdSerialize::Serialize(nValue);
		TO_LOCAL_32(nValue);
		memcpy(&fValue, &nValue, sizeof(uint32_t));
	}
	else
	{
		uint32_t nValue = 0;
		memcpy(&nValue, &fValue, sizeof(uint32_t));
		TO_NETWORK_32(nValue);
		CStdSerialize::Serialize(nValue);
	}
}

//序列化double类型的值
void CNetSerialize::Serialize(double& fValue)
{
	if(GetType() == LOAD)
	{
		uint64_t nValue = 0;
		CStdSerialize::Serialize(nValue);
		TO_LOCAL_64(nValue);
		memcpy(&fValue, &nValue, sizeof(uint64_t));
	}
	else
	{
		uint64_t nValue = 0;
		memcpy(&nValue, &fValue, sizeof(uint64_t));
		TO_NETWORK_64(nValue);
		CStdSerialize::Serialize(nValue);
	}
}

//序列化串类型的值(以\0结尾的字符串)
void CNetSerialize::Serialize(char* pValue, uint16_t nMaxSize)
{
	if ( m_lBufferSize < ( m_lDataSize+2 ) )
		throw( -1 );

	uint16_t nLen = 0;
	if(m_enType == LOAD)
	{
		//首先读取长度
		memcpy(&nLen, m_pBuffer+m_lDataSize, 2);
		// 转换为本地字节序

		TO_LOCAL_16(nLen);
		m_lDataSize += 2;

		//读取数据本身
		if((nLen >= nMaxSize ) || ((m_lDataSize+nLen) > m_lBufferSize))
		{
			throw( -1 );
		}

		memcpy(pValue, m_pBuffer+m_lDataSize, nLen);
		pValue[nLen] = '\0';
		m_lDataSize += nLen;
	}
	else
	{
		//首先存储长度
		nLen = (uint16_t)strlen(pValue);
		if((nLen >= nMaxSize) || (nLen+m_lDataSize+2 > m_lBufferSize))
			throw( -1 );

		// 转换为网络字节序
		uint16_t nDataSize = nLen;
		TO_NETWORK_16(nDataSize);
		memcpy(m_pBuffer+m_lDataSize, &nDataSize, 2);
		m_lDataSize += 2;

		//存储数据本身
		memcpy(m_pBuffer+m_lDataSize, pValue, nLen);
		m_lDataSize += nLen;
	}
}

// 序列化二进制流
void CNetSerialize::Serialize(char* pValue, uint16_t nSize, uint16_t nMaxSize)
{
	CStdSerialize::Serialize(pValue, nSize, nMaxSize);
}
