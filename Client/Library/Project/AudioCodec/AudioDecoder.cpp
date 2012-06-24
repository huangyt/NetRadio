#include "StdAfx.h"
#include "AudioDecoder.h"


CAudioDecoder::CAudioDecoder(void)
	: m_enCodecType(ENUM_AUDIO_CODEC_AAC)
{
}


CAudioDecoder::~CAudioDecoder(void)
{
}


/// 创建编码器
BOOL CAudioDecoder::Create(ENUM_AUDIO_CODEC_TYPE enCodecType)
{
	BOOL bResult = FALSE;
	switch(enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	bResult = m_Mp3Decoder.Create();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacDecoder.Create();
		break;
	default:
		break;
	}
	m_enCodecType = enCodecType;
	return bResult;
}

/// 销毁编码器
void CAudioDecoder::Destroy(void)
{
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	m_Mp3Decoder.Destroy();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		m_AacDecoder.Destroy();
		break;
	default:
		break;
	}
}

/// 获得音频参数
BOOL CAudioDecoder::GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
	ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	bResult = m_Mp3Decoder.Create();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacDecoder.GetAudioFormat(enFrequency, enChannel, enSample);
		break;
	default:
		break;
	}
	return bResult;
}

/// 设置音频参数
BOOL CAudioDecoder::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency, 
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
{
	BOOL bResult = FALSE;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	bResult = m_Mp3Decoder.Create();
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		bResult = m_AacDecoder.SetAudioFormat(enFrequency, enChannel, enSample);
		break;
	default:
		break;
	}
	return bResult;
}

/// 解码
int32_t CAudioDecoder::Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	int32_t nResult = 0;
	switch(m_enCodecType)
	{
	//case ENUM_AUDIO_CODEC_MP3:
	//	nResult = m_Mp3Decoder.Decodec(pSrcBuffer, nSrcBuffSize, 
	//		pDestBuffer, nDestBufferSize);
	//	break;
	case ENUM_AUDIO_CODEC_AAC:
		nResult = m_AacDecoder.Decodec(pSrcBuffer, nSrcBuffSize, 
			pDestBuffer, nDestBufferSize);
		break;
	default:
		break;
	}
	return nResult;
}