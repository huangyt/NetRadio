#include <memory.h>
#include <assert.h>
#include "Encrypt.h"

//=============================================================================
CEncrypt::CEncrypt(void)
{
	m_szEncryFun[ENUM_ENCRYPT_NONE] = &CEncrypt::NullEncrypt;
	m_szEncryFun[ENUM_ENCRYPT_AES]  = &CEncrypt::AesEncrypt;
	m_szEncryFun[ENUM_ENCRYPT_RC6]  = &CEncrypt::Rc6Encrypt;

	m_szDecryFun[ENUM_ENCRYPT_NONE] = &CEncrypt::NullDecrypt;
	m_szDecryFun[ENUM_ENCRYPT_AES]  = &CEncrypt::AesDecrypt;
	m_szDecryFun[ENUM_ENCRYPT_RC6]  = &CEncrypt::Rc6Decrypt;

	memset(&m_stRc6Ctx,0,sizeof(rc6_ctx));
	memset(&m_stAesEncryCtx,0,sizeof(aes_ctx));
	memset(&m_stAesDecryCtx,0,sizeof(aes_ctx));
}

CEncrypt::~CEncrypt(void)
{
}

//设置加密密钥
BOOL CEncrypt::SetEncryptKey(const char* szEncryKey, uint16_t nKeyLen)
{
	assert(szEncryKey);
	assert(nKeyLen >= MIN_ENCRYPT_KEY_SIZE);

	if(NULL == szEncryKey || nKeyLen < MIN_ENCRYPT_KEY_SIZE)
		return FALSE;

	rc6_set_key((unsigned long *)szEncryKey, nKeyLen*8, &m_stRc6Ctx);
	aes_enc_key((unsigned char *)szEncryKey, nKeyLen, &m_stAesEncryCtx);
	aes_dec_key((unsigned char *)szEncryKey, nKeyLen, &m_stAesDecryCtx);

	return TRUE;
}

//加密
BOOL CEncrypt::Encrypt(ENUM_ENCRYPT_TYPE enEncryptType, 
	const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(enEncryptType < ENUM_ENCRYPT_COUNT);
	return (this->*m_szEncryFun[enEncryptType])(szInDataBuff, nInDataSize,
		szOutDataBuff, nOutDataSize);
}

//解密
BOOL CEncrypt::Decrypt(ENUM_ENCRYPT_TYPE enEncryptType, 
	const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(enEncryptType < ENUM_ENCRYPT_COUNT);
	return (this->*m_szDecryFun[enEncryptType])(szInDataBuff, nInDataSize,
		szOutDataBuff, nOutDataSize);
}

//空加密解密
BOOL CEncrypt::NullEncrypt(const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(nOutDataSize >= nInDataSize);
	if(nOutDataSize < nInDataSize)
		return FALSE;

	nOutDataSize = nInDataSize;
	memcpy(szOutDataBuff, szInDataBuff, nInDataSize); //直接复制

	return TRUE;
}

BOOL CEncrypt::NullDecrypt(const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(nOutDataSize >= nInDataSize);
	if(nOutDataSize < nInDataSize)
		return FALSE;

	nOutDataSize = nInDataSize;
	memcpy(szOutDataBuff, szInDataBuff, nInDataSize); //直接复制

	return TRUE;
}

//rc6
BOOL CEncrypt::Rc6Encrypt(const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(m_stRc6Ctx.l_key[0] != 0);
	assert(nOutDataSize >= ((nInDataSize + 15) / 16) * 16);

	nOutDataSize = 0;
	int nDataLen = nInDataSize - nInDataSize % 16;
	while (nOutDataSize < nDataLen)
	{
		rc6_encrypt((const unsigned long *)(szInDataBuff + nOutDataSize),
			(unsigned long *)(szOutDataBuff + nOutDataSize),&m_stRc6Ctx);
		nOutDataSize += 16;
	}

	if (nDataLen < nInDataSize)
	{
		unsigned long  llInBuff[4] = {0};
		memcpy(llInBuff,szInDataBuff + nOutDataSize,nInDataSize % 16);
		rc6_encrypt((const unsigned long *)llInBuff,
			(unsigned long *)(szOutDataBuff + nOutDataSize),&m_stRc6Ctx);
		nOutDataSize += 16;
	}
	return TRUE;
}

BOOL CEncrypt::Rc6Decrypt(const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(m_stRc6Ctx.l_key[0] != 0);
	assert(nOutDataSize >= nInDataSize);

	nOutDataSize = 0;    
	while (nOutDataSize < nInDataSize)
	{
		rc6_decrypt((const unsigned long *)(szInDataBuff + nOutDataSize),
			(unsigned long *)(szOutDataBuff + nOutDataSize),&m_stRc6Ctx);
		nOutDataSize += 16;
	}
	return TRUE;
}

//aes
BOOL CEncrypt::AesEncrypt(const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(m_stAesEncryCtx.n_rnd != 0);
	assert(nOutDataSize >= 
		((nInDataSize + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE);

	nOutDataSize = 0;
	int lnDataLen = nInDataSize - nInDataSize % AES_BLOCK_SIZE;
	while (nOutDataSize < lnDataLen)
	{
		aes_enc_blk((unsigned char *)(szInDataBuff + nOutDataSize),
			(unsigned char *)(szOutDataBuff + nOutDataSize),&m_stAesEncryCtx);
		nOutDataSize += AES_BLOCK_SIZE;
	}

	if (lnDataLen < nInDataSize)
	{
		char lszTmpBuff[AES_BLOCK_SIZE] = {0};
		memcpy(lszTmpBuff,szInDataBuff + nOutDataSize,nInDataSize % AES_BLOCK_SIZE);
		aes_enc_blk((unsigned char *)lszTmpBuff,
			(unsigned char *)(szOutDataBuff + nOutDataSize),&m_stAesEncryCtx);
		nOutDataSize += AES_BLOCK_SIZE;
	}
	return TRUE;
}

BOOL CEncrypt::AesDecrypt(const char* szInDataBuff, uint32_t nInDataSize, 
	char* szOutDataBuff, uint32_t& nOutDataSize)
{
	assert(m_stAesDecryCtx.n_rnd != 0);
	assert(nOutDataSize >= nInDataSize);

	nOutDataSize = 0;    
	while (nOutDataSize < nInDataSize)
	{
		aes_dec_blk((const unsigned char *)(szInDataBuff + nOutDataSize),
			(unsigned char *)(szOutDataBuff + nOutDataSize),&m_stAesDecryCtx);
		nOutDataSize += AES_BLOCK_SIZE;
	}
	return TRUE;
}
