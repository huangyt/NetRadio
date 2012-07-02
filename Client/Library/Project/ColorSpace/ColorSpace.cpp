#include "StdAfx.h"
#include "ColorSpace.h"

//=============================================================================
BOOL CColorSpaceYV12::m_bIsInit = FALSE;
colorspace_to_yv12_ptr CColorSpaceYV12::m_ToYv12FuncPtr[END_OF_TO_YV12] = {0};
yv12_to_colorspace_ptr CColorSpaceYV12::m_Yv12ToFuncPtr[END_OF_YV12_TO] = {0};

//=============================================================================
CColorSpaceYV12::CColorSpaceYV12(void)
{
}


CColorSpaceYV12::~CColorSpaceYV12(void)
{
}

/// 创建
BOOL CColorSpaceYV12::Create(void)
{
	if(!m_bIsInit)
	{
		colorspace_init();
		m_bIsInit = TRUE;

		m_ToYv12FuncPtr[RGB555_TO_YV12] = rgb555_to_yv12;
		m_ToYv12FuncPtr[RGB565_TO_YV12] = rgb565_to_yv12;
		m_ToYv12FuncPtr[RGB_TO_YV12] = rgb_to_yv12;
		m_ToYv12FuncPtr[BGR_TO_YV12] = bgr_to_yv12;
		m_ToYv12FuncPtr[BGRA_TO_YV12] = bgra_to_yv12;
		m_ToYv12FuncPtr[ABGR_TO_YV12] = abgr_to_yv12;
		m_ToYv12FuncPtr[RGBA_TO_YV12] = rgba_to_yv12;
		m_ToYv12FuncPtr[ARGB_TO_YV12] = argb_to_yv12;
		m_ToYv12FuncPtr[YUYV_TO_YV12] = yuyv_to_yv12;
		m_ToYv12FuncPtr[UYVY_TO_YV12] = uyvy_to_yv12;

		m_ToYv12FuncPtr[RGB555I_TO_YV12] = rgb555i_to_yv12;
		m_ToYv12FuncPtr[RGB565I_TO_YV12] = rgb565i_to_yv12;
		m_ToYv12FuncPtr[RGBI_TO_YV12] = rgbi_to_yv12;
		m_ToYv12FuncPtr[BGRI_TO_YV12] = bgri_to_yv12;
		m_ToYv12FuncPtr[BGRAI_TO_YV12] = bgrai_to_yv12;
		m_ToYv12FuncPtr[ABGRI_TO_YV12] = abgri_to_yv12;
		m_ToYv12FuncPtr[RGBAI_TO_YV12] = rgbai_to_yv12;
		m_ToYv12FuncPtr[ARGBI_TO_YV12] = argbi_to_yv12;
		m_ToYv12FuncPtr[YUYVI_TO_YV12] = yuyvi_to_yv12;
		m_ToYv12FuncPtr[UYVYI_TO_YV12] = uyvyi_to_yv12;

		m_Yv12ToFuncPtr[YV12_TO_RGB555] = yv12_to_rgb555;
		m_Yv12ToFuncPtr[YV12_TO_RGB565] = yv12_to_rgb565;
		m_Yv12ToFuncPtr[YV12_TO_RGB] = yv12_to_rgb;
		m_Yv12ToFuncPtr[YV12_TO_BGR] = yv12_to_bgr;
		m_Yv12ToFuncPtr[YV12_TO_BGRA] = yv12_to_bgra;
		m_Yv12ToFuncPtr[YV12_TO_ABGR] = yv12_to_abgr;
		m_Yv12ToFuncPtr[YV12_TO_RGBA] = yv12_to_rgba;
		m_Yv12ToFuncPtr[YV12_TO_ARGB] = yv12_to_argb;
		m_Yv12ToFuncPtr[YV12_TO_YUYV] = yv12_to_yuyv;
		m_Yv12ToFuncPtr[YV12_TO_UYVY] = yv12_to_uyvy;

		m_Yv12ToFuncPtr[YV12_TO_RGB555I] = yv12_to_rgb555i;
		m_Yv12ToFuncPtr[YV12_TO_RGB565I] = yv12_to_rgb565i;
		m_Yv12ToFuncPtr[YV12_TO_RGBI] = yv12_to_rgbi;
		m_Yv12ToFuncPtr[YV12_TO_BGRI] = yv12_to_bgri;
		m_Yv12ToFuncPtr[YV12_TO_BGRAI] = yv12_to_bgrai;
		m_Yv12ToFuncPtr[YV12_TO_ABGRI] = yv12_to_abgri;
		m_Yv12ToFuncPtr[YV12_TO_RGBAI] = yv12_to_rgbai;
		m_Yv12ToFuncPtr[YV12_TO_ARGBI] = yv12_to_argbi;
		m_Yv12ToFuncPtr[YV12_TO_YUYVI] = yv12_to_yuyvi;
		m_Yv12ToFuncPtr[YV12_TO_UYVYI] = yv12_to_uyvyi;

	}
	return TRUE;
}

/// 释放
void CColorSpaceYV12::Destroy(void)
{

}

/// 其他色彩空间转换至YV12
BOOL CColorSpaceYV12::ColorSpaceToYV12(TO_YV12_TYPE enConverType, 
	uint8_t* src_ptr, int src_stride,
	uint8_t* y_dst, uint8_t* v_dst, uint8_t* u_dst,
	int y_stride, int uv_stride, int width, int height, int vflip)
{
	if(NULL != m_ToYv12FuncPtr[enConverType])
	{
		m_ToYv12FuncPtr[enConverType](src_ptr, src_stride, 
			y_dst, v_dst, u_dst, y_stride, uv_stride, width, height, vflip);
		return TRUE;
	}
	return FALSE;
}

/// YV12转换至其他色彩空间
BOOL CColorSpaceYV12::YV12ToColorSpace(YV12_TO_TYPE enConverType, 
	uint8_t* dst_ptr, int dst_stride,
	uint8_t* y_src, uint8_t* v_src, uint8_t* u_src,
	int y_stride, int uv_stride, int width, int height, int vflip)
{
	if(NULL != m_ToYv12FuncPtr[enConverType])
	{
		m_Yv12ToFuncPtr[enConverType](dst_ptr, dst_stride, 
			y_src, v_src, u_src, y_stride, uv_stride, width, height, vflip);
		return TRUE;
	}
	return FALSE;
}

/// YV12转换至YV12
BOOL CColorSpaceYV12::YV12ToYV12(uint8_t* y_dst, uint8_t* u_dst, uint8_t* v_dst,
	int y_dst_stride, int uv_dst_stride,
	uint8_t* y_src, uint8_t* u_src, uint8_t* v_src,
	int y_src_stride, int uv_src_stride,
	int width, int height, int vflip)
{
	yv12_to_yv12(y_dst, u_dst, v_dst, y_dst_stride, uv_dst_stride, 
		y_src, u_src, v_src, y_src_stride, uv_src_stride, width, height, vflip);
	return TRUE;
}