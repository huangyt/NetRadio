#include "StdAfx.h"
#include "AacCodec.h"
#include "TypeDefine.h"

//=============================================================================
CAacEncoder::CAacEncoder(void)
	: m_hHandleEncoder(NULL)
	, m_enFrequency(ENUM_FREQUENCY_44KHZ)
	, m_enChannel(ENUM_CHANNEL_STEREO)
	, m_enSample(ENUM_SAMPLE_16BIT)
	, m_nAudioQuant(85)
	, m_nBandWidth(16000)
	, m_nInputSamples(0)
	, m_nMaxOutputBytes(0)
{
}


CAacEncoder::~CAacEncoder(void)
{
	Destroy();
}

/// 创建编码器
BOOL CAacEncoder::Create(void)
{
	if(NULL != m_hHandleEncoder)
		return FALSE;

	m_hHandleEncoder = faacEncOpen(m_enFrequency, m_enChannel, 
		(unsigned long*)&m_nInputSamples, (unsigned long*)&m_nMaxOutputBytes);

	faacEncConfigurationPtr pConfig = faacEncGetCurrentConfiguration(m_hHandleEncoder);
	if(NULL != pConfig)
	{
		int nInputFormat = FAAC_INPUT_16BIT;
		switch(m_enSample)
		{
		case ENUM_SAMPLE_8BIT:
			nInputFormat = FAAC_INPUT_NULL;
			break;
		case ENUM_SAMPLE_16BIT:
			nInputFormat = FAAC_INPUT_16BIT;
			break;
		default:
			nInputFormat = FAAC_INPUT_16BIT;
			break;
		}

		pConfig->version = MPEG4;				//MPEG4;
		pConfig->outputFormat = 1;				//ADTS;		
		pConfig->inputFormat = nInputFormat;		//FAAC_INPUT_16BIT;
		pConfig->aacObjectType = LOW;			//LOW;
		pConfig->useTns = 0;						//DEFAULT_TNS;
		pConfig->shortctl =  SHORTCTL_NORMAL;	//SHORTCTL_NORMAL;
		pConfig->allowMidside = 1;

		pConfig->quantqual = m_nAudioQuant;		//Audio Auant (0-100)
		pConfig->bandWidth = m_nBandWidth;		//Output Band Width

		if(0 == faacEncSetConfiguration(m_hHandleEncoder, pConfig))
		{
			faacEncClose(m_hHandleEncoder);
			m_hHandleEncoder = NULL;
		}
	}

	return (NULL != m_hHandleEncoder);

}

/// 销毁编码器
void CAacEncoder::Destroy(void)
{
	if(NULL != m_hHandleEncoder)
	{
		faacEncClose(m_hHandleEncoder);
		m_hHandleEncoder = NULL;
	}
}

/// 获得音频参数
BOOL CAacEncoder::GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
	ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const
{
	enFrequency = m_enFrequency;
	enChannel = m_enChannel;
	enSample = m_enSample;
	return TRUE;
}

