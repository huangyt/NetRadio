#include "StdAfx.h"
#include "TypeDefine.h"
#include "Mp3Codec.h"

//=============================================================================
CMp3Encoder::CMp3Encoder(void)
	: m_hHandleEncoder(NULL)
{
}

CMp3Encoder::~CMp3Encoder(void)
{
}

/// 创建编码器
BOOL CMp3Encoder::Create(void)
{
	if(NULL != m_hHandleEncoder)
		return FALSE;

	m_hHandleEncoder = lame_init();
	
	return (NULL != m_hHandleEncoder);
}

/// 销毁编码器
void CMp3Encoder::Destroy(void)
{
	if(NULL != m_hHandleEncoder)
	{
		lame_close(m_hHandleEncoder);
		m_hHandleEncoder = NULL;
	}
}

/// 编码
int32_t CMp3Encoder::Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	return lame_encode_buffer_interleaved(m_hHandleEncoder, (short*)pSrcBuffer, 24, 
		(unsigned char*)pDestBuffer, nDestBufferSize);
}

//=============================================================================
CMp3Decoder::CMp3Decoder(void)
	: m_hHandleDecoder(NULL)
{

}

CMp3Decoder::~CMp3Decoder(void)
{

}

/// 创建编码器
BOOL CMp3Decoder::Create(void)
{
	if(NULL != m_hHandleDecoder)
		return FALSE;

	mad_decoder_init(&m_MadDecoder, NULL,
		AudioInput, AudioHeader, AudioFilter, AudioOutput, 
		Error, Message);

	m_hHandleDecoder = &m_MadDecoder;
	return TRUE;
}

/// 销毁编码器
void CMp3Decoder::Destroy(void)
{
	if(NULL != m_hHandleDecoder)
	{
		mad_decoder_finish(&m_MadDecoder);
		m_hHandleDecoder = NULL;
	}
}

/// 编码
int32_t CMp3Decoder::Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	return 0;
}

//=============================================================================
// mad 输入回调函数
enum mad_flow CMp3Decoder::AudioInput(void *data, struct mad_stream *stream)
{
	return MAD_FLOW_CONTINUE;
}

// mad header回调函数
enum mad_flow CMp3Decoder::AudioHeader(void *data, 
	struct mad_header const *header)
{
	return MAD_FLOW_CONTINUE;
}

// mad filter回调函数
enum mad_flow CMp3Decoder::AudioFilter(void *data, 
struct mad_stream const *stream, 
struct mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}

// mad 输出回调函数
enum mad_flow CMp3Decoder::AudioOutput(void *data, 
struct mad_header const *header, 
struct mad_pcm *pcm)
{
	return MAD_FLOW_CONTINUE;
}

// mad 错误回调函数
enum mad_flow CMp3Decoder::Error(void *data, struct mad_stream *stream, 
struct mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}
// mad 消息回调函数
enum mad_flow CMp3Decoder::Message(void *, void *, unsigned int *)
{
	return MAD_FLOW_CONTINUE;
};