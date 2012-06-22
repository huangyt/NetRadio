#include "StdAfx.h"
#include "VideoEncoder.h"

//=============================================================================
CVideoEncoder::CVideoEncoder(void)
	: m_enCodecType(ENUM_VIDEO_CODEC_H264)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
	, m_nVideoQuant(DEFAULT_CODEC_VIDEO_QUANT)
	, m_nFrameRate(DEFAULT_CODEC_FRAME_RATE)
{
}


CVideoEncoder::~CVideoEncoder(void)
{
}

/// 创建编码器
BOOL CVideoEncoder::Create(ENUM_VIDEO_CODEC_TYPE enCodecType)
{
	BOOL bResult = FALSE;
	switch(enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		m_XvidEncoder.SetFrameInfo(m_nVideoWidth, m_nVideoHeight);
		m_XvidEncoder.SetVideoQuant(m_nVideoQuant);
		m_XvidEncoder.SetFrameRate(m_nFrameRate);
		bResult = m_XvidEncoder.Create();
		break;
	case ENUM_VIDEO_CODEC_H264:
		m_H264Encoder.SetFrameInfo(m_nVideoWidth, m_nVideoHeight);
		m_H264Encoder.SetVideoQuant(m_nVideoQuant);
		m_H264Encoder.SetFrameRate(m_nFrameRate);
		bResult = m_H264Encoder.Create();
		break;
	default:
		break;
	}
	m_enCodecType = enCodecType;
	return bResult;
}

/// 销毁编码器
void CVideoEncoder::Destroy(void)
{
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		m_XvidEncoder.Destroy();
		break;
	case ENUM_VIDEO_CODEC_H264:
		m_H264Encoder.Destroy();
		break;
	default:
		break;
	}	
}

/// 设置视频帧信息
BOOL CVideoEncoder::SetFrameInfo(uint16_t nVideoWidth, 
	uint16_t nVideoHeight)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		bResult = m_XvidEncoder.SetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	case ENUM_VIDEO_CODEC_H264:
		bResult = m_H264Encoder.SetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	default:
		break;
	}

	m_nVideoWidth = nVideoWidth;
	m_nVideoHeight = nVideoHeight;

	return bResult;
}
/// 获得视频帧信息
BOOL CVideoEncoder::GetFrameInfo(uint16_t& nVideoWidth, 
	uint16_t& nVideoHeight) const
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		bResult = m_XvidEncoder.GetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	case ENUM_VIDEO_CODEC_H264:
		bResult = m_H264Encoder.GetFrameInfo(nVideoWidth, nVideoHeight);
		break;
	default:
		break;
	}
	return bResult;
}

/// 设置视频质量
BOOL CVideoEncoder::SetVideoQuant(uint16_t nQuant)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		bResult = m_XvidEncoder.SetVideoQuant(nQuant);
		break;
	case ENUM_VIDEO_CODEC_H264:
		bResult = m_H264Encoder.SetVideoQuant(nQuant);
		break;
	default:
		break;
	}

	m_nVideoQuant = nQuant;
	return bResult;
}

/// 获得视频质量
uint16_t CVideoEncoder::GetVideoQuant(void) const
{
	uint16_t nQuant = 0;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		nQuant = m_XvidEncoder.GetVideoQuant();
		break;
	case ENUM_VIDEO_CODEC_H264:
		nQuant = m_H264Encoder.GetVideoQuant();
		break;
	default:
		break;
	}
	return nQuant;
}

/// 设置帧率
BOOL CVideoEncoder::SetFrameRate(uint16_t nFrameRate)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		bResult = m_XvidEncoder.SetFrameRate(nFrameRate);
		break;
	case ENUM_VIDEO_CODEC_H264:
		bResult = m_H264Encoder.SetFrameRate(nFrameRate);
		break;
	default:
		break;
	}

	m_nFrameRate = nFrameRate;
	return bResult;
}

/// 获得帧率
uint16_t CVideoEncoder::GetFrameRate(void) const
{
	uint16_t nFrameRate = 0;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		nFrameRate = m_XvidEncoder.GetFrameRate();
		break;
	case ENUM_VIDEO_CODEC_H264:
		nFrameRate = m_H264Encoder.GetFrameRate();
		break;
	default:
		break;
	}
	return nFrameRate;
}

/// 编码
int32_t CVideoEncoder::Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	uint32_t nResult = 0;
	switch(m_enCodecType)
	{
	case ENUM_VIDEO_CODEC_XVID:
		nResult = m_XvidEncoder.Encodec(pSrcBuffer, nSrcBuffSize, 
			pDestBuffer, nDestBufferSize);
		break;
	case ENUM_VIDEO_CODEC_H264:
		nResult = m_H264Encoder.Encodec(pSrcBuffer, nSrcBuffSize, 
			pDestBuffer, nDestBufferSize);
		break;
	default:
		break;
	}

	return nResult;
}