/// 设置音频参数
BOOL CAacEncoder::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency, 
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
{
	if(NULL == m_hHandleEncoder)
	{
		m_enFrequency = enFrequency;
		m_enChannel = enChannel;
		m_enSample = enSample;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/// 设置音频质量
BOOL CAacEncoder::SetAudioQuant(uint16_t nQuant)
{
	if(NULL == m_hHandleEncoder)
	{
		m_nAudioQuant = nQuant;
		return TRUE;
	}
	else
	{
		BOOL bResult = FALSE;
		faacEncConfigurationPtr pConfig = faacEncGetCurrentConfiguration(m_hHandleEncoder);
		if(NULL != pConfig)
		{
			pConfig->quantqual = m_nAudioQuant;
			int nRet = faacEncSetConfiguration(m_hHandleEncoder, pConfig);
			if(nRet != 0)
			{
				m_nAudioQuant = nQuant;
				bResult = TRUE;
			}
		}
		return FALSE;
	}
}

/// 获得音频质量
uint16_t CAacEncoder::GetAudioQuant(void) const
{
	return m_nAudioQuant;
}

/// 设置带宽
BOOL CAacEncoder::SetBandWidth(uint32_t nBandWidth)
{
	if(NULL == m_hHandleEncoder)
	{
		m_nBandWidth = nBandWidth;
		return TRUE;
	}
	else
	{
		BOOL bResult = FALSE;
		faacEncConfigurationPtr pConfig = faacEncGetCurrentConfiguration(m_hHandleEncoder);
		if(NULL != pConfig)
		{
			pConfig->bandWidth = m_nBandWidth;
			int nRet = faacEncSetConfiguration(m_hHandleEncoder, pConfig);
			if(nRet != 0)
			{
				m_nBandWidth = nBandWidth;
				bResult = TRUE;
			}
		}
		return FALSE;
	}
}

/// 获得带宽
uint32_t CAacEncoder::GetBandWidth(void) const
{
	return m_nBandWidth;
}

/// 编码
int32_t CAacEncoder::Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	// 参数检查
	if(NULL == pSrcBuffer || 0 == nSrcBuffSize
		|| NULL == pDestBuffer || nDestBufferSize < m_nMaxOutputBytes)
	{
		return 0;
	}

	return faacEncEncode(m_hHandleEncoder, (int32_t*)pSrcBuffer, m_nInputSamples, 
		(unsigned char*)pDestBuffer, nDestBufferSize);
}


//=============================================================================
CAacDecoder::CAacDecoder(void)
	: m_hHandleDecoder(NULL)
	, m_enFrequency(ENUM_FREQUENCY_44KHZ)
	, m_enChannel(ENUM_CHANNEL_STEREO)
	, m_enSample(ENUM_SAMPLE_16BIT)
	, m_bIsInitDec(FALSE)
{

}

CAacDecoder::~CAacDecoder(void)
{
	Destroy();
}

/// 创建编码器
BOOL CAacDecoder::Create(void)
{
	if(NULL != m_hHandleDecoder)
		return FALSE;

	m_hHandleDecoder = faacDecOpen();

	faacDecConfigurationPtr pConfig = NULL;
	pConfig = faacDecGetCurrentConfiguration(m_hHandleDecoder);
	if(NULL != pConfig)
	{
		int nOutputFormat = FAAD_FMT_16BIT;
		switch(m_enSample)
		{
		case ENUM_SAMPLE_8BIT:
			nOutputFormat = 0;
			break;
		case ENUM_SAMPLE_16BIT:
			nOutputFormat = FAAD_FMT_16BIT;
			break;
		default:
			nOutputFormat = FAAD_FMT_16BIT;
			break;
		}

		pConfig->defObjectType = LOW;
		pConfig->defSampleRate = m_enFrequency;
		pConfig->outputFormat = nOutputFormat;
		pConfig->dontUpSampleImplicitSBR = 1;
	
		if(0 == faacDecSetConfiguration(m_hHandleDecoder, pConfig))
		{
			faacDecClose(m_hHandleDecoder);
			m_hHandleDecoder = NULL;
		}
	}
	//faacDecInitDRM(&m_hHandleDecoder, 44100, 2);

	return (NULL != m_hHandleDecoder);
}

/// 销毁编码器
void CAacDecoder::Destroy(void)
{
	if(NULL != m_hHandleDecoder)
	{
		faacDecClose(m_hHandleDecoder);
		m_hHandleDecoder = NULL;
	}
}

/// 获得音频参数
BOOL CAacDecoder::GetAudioFormat(ENUM_FREQUENCY_TYPE& enFrequency,
	ENUM_CHANNEL_TYPE& enChannel, ENUM_SAMPLE_TYPE& enSample) const
{
	enFrequency = m_enFrequency;
	enChannel = m_enChannel;
	enSample = m_enSample;
	return TRUE;
}

/// 设置音频参数
BOOL CAacDecoder::SetAudioFormat(ENUM_FREQUENCY_TYPE enFrequency, 
	ENUM_CHANNEL_TYPE enChannel, ENUM_SAMPLE_TYPE enSample)
{
	if(NULL == m_hHandleDecoder)
	{
		m_enFrequency = enFrequency;
		m_enChannel = enChannel;
		m_enSample = enSample;
		return TRUE;
	}
	else
	{
		BOOL bResult = FALSE;
		faacDecConfigurationPtr pConfig = NULL;
		pConfig = faacDecGetCurrentConfiguration(m_hHandleDecoder);
		if(NULL != pConfig)
		{
			int nOutputFormat = FAAD_FMT_16BIT;
			switch(m_enSample)
			{
			case ENUM_SAMPLE_8BIT:
				nOutputFormat = 0;
				break;
			case ENUM_SAMPLE_16BIT:
				nOutputFormat = FAAD_FMT_16BIT;
				break;
			default:
				nOutputFormat = FAAD_FMT_16BIT;
				break;
			}

			pConfig->defSampleRate = m_enFrequency;
			pConfig->outputFormat = nOutputFormat;

			if(0 != faacDecSetConfiguration(m_hHandleDecoder, pConfig))
			{
				m_enFrequency = enFrequency;
				m_enChannel = enChannel;
				m_enSample = enSample;
				bResult = TRUE;
			}
		}
		return bResult;
	}
}

/// 编码
int32_t CAacDecoder::Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	int32_t nResult = 0;

	if(NULL != m_hHandleDecoder)
	{
		if(!m_bIsInitDec)
		{
			unsigned long nSamplesRate = 0;
			unsigned char nChannel = 0;

			faacDecInit(m_hHandleDecoder, (unsigned char*)pSrcBuffer, nSrcBuffSize, 
				&nSamplesRate, &nChannel);

			m_bIsInitDec = TRUE;
		}

		if(m_bIsInitDec)
		{
			char* pSampleBuffer = (char*)NeAACDecDecode2(m_hHandleDecoder, &m_aacFrameInfo,
				(unsigned char*)pSrcBuffer, nSrcBuffSize, (void**)&pDestBuffer, nDestBufferSize);

			if(NULL != pSampleBuffer 
				&& m_aacFrameInfo.error == 0 
				&& m_aacFrameInfo.samples > 0)
			{
				nResult = m_aacFrameInfo.samples * m_aacFrameInfo.channels;
				if(nResult > nDestBufferSize)
				{
					nResult = 0;
				}
			}
		}
	}

	return nResult;
}

