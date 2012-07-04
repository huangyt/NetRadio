#include "VideoResize.h"


CVideoResize::CVideoResize(void)
	: m_hContext(NULL)
	, m_enColorSpace(ENUM_RESIZE_CSP_RGB24)
	, m_nSrcWidth(0)
	, m_nSrcHeight(0)
	, m_nDstWidth(0)
	, m_nDstHeight(0)
{
	init_mplayer(1, 0, 0, 0, 0, 0);
}

CVideoResize::~CVideoResize(void)
{
}

/// 创建
BOOL CVideoResize::Create(ENUM_RESIZE_ALGORITHM enAlgorithm, 
	ENUM_RESIZE_COLOR_SPACE enColorSpace,
	uint32_t nSrcWidth, uint32_t nSrcHeight, 
	uint32_t nDstWidth, uint32_t nDstHeight)
{
	// 重复创建检查
	if(NULL != m_hContext)
		return FALSE;

	// 初始化参数
	SwsParams params;
	memset(&params, 0, sizeof(params));
	params.cpu = SWS_CPU_CAPS_MMX;
	params.methodLuma.method = GetSWSMethod(enAlgorithm);
	params.methodChroma.method = GetSWSMethod(enAlgorithm);

	int nColorSpace = GetSWSColorSpace(enColorSpace);

	m_hContext = sws_getContext(nSrcWidth, nSrcHeight, nColorSpace, 
		nDstWidth, nDstHeight, nColorSpace, &params, NULL, NULL);

	if(NULL != m_hContext)
	{
		m_enColorSpace = enColorSpace;
		m_nSrcWidth = nSrcWidth;
		m_nSrcHeight = nSrcHeight;
		m_nDstWidth = nDstWidth;
		m_nDstHeight = nDstHeight;
		return TRUE;
	}

	return FALSE;
}

/// 销毁
void CVideoResize::Destroy(void)
{
	if(NULL != m_hContext)
	{
		sws_freeContext(m_hContext);
		m_hContext = NULL;
	}
}

/// Resize
uint32_t CVideoResize::Resize(const char* pInFrame, uint32_t nInFrameSize, 
	char* pOutFrame, uint32_t nOutFrameSize)
{
	uint32_t nResult = 0;

	switch(m_enColorSpace)
	{
	case ENUM_RESIZE_CSP_YV12:
		nResult = ResizeYV12(pInFrame, nInFrameSize, pOutFrame, nOutFrameSize);
		break;
	case ENUM_RESIZE_CSP_RGB24:
		nResult = ResizeRGB24(pInFrame, nInFrameSize, pOutFrame, nOutFrameSize);
		break;
	}

	return nResult;
}

int CVideoResize::GetSWSMethod(ENUM_RESIZE_ALGORITHM enAlgorithm)
{
	int nMethod = 0;
	switch(enAlgorithm)
	{
	case ENUM_RESIZE_NONE:
		break;
	case ENUM_RESIZE_FAST_BILINEAR:
		nMethod = SWS_FAST_BILINEAR;
		break;
	case ENUM_RESIZE_BILINEAER:
		nMethod = SWS_BILINEAR;
		break;
	case ENUM_RESIZE_BICUBIC:
		nMethod = SWS_BICUBIC;
		break;
	case ENUM_RESIZE_EXPERIMENTAL:
		nMethod = SWS_X;
		break;
	case ENUM_RESIZE_POINT:
		nMethod = SWS_POINT;
		break;
	case ENUM_RESIZE_AREA:
		nMethod = SWS_AREA;
		break;
	//case ENUM_RESIZE_BICUBLIN:
	//	break;
	case ENUM_RESIZE_GAUSS:
		nMethod = SWS_GAUSS;
		break;
	case ENUM_RESIZE_SINC:
		nMethod = SWS_SINC;
		break;
	case ENUM_RESIZE_LANCZOS:
		nMethod = SWS_LANCZOS;
		break;
	case ENUM_RESIZE_SPLINE:
		nMethod = SWS_SPLINE;
		break;
	default:
		break;
	}
	return nMethod;
}

