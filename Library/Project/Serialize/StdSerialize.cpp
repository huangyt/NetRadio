#include <assert.h>
#include "StdSerialize.h"

CStdSerialize::CStdSerialize(char* apBuffer, uint32_t alBufferSize, 
	ENUM_TYPE aenType)
	: m_enType(aenType)
	, m_lDataSize(0)
	, m_pBuffer(apBuffer)
	, m_lBufferSize(alBufferSize)
{
	assert( m_pBuffer != NULL );
	assert( m_lBufferSize > 0 );
}


CStdSerialize::~CStdSerialize(void)
{
}

//取得打包长度
long CStdSerialize::GetDataSize(void) const
{
	return m_lDataSize;
}

// 取得序列化类型
CStdSerialize::ENUM_TYPE CStdSerialize::GetType(void) const
{
	return m_enType;
}

//序列化int8_t类型的值
void CStdSerialize::Serialize(int8_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int8_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int8_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int8_t));
	}
	m_lDataSize += sizeof(int8_t);
}

//序列化uint8_t类型的值
void CStdSerialize::Serialize(uint8_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint8_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint8_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint8_t));
	}
	m_lDataSize += sizeof(uint8_t);
}

//序列化int16_t类型的值
void CStdSerialize::Serialize(int16_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int16_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int16_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int16_t));
	}
	m_lDataSize += sizeof(int16_t);
}

//序列化uint16_t类型的值
void CStdSerialize::Serialize(uint16_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint16_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint16_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint16_t));
	}
	m_lDataSize += sizeof(uint16_t);
}

//序列化int32_t类型的值
void CStdSerialize::Serialize(int32_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int32_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int32_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int32_t));
	}
	m_lDataSize += sizeof(int32_t);
}

//序列化uint32_t类型的值
void CStdSerialize::Serialize(uint32_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint32_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint32_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint32_t));
	}
	m_lDataSize += sizeof(uint32_t);
}

//序列化int64_t类型的值
void CStdSerialize::Serialize(int64_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(int64_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(int64_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(int64_t));
	}
	m_lDataSize += sizeof(int64_t);
}

//序列化uint64_t类型的值
void CStdSerialize::Serialize(uint64_t& nValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(uint64_t)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&nValue, m_pBuffer + m_lDataSize, sizeof(uint64_t));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &nValue, sizeof(uint64_t));
	}
	m_lDataSize += sizeof(uint64_t);
}

//序列化float类型的值
void CStdSerialize::Serialize(float& fValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(float)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&fValue, m_pBuffer + m_lDataSize, sizeof(float));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &fValue, sizeof(float));
	}
	m_lDataSize += sizeof(float);
}

//序列化double类型的值
void CStdSerialize::Serialize(double& fValue)
{
	if (m_lBufferSize < (m_lDataSize + sizeof(double)))
		throw( -1 );

	if(m_enType == LOAD )
	{
		//读取
		memcpy(&fValue, m_pBuffer + m_lDataSize, sizeof(double));
	}
	else
	{
		//存储
		memcpy( m_pBuffer + m_lDataSize, &fValue, sizeof(double));
	}
	m_lDataSize += sizeof(double);
}

//序列化串类型的值(以\0结尾的字符串)
void CStdSerialize::Serialize(char* pValue, uint16_t nMaxSize)
{
	if ( m_lBufferSize < ( m_lDataSize+2 ) )
		throw( -1 );

	uint16_t nLen = 0;
	if(m_enType == LOAD)
	{
		//首先读取长度
		memcpy(&nLen, m_pBuffer+m_lDataSize, 2);
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

		memcpy(m_pBuffer+m_lDataSize, &nLen, 2);
		m_lDataSize += 2;

		//存储数据本身
		memcpy(m_pBuffer+m_lDataSize, pValue, nLen);
		m_lDataSize += nLen;
	}
}

// 序列化二进制流
void CStdSerialize::Serialize(char* pValue, uint16_t nSize, uint16_t nMaxSize)
{
	if((nSize > nMaxSize) || (m_lBufferSize < (m_lDataSize+nSize)))
		throw ( -1 );

	if(m_enType == LOAD)
	{
		//因为外部制定了读取长度，所以不需要对数据长度进行序列化
		memcpy(pValue, m_pBuffer+m_lDataSize, nSize);
	}
	else
	{
		//存储数据本身
		memcpy(m_pBuffer+m_lDataSize, pValue, nSize);
	}
	m_lDataSize += nSize;
}
