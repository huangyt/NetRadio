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

//序列化int16_t类型的值
void CNetSerialize::Serialize(int16_t& nValue)
{
	if(GetType() == LOAD)
	{
		int16_t nLittleValue = 0;
		CStdSerialize::Serialize(nLittleValue);
		nValue = TO_LOCAL_16(nLittleValue);
	}
	else
	{
		int16_t nBigValue = TO_NETWORK_16(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}

//序列化uint16_t类型的值
void CNetSerialize::Serialize(uint16_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint16_t nLittleValue = 0;
		CStdSerialize::Serialize(nLittleValue);
		nValue = TO_LOCAL_16(nLittleValue);
	}
	else
	{
		uint16_t nBigValue = TO_NETWORK_16(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}

//序列化int32_t类型的值
void CNetSerialize::Serialize(int32_t& nValue)
{
	if(GetType() == LOAD)
	{
		int32_t nLittleValue = 0;
		CStdSerialize::Serialize(nLittleValue);
		nValue = TO_LOCAL_32(nLittleValue);
	}
	else
	{
		int32_t nBigValue = TO_NETWORK_32(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}

//序列化uint32_t类型的值
void CNetSerialize::Serialize(uint32_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint32_t nLittleValue = 0;
		CStdSerialize::Serialize(nLittleValue);
		nValue = TO_LOCAL_32(nLittleValue);
	}
	else
	{
		uint32_t nBigValue = TO_NETWORK_32(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}

//序列化int64_t类型的值
void CNetSerialize::Serialize(int64_t& nValue)
{
	if(GetType() == LOAD)
	{
		int64_t nLittleValue = 0;
		CStdSerialize::Serialize(nLittleValue);
		nValue = TO_LOCAL_64(nLittleValue);
	}
	else
	{
		int64_t nBigValue = TO_NETWORK_64(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}

//序列化uint64_t类型的值
void CNetSerialize::Serialize(uint64_t& nValue)
{
	if(GetType() == LOAD)
	{
		uint64_t nLittleValue = 0;
		CStdSerialize::Serialize(nLittleValue);
		nValue = TO_LOCAL_64(nLittleValue);
	}
	else
	{
		uint64_t nBigValue = TO_NETWORK_64(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}

//序列化float类型的值
void CNetSerialize::Serialize(float& fValue)
{
	if(GetType() == LOAD)
	{
		uint32_t nValue = 0;
		CStdSerialize::Serialize(nValue);
		uint32_t nLittleValue = TO_LOCAL_32(nValue);
		memcpy(&fValue, &nValue, sizeof(uint32_t));
	}
	else
	{
		uint32_t nValue = 0;
		memcpy(&nValue, &fValue, sizeof(uint32_t));
		uint32_t nBigValue = TO_NETWORK_32(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}

//序列化double类型的值
void CNetSerialize::Serialize(double& fValue)
{
	if(GetType() == LOAD)
	{
		uint64_t nValue = 0;
		CStdSerialize::Serialize(nValue);
		uint64_t nLittleValue = TO_LOCAL_64(nValue);
		memcpy(&fValue, &nValue, sizeof(uint64_t));
	}
	else
	{
		uint64_t nValue = 0;
		memcpy(&nValue, &fValue, sizeof(uint64_t));
		uint64_t nBigValue = TO_NETWORK_64(nValue);
		CStdSerialize::Serialize(nBigValue);
	}
}