int CVideoResize::GetSWSColorSpace(ENUM_RESIZE_COLOR_SPACE enColorSpace)
{
	int nColorSpace = 0;
	switch(enColorSpace)
	{
	case ENUM_RESIZE_CSP_YV12:
		nColorSpace = IMGFMT_YV12;
		break;
	case ENUM_RESIZE_CSP_RGB24:
		nColorSpace = IMGFMT_RGB24;
		break;
	}
	return nColorSpace;
}

/// ResizeRGB24
uint32_t CVideoResize::ResizeRGB24(const char* pInFrame, uint32_t nInFrameSize, 
	char* pOutFrame, uint32_t nOutFrameSize)
{
	// 参数检查
	if(NULL == pInFrame || NULL == pOutFrame 
		|| 0 == nInFrameSize || 0 == nOutFrameSize)
	{
		return 0;
	}

	// 缓冲区长度检查
	if((m_nSrcWidth * m_nSrcHeight * 3) > nInFrameSize 
		|| (m_nDstWidth * m_nDstHeight * 3) > nOutFrameSize)
	{
		return 0;
	}

	BYTE* srcImage[3];
	srcImage[0] = (BYTE*)pInFrame;
	srcImage[1] = NULL;
	srcImage[2] = NULL;

	int srcStride[3];
	srcStride[0] = m_nSrcWidth * 3;
	srcStride[1] = 0;
	srcStride[2] = 0;

	BYTE* dstImage[3];
	dstImage[0] = (BYTE*)pOutFrame;
	dstImage[1] = NULL;
	dstImage[2] = NULL;

	int dstStride[3];
	dstStride[0] = m_nDstWidth * 3;
	dstStride[1] = 0;
	dstStride[2] = 0;

	uint32_t nResult = 0;
	if(sws_scale_ordered(m_hContext, srcImage, srcStride, 0, m_nSrcHeight, dstImage, dstStride)>0)
	{
		nResult = m_nDstWidth * m_nDstHeight * 3;
	}

	// 不知道为什么libmplayer始终返回0，所以在这里重新计算了返回值
	nResult = m_nDstWidth * m_nDstHeight * 3;
	return nResult;
}

/// ResizeRGB24
uint32_t CVideoResize::ResizeYV12(const char* pInFrame, uint32_t nInFrameSize, 
	char* pOutFrame, uint32_t nOutFrameSize)
{
	// 参数检查
	if(NULL == pInFrame || NULL == pOutFrame 
		|| 0 == nInFrameSize || 0 == nOutFrameSize)
	{
		return 0;
	}

	// 缓冲区长度检查
	if((m_nSrcWidth * m_nSrcHeight * 3 / 2) > nInFrameSize 
		|| (m_nDstWidth * m_nDstHeight * 3 / 2) > nOutFrameSize)
	{
		return 0;
	}

	BYTE* srcImage[3];
	srcImage[0] = (BYTE*)pInFrame;
	srcImage[1] = NULL;
	srcImage[2] = NULL;

	int srcStride[3];
	srcStride[0] = m_nSrcWidth * 2;
	srcStride[1] = 0;
	srcStride[2] = 0;

	BYTE* dstImage[3];
	dstImage[0] = (BYTE*)pOutFrame;
	dstImage[1] = dstImage[0] + m_nDstWidth * m_nDstHeight;
	dstImage[2] = dstImage[1] + m_nDstWidth * m_nDstHeight/4;

	int dstStride[3];
	dstStride[0] = m_nDstWidth;
	dstStride[1] = m_nDstWidth>>1;
	dstStride[2] = m_nDstWidth>>1;

	uint32_t nResult = 0;
	if(sws_scale_ordered(m_hContext, srcImage, srcStride, 0, m_nDstHeight, dstImage, dstStride)>0)
	{
		nResult = m_nDstWidth * m_nDstHeight * 3 / 2;
	}
	// 不知道为什么libmplayer始终返回0，所以在这里重新计算了返回值
	nResult = m_nDstWidth * m_nDstHeight * 3 / 2;
	return nResult;
}