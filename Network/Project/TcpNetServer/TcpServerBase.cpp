#include "TcpServerBase.h"
#include "DebugTrace.h"

CTcpServerBase::CTcpServerBase(void)
	: m_hListenSocket(INVALID_SOCKET)
	, m_pTcpEvent(NULL)
{
}


CTcpServerBase::~CTcpServerBase(void)
{
}

/// 设置加密类型
BOOL CTcpServerBase::SetEncryptType(ENUM_ENCRYPT_TYPE enType)
{
	if(m_SendPackBuffer.SetEncryptType(enType))
	{
		m_EncryptInfo.SetEncryptType(enType);
		return TRUE;
	}
	return FALSE;
}

/// 设置加密密钥
BOOL CTcpServerBase::SetEncryptKey(const char *szEncryptKey, uint16_t nKeySize)
{
	if(m_SendPackBuffer.SetEncryptKey(szEncryptKey, nKeySize))
	{
		m_EncryptInfo.SetEncryptKey(szEncryptKey, nKeySize);
		return TRUE;
	}
	return FALSE;
}

/// 检查Context是否有效
BOOL CTcpServerBase::ContextIsValid(const CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// 通过KEY值取出Context指针（该指针有可能已经失效，请勿修改该指针)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// 判断指针地址是否相同
	if(pTempContext == pContext)
	{
		// 判断SOCKET句柄是否相同
		if(pTempContext->m_hSocket == pContext->m_hSocket)
			bResult = TRUE;
	}
	return bResult;
}

BOOL CTcpServerBase::ResetContext(CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// 通过KEY值取出Context指针（该指针有可能已经失效，请勿修改该指针)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// 判断指针地址是否相同
	if(pTempContext == pContext)
	{
		// 判断SOCKET句柄是否相同
		if(pTempContext->m_hSocket == pContext->m_hSocket 
			&& pTempContext->m_oSocketAddr.sin_addr.s_addr 
			== pContext->m_oSocketAddr.sin_addr.s_addr)
		{
			DestroySocket(pContext->m_hSocket);
			pContext->m_hSocket = INVALID_SOCKET;
			bResult = TRUE;
		}
	}
	return bResult;
}

//=============================================================================
/// 创建SOCKET套接字
SOCKET CTcpServerBase::CreateSocket(uint16_t nSvrPort)
{
	// 创建监听Socket
	SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	return hSocket;
}

/// 销毁SOCKET套接字
void CTcpServerBase::DestroySocket(SOCKET hSocket)
{
	closesocket(hSocket);
	hSocket = INVALID_SOCKET;
}

//=============================================================================
/// 创建Context
CTcpContext* CTcpServerBase::CreateContext(void)
{
	CTcpContext* pContext = m_pTcpEvent->MallocContext();
	if(NULL != pContext)
	{
		// 创建Context中的PackBuffer
		pContext->m_TcpPackBuffer.Create(m_EncryptInfo.GetEncryptType());
		// 设置加密密钥
		pContext->m_TcpPackBuffer.SetEncryptKey(m_EncryptInfo.GetEncryptKey(), 
			m_EncryptInfo.GetEncryptKeySize());
	}
	return pContext;
}

/// 释放Context
void CTcpServerBase::DestroyContext(CTcpContext* pContext)
{
	if(NULL != pContext)
	{
		// 销毁Context中的PackBuffer
		pContext->m_TcpPackBuffer.Destroy();
	}
}

/// 添加Context
BOOL CTcpServerBase::AddTcpContext(CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	CCriticalAutoLock loAutoLock(m_ContextListLock);
	POSITION pos = m_ContextList.AddTail(pContext);
	pContext->m_i64ContextKey = (uint64_t)pos;
	return TRUE;
}

/// 删除Context
BOOL CTcpServerBase::RemoveTcpContext(CTcpContext* pContext)
{
	// 参数检查
	ASSERT(pContext);
	if(NULL == pContext)
		return FALSE;

	BOOL bResult = FALSE;
	POSITION pos = (POSITION)pContext->m_i64ContextKey;

	CCriticalAutoLock loAutoLock(m_ContextListLock);

	// 通过KEY值取出Context指针（该指针有可能已经失效，请勿修改该指针)
	CTcpContext* pTempContext = m_ContextList.GetAt(pos);
	// 判断指针地址是否相同
	if(pTempContext == pContext)
	{
		// 判断SOCKET句柄是否相同
		if(pTempContext->m_hSocket == pContext->m_hSocket)
		{
			m_ContextList.RemoveAt(pos);
			DestroyContext(pContext);
			bResult = TRUE;
		}
	}
	return bResult;
}

/// 关闭所有Context
void CTcpServerBase::CloseAllContext(void)
{
	//关闭所有已连接SOCKET
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		DestroySocket(pContext->m_hSocket);
		pContext->m_hSocket = INVALID_SOCKET;
	}
}

/// 关闭无效链接
void CTcpServerBase::CheckInvalidContext(void)
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);

	//检查无效连接
	POSITION pos = m_ContextList.GetHeadPosition();
	while(NULL != pos)
	{
		CTcpContext* pContext = m_ContextList.GetNext(pos);
		if(!pContext->CheckValid())
		{
			DestroySocket(pContext->m_hSocket);
			pContext->m_hSocket = INVALID_SOCKET;
		}
	}
}

/// 获得连接数量
uint32_t CTcpServerBase::GetTcpContextCount(void) const
{
	CCriticalAutoLock loAutoLock(m_ContextListLock);
	return m_ContextList.GetCount();
}

//=============================================================================
/// 处理接收数据
BOOL CTcpServerBase::DealRecvData(const char* szDataBuffer, uint32_t nDataSize, 
	CTcpContext *pContext)
{
	// 参数检查
	ASSERT(szDataBuffer);
	ASSERT(nDataSize);
	ASSERT(pContext);
	if(0 == nDataSize || NULL == szDataBuffer || NULL == pContext)
		return FALSE;

	char szRecvBuffer[MAX_PACK_BUFFER_SIZE] = {0};
	uint16_t nRecvSize = MAX_PACK_BUFFER_SIZE;
	uint16_t nTimeStamp = 0;

	const char* pBuffer = szDataBuffer;
	uint16_t nSize = nDataSize;

	for(;;)
	{
		// 解包
		int32_t nPackSize = pContext->m_TcpPackBuffer.UnPack(pBuffer, 
			nSize, szRecvBuffer, nRecvSize, nTimeStamp);
		if(nPackSize <= 0)
			break;

		// 回调数据
		m_pTcpEvent->OnRecvData(szRecvBuffer, nRecvSize, pContext);

		// 判断是否全部解完
		if(nPackSize < nSize)
		{
			pBuffer = pBuffer + nPackSize;
			nSize = nSize - nPackSize;
		}
	}

	return TRUE;
}