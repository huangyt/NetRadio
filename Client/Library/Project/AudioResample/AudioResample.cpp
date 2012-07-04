#include "StdAfx.h"
#include "AudioResample.h"
#include <math.h>

#define RESAMPLE_FRAME_LEN	1024

//=============================================================================
CAudioResample::CAudioResample(void)
	: m_pSrcState(NULL)
	, m_fSrcRadio(0.0)
	, m_nAudioChannel(0)
{
}


CAudioResample::~CAudioResample(void)
{
	Destroy();
}

/// 创建
BOOL CAudioResample::Create(ENUM_RESAMPLE_LEVEL enResampleLevel, 
	int nAudioChannel, ENUM_RESAMPLE_RATE enAudioRate, 
	ENUM_RESAMPLE_RATE enResampleRate)
{
	if(NULL != m_pSrcState)
		return FALSE;

	BOOL bResult = FALSE;
	do 
	{
		int nError = 0;
		m_pSrcState = src_new(enResampleLevel, nAudioChannel, &nError);
		if(NULL != m_pSrcState)
		{
			double src_ratio = (1.0 * enResampleRate) / enAudioRate ;
			if(fabs (src_ratio - 1.0) < 1e-20)
			{
				// 不需要重采样
				break;
			}

			if (src_is_valid_ratio (src_ratio) == 0)
			{	
				break;
			}

			if(src_set_ratio(m_pSrcState, src_ratio) != 0)
				break;

			m_fSrcRadio = src_ratio;
			m_nAudioChannel = nAudioChannel;
			bResult = TRUE;
		}
	} while (FALSE);

	if(!bResult)
	{
		Destroy();
	}

	return bResult;
}

/// 销毁
void CAudioResample::Destroy(void)
{
	if(NULL != m_pSrcState)
	{
		src_delete(m_pSrcState);
		m_pSrcState = NULL;
	}
}

/// 重采样
uint32_t CAudioResample::Resample(int16_t* pInput, uint32_t nInputSize, 
	int16_t* pOutput, uint32_t nOutputSize)
{
	if(NULL == pInput || NULL == pOutput
		|| 0 == nInputSize || 0 == nOutputSize)
	{
		return 0;
	}

	if(NULL == m_pSrcState)
		return 0;

	SRC_DATA src_data;
	src_data.end_of_input = 1 ; /* Set this later. */

	/* Start with zero to force load in while loop. */
	src_data.input_frames = nInputSize / m_nAudioChannel;
	src_data.data_in = (float*)pInput ;

	src_data.src_ratio = m_fSrcRadio ;

	src_data.data_out = (float*)pOutput ;
	src_data.output_frames = nOutputSize / m_nAudioChannel;

	int nError = src_process(m_pSrcState, &src_data);
	if(nError != 0)
	{
		return 0;
	}

	return (src_data.output_frames_gen * m_nAudioChannel);
}