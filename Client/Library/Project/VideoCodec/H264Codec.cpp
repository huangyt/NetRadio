#include "StdAfx.h"
#include "H264Codec.h"
#include "ColorSpaceAPI.h"

//=============================================================================
// class CH264Encoder
CH264Encoder::CH264Encoder(void)
	: m_pHandleEncoder(NULL)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
	, m_nVideoQuant(DEFAULT_CODEC_VIDEO_QUANT)
	, m_nFrameRate(DEFAULT_CODEC_FRAME_RATE)
{
	init_rgb2yuv_table();
}


CH264Encoder::~CH264Encoder(void)
{
}

/// 创建
BOOL CH264Encoder::Create(void)
{
	if(NULL != m_pHandleEncoder)
	{
		return FALSE;
	}

	// 获得默认参数

	x264_param_default(&m_EncoderParam);

	m_EncoderParam.cpu = X264_CPU_CACHELINE_32 | X264_CPU_MMX;
	m_EncoderParam.i_width = m_nVideoWidth;
	m_EncoderParam.i_height = m_nVideoHeight;
	m_EncoderParam.i_csp = X264_CSP_I420;
	m_EncoderParam.i_fps_num = m_nFrameRate;

	m_EncoderParam.rc.i_rc_method = X264_RC_CRF; 
	m_EncoderParam.rc.f_rf_constant = 26.0;
	//m_EncoderParam.rc.i_bitrate = m_nVideoQuant;


	m_pHandleEncoder = x264_encoder_open(&m_EncoderParam);
	if(NULL != m_pHandleEncoder)
	{
		x264_picture_alloc(&m_inPicture, m_EncoderParam.i_csp, 
			m_EncoderParam.i_width, m_EncoderParam.i_height );

		x264_picture_alloc(&m_outPicture, m_EncoderParam.i_csp, 
			m_EncoderParam.i_width, m_EncoderParam.i_height );
		
	}
	return (NULL != m_pHandleEncoder);
}

/// 销毁
void CH264Encoder::Destroy(void)
{
	if(NULL != m_pHandleEncoder)
	{
		x264_encoder_close(m_pHandleEncoder);
		m_pHandleEncoder = NULL;

		x264_picture_clean(&m_inPicture);
		x264_picture_clean(&m_outPicture);
	}
}

/// 设置视频帧信息
BOOL CH264Encoder::SetFrameInfo(uint16_t nVideoWidth, uint16_t nVideoHeight)
{
	if(NULL != m_pHandleEncoder)
	{
		m_EncoderParam.i_width = nVideoWidth;
		m_EncoderParam.i_height = nVideoHeight;

		x264_encoder_reconfig(m_pHandleEncoder, &m_EncoderParam);
	}

	m_nVideoWidth = nVideoWidth;
	m_nVideoHeight = nVideoHeight;

	return TRUE;
}

/// 获得视频帧信息
BOOL CH264Encoder::GetFrameInfo(uint16_t& nVideoWidth, 
	uint16_t& nVideoHeight) const
{
	nVideoWidth = m_nVideoWidth;
	nVideoHeight = m_nVideoHeight;

	return TRUE;
}

/// 设置视频质量
BOOL CH264Encoder::SetVideoQuant(uint16_t nQuant)
{
	if(NULL != m_pHandleEncoder)
	{
		m_EncoderParam.rc.i_bitrate = nQuant;

		x264_encoder_reconfig(m_pHandleEncoder, &m_EncoderParam);

	}

	m_nVideoQuant = nQuant;

	return TRUE;
}

/// 获得视频质量
uint16_t CH264Encoder::GetVideoQuant(void) const
{
	return m_nVideoQuant;
}

/// 设置帧率
BOOL CH264Encoder::SetFrameRate(uint16_t nFrameRate)
{
	if(NULL != m_pHandleEncoder)
	{
		m_EncoderParam.i_fps_num = nFrameRate;

		x264_encoder_reconfig(m_pHandleEncoder, &m_EncoderParam);

	}
	m_nFrameRate = nFrameRate;

	return TRUE;
}

/// 获得帧率
uint16_t CH264Encoder::GetFrameRate(void) const
{
	return m_nFrameRate;
}

/// 编码
int32_t CH264Encoder::Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
		char* pDestBuffer, uint32_t nDestBufferSize)
{
	int32_t nRet = 0;
	if(NULL != m_pHandleEncoder)
	{
		int i_nal;
		x264_nal_t* nal;

		// RGB2YUV
		rgb_to_yuv420(m_EncoderParam.i_width, m_EncoderParam.i_height, (uint8_t*)pSrcBuffer, 
			m_inPicture.img.plane[0], m_inPicture.img.plane[1], m_inPicture.img.plane[2], 1);

		//rgb_to_yv12_mmx((uint8_t*)pSrcBuffer, m_EncoderParam.i_width * 3, 
		//	inPicture.img.plane[0], inPicture.img.plane[1], inPicture.img.plane[2], 
		//	m_EncoderParam.i_width, m_EncoderParam.i_width >> 1, 
		//	m_EncoderParam.i_width, m_EncoderParam.i_height, 0);

		////写yuv文件到本地-->为了测试
		//FILE* fp = fopen("e:\\3.yuv", "wb");
		//fwrite(inPicture.img.plane[0],1, m_EncoderParam.i_width * m_EncoderParam.i_height,fp); 
		//fwrite(inPicture.img.plane[1],1, m_EncoderParam.i_width * m_EncoderParam.i_height/4,fp); 
		//fwrite(inPicture.img.plane[2],1, m_EncoderParam.i_width * m_EncoderParam.i_height/4,fp); 
		//fflush(fp);
		//fclose(fp);

		int nResult = x264_encoder_encode(m_pHandleEncoder, &nal, &i_nal, 
			&m_inPicture, &m_outPicture);
		if(nResult >= 0)
		{
			char* pDstBuff = pDestBuffer;
			for(int i = 0; i < i_nal; i++ )
			{
				int i_size;
				int i_data;

				i_data = nDestBufferSize;
				if(( i_size = x264_nal_encode(pDstBuff, &i_data, 1, &nal[i] ) ) > 0 )
				{
					nRet += i_size;
					if(nRet > nDestBufferSize) 
					{
						return 0;
					}
					pDstBuff += i_data;
				}
				else if( i_size < 0 )
				{
					
				}
			}
		}
	}
	return nRet;
}

//=============================================================================
// class CH264Decoder
CH264Decoder::CH264Decoder(void)
	: m_hHandleDecoder(NULL)
	, m_pH264Context(NULL)
	, m_pVideoFrame(NULL)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
{
	avcodec_init();
}


CH264Decoder::~CH264Decoder(void)
{
	Destroy();
}

/// 创建
BOOL CH264Decoder::Create(void)
{
	if(NULL != m_pH264Context)
		return FALSE;

	m_hHandleDecoder = &h264_decoder;
	if(m_hHandleDecoder == NULL)
	{
		return FALSE;
	}

	m_pH264Context = avcodec_alloc_context();
	m_pH264Context->error_resilience = 0;
	m_pH264Context->flags |= CODEC_FLAG_EMU_EDGE;
	m_pVideoFrame = avcodec_alloc_frame();

	if (avcodec_open(m_pH264Context, &h264_decoder) < 0) 
	{
		return FALSE;
	}

	H264Context* pH264Context = (H264Context*)m_pH264Context->priv_data;
	MpegEncContext* pMpegContext = &pH264Context->s;
	pMpegContext->dsp.idct_permutation_type =1;		

	dsputil_init(&pMpegContext->dsp, m_pH264Context);
	return TRUE;
}

/// 销毁
void CH264Decoder::Destroy(void)
{
	if (m_pH264Context)
	{
		if (m_pH264Context->intra_matrix) 
			av_free(m_pH264Context->intra_matrix);

		if (m_pH264Context->inter_matrix) 
			av_free(m_pH264Context->inter_matrix);

		if (m_pH264Context->slice_offset) 
			av_free(m_pH264Context->slice_offset);

		if (m_pH264Context->codec) 
			avcodec_close(m_pH264Context);

		av_free(m_pH264Context);
		m_pH264Context = NULL;

	}	

	if(NULL != m_pVideoFrame)
	{
		av_free(m_pVideoFrame);
		m_pVideoFrame = NULL;
	}

	if(NULL != m_hHandleDecoder)
	{
		m_hHandleDecoder = NULL;
	}
}

/// 设置视频帧信息
BOOL CH264Decoder::SetFrameInfo(uint16_t nVideoWidth, uint16_t nVideoHeight)
{
	m_nVideoWidth = nVideoWidth;
	m_nVideoHeight = nVideoHeight;
	return TRUE;
}

/// 获得视频帧信息
BOOL CH264Decoder::GetFrameInfo(uint16_t& nVideoWidth, uint16_t& nVideoHeight) const
{
	nVideoWidth = m_nVideoWidth;
	nVideoHeight = m_nVideoHeight;
	return TRUE;
}

/// 解码
int32_t CH264Decoder::Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	int32_t nResult = 0;
	int32_t nDecodeSize = 0;
	int nGotPictuer = 0;

	nDecodeSize = avcodec_decode_video(m_pH264Context, m_pVideoFrame, &nGotPictuer,
		(unsigned char *)pSrcBuffer, nSrcBuffSize);

	if(nDecodeSize >= 0)
	{
		BOOL bIsKeyFrame = FALSE;
		if(nGotPictuer)
		{
			switch(m_pVideoFrame->pict_type)
			{
			case FF_I_TYPE:
				bIsKeyFrame = TRUE;
				break;
			case FF_P_TYPE:
				bIsKeyFrame = FALSE;
				break;
			default:				
				break;
			}

			int nImageSize = m_pH264Context->width * m_pH264Context->height * 3;
			if (nDestBufferSize >= nImageSize)
			{
				////写yuv文件到本地-->为了测试
				//FILE* fp = fopen("e:\\2.yuv", "wb");
				//fwrite(m_pVideoFrame->data[0],1, m_pH264Context->width * m_pH264Context->height,fp); 
				//fwrite(m_pVideoFrame->data[1],1, m_pH264Context->width * m_pH264Context->height/4,fp); 
				//fwrite(m_pVideoFrame->data[2],1, m_pH264Context->width * m_pH264Context->height/4,fp); 
				//fflush(fp);
				//fclose(fp);

				// YUVTORGB
				yuv420_to_rgb(m_pH264Context->width, -m_pH264Context->height, 
					(uint8_t*)pDestBuffer, m_pH264Context->width * 3, 
					m_pVideoFrame->data[0], m_pH264Context->width, 
					m_pVideoFrame->data[1], m_pVideoFrame->data[2], m_pH264Context->width >> 1);


				//yv12_to_rgb_c((uint8_t*)pDestBuffer, m_pH264Context->width * 3, 
				//	m_pVideoFrame->data[0], m_pVideoFrame->data[1], m_pVideoFrame->data[2], 
				//	m_pH264Context->width, m_pH264Context->width >> 1 , 
				//	m_pH264Context->width, m_pH264Context->height, 0);

				nResult = nImageSize;
			}
		}
	}
	return nResult;
}