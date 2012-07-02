/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : ColorSpaceYV12.h
/// \brief   : YV12É«²Ê¿Õ¼äº¯Êý
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-07-02
///============================================================================
#ifndef __COLOR_SPACE_YV12_H__
#define __COLOR_SPACE_YV12_H__

#include "TypeDefine.h"

//=============================================================================
#ifdef __cplusplus
extern "C"
{
#endif
	/* colorspace conversion function*/
	typedef void (colorspace_to_yv12) (uint8_t* x_ptr, int x_stride,
		uint8_t* y_src, uint8_t* v_src, uint8_t* u_src,
		int y_stride, int uv_stride, int width, int height, int vflip);

	typedef void (yv12_to_colorspace) (uint8_t* x_ptr, int x_stride,
		uint8_t* y_src, uint8_t* v_src, uint8_t* u_src,
		int y_stride, int uv_stride, int width, int height, int vflip);

	typedef void (planar_conver) (
		uint8_t * y_dst, uint8_t * u_dst, uint8_t * v_dst,
		int y_dst_stride, int uv_dst_stride,
		uint8_t * y_src, uint8_t * u_src, uint8_t * v_src,
		int y_src_stride, int uv_src_stride,
		int width, int height, int vflip);

	typedef colorspace_to_yv12* colorspace_to_yv12_ptr;
	typedef yv12_to_colorspace* yv12_to_colorspace_ptr;
	typedef planar_conver* planar_conver_ptr;

	/* initialize tables */
	void colorspace_init(void);

	/* initialize colorspace function pointer */
	void convert_func_ptr_init(void);

	/* xxx_to_yv12 colorspace conversion functions (encoder) */
	extern colorspace_to_yv12_ptr rgb555_to_yv12;
	extern colorspace_to_yv12_ptr rgb565_to_yv12;
	extern colorspace_to_yv12_ptr rgb_to_yv12;
	extern colorspace_to_yv12_ptr bgr_to_yv12;
	extern colorspace_to_yv12_ptr bgra_to_yv12;
	extern colorspace_to_yv12_ptr abgr_to_yv12;
	extern colorspace_to_yv12_ptr rgba_to_yv12;
	extern colorspace_to_yv12_ptr argb_to_yv12;
	extern colorspace_to_yv12_ptr yuyv_to_yv12;
	extern colorspace_to_yv12_ptr uyvy_to_yv12;

	extern colorspace_to_yv12_ptr rgb555i_to_yv12;
	extern colorspace_to_yv12_ptr rgb565i_to_yv12;
	extern colorspace_to_yv12_ptr rgbi_to_yv12;
	extern colorspace_to_yv12_ptr bgri_to_yv12;
	extern colorspace_to_yv12_ptr bgrai_to_yv12;
	extern colorspace_to_yv12_ptr abgri_to_yv12;
	extern colorspace_to_yv12_ptr rgbai_to_yv12;
	extern colorspace_to_yv12_ptr argbi_to_yv12;
	extern colorspace_to_yv12_ptr yuyvi_to_yv12;
	extern colorspace_to_yv12_ptr uyvyi_to_yv12;

	/* plain c */
	colorspace_to_yv12 rgb555_to_yv12_c;
	colorspace_to_yv12 rgb565_to_yv12_c;
	colorspace_to_yv12 rgb_to_yv12_c;
	colorspace_to_yv12 bgr_to_yv12_c;
	colorspace_to_yv12 bgra_to_yv12_c;
	colorspace_to_yv12 abgr_to_yv12_c;
	colorspace_to_yv12 rgba_to_yv12_c;
	colorspace_to_yv12 argb_to_yv12_c;
	colorspace_to_yv12 yuyv_to_yv12_c;
	colorspace_to_yv12 uyvy_to_yv12_c;

	colorspace_to_yv12 rgb555i_to_yv12_c;
	colorspace_to_yv12 rgb565i_to_yv12_c;
	colorspace_to_yv12 rgbi_to_yv12_c;
	colorspace_to_yv12 bgri_to_yv12_c;
	colorspace_to_yv12 bgrai_to_yv12_c;
	colorspace_to_yv12 abgri_to_yv12_c;
	colorspace_to_yv12 rgbai_to_yv12_c;
	colorspace_to_yv12 argbi_to_yv12_c;
	colorspace_to_yv12 yuyvi_to_yv12_c;
	colorspace_to_yv12 uyvyi_to_yv12_c;

#if defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64)
	/* mmx */
	colorspace_to_yv12 bgr_to_yv12_mmx;
	colorspace_to_yv12 rgb_to_yv12_mmx;
	colorspace_to_yv12 bgra_to_yv12_mmx;
	colorspace_to_yv12 rgba_to_yv12_mmx;
	colorspace_to_yv12 yuyv_to_yv12_mmx;
	colorspace_to_yv12 uyvy_to_yv12_mmx;

	/* 3dnow */
	colorspace_to_yv12 yuyv_to_yv12_3dn;
	colorspace_to_yv12 uyvy_to_yv12_3dn;

	/* xmm */
	colorspace_to_yv12 yuyv_to_yv12_xmm;
	colorspace_to_yv12 uyvy_to_yv12_xmm;
#endif

#ifdef ARCH_IS_PPC
	colorspace_to_yv12 bgra_to_yv12_altivec_c;
	colorspace_to_yv12 abgr_to_yv12_altivec_c;
	colorspace_to_yv12 rgba_to_yv12_altivec_c;
	colorspace_to_yv12 argb_to_yv12_altivec_c;

	colorspace_to_yv12 yuyv_to_yv12_altivec_c;
	colorspace_to_yv12 uyvy_to_yv12_altivec_c;
#endif

	/* yv12_to_xxx colorspace conversion functions (decoder) */

	extern yv12_to_colorspace_ptr yv12_to_rgb555;
	extern yv12_to_colorspace_ptr yv12_to_rgb565;
	extern yv12_to_colorspace_ptr yv12_to_rgb;
	extern yv12_to_colorspace_ptr yv12_to_bgr;
	extern yv12_to_colorspace_ptr yv12_to_bgra;
	extern yv12_to_colorspace_ptr yv12_to_abgr;
	extern yv12_to_colorspace_ptr yv12_to_rgba;
	extern yv12_to_colorspace_ptr yv12_to_argb;
	extern yv12_to_colorspace_ptr yv12_to_yuyv;
	extern yv12_to_colorspace_ptr yv12_to_uyvy;

	extern yv12_to_colorspace_ptr yv12_to_rgb555i;
	extern yv12_to_colorspace_ptr yv12_to_rgb565i;
	extern yv12_to_colorspace_ptr yv12_to_rgbi;
	extern yv12_to_colorspace_ptr yv12_to_bgri;
	extern yv12_to_colorspace_ptr yv12_to_bgrai;
	extern yv12_to_colorspace_ptr yv12_to_abgri;
	extern yv12_to_colorspace_ptr yv12_to_rgbai;
	extern yv12_to_colorspace_ptr yv12_to_argbi;
	extern yv12_to_colorspace_ptr yv12_to_yuyvi;
	extern yv12_to_colorspace_ptr yv12_to_uyvyi;

	/* plain c */
	yv12_to_colorspace yv12_to_rgb555_c;
	yv12_to_colorspace yv12_to_rgb565_c;
	yv12_to_colorspace yv12_to_rgb_c;
	yv12_to_colorspace yv12_to_bgr_c;
	yv12_to_colorspace yv12_to_bgra_c;
	yv12_to_colorspace yv12_to_abgr_c;
	yv12_to_colorspace yv12_to_rgba_c;
	yv12_to_colorspace yv12_to_argb_c;
	yv12_to_colorspace yv12_to_yuyv_c;
	yv12_to_colorspace yv12_to_uyvy_c;

	yv12_to_colorspace yv12_to_rgb555i_c;
	yv12_to_colorspace yv12_to_rgb565i_c;
	yv12_to_colorspace yv12_to_rgbi_c;
	yv12_to_colorspace yv12_to_bgri_c;
	yv12_to_colorspace yv12_to_bgrai_c;
	yv12_to_colorspace yv12_to_abgri_c;
	yv12_to_colorspace yv12_to_rgbai_c;
	yv12_to_colorspace yv12_to_argbi_c;
	yv12_to_colorspace yv12_to_yuyvi_c;
	yv12_to_colorspace yv12_to_uyvyi_c;

#if defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64)
	/* mmx */
	yv12_to_colorspace yv12_to_bgr_mmx;
	yv12_to_colorspace yv12_to_bgra_mmx;
	yv12_to_colorspace yv12_to_yuyv_mmx;
	yv12_to_colorspace yv12_to_uyvy_mmx;

	yv12_to_colorspace yv12_to_yuyvi_mmx;
	yv12_to_colorspace yv12_to_uyvyi_mmx;
#endif

#ifdef ARCH_IS_PPC
	yv12_to_colorspace yv12_to_yuyv_altivec_c;
	yv12_to_colorspace yv12_to_uyvy_altivec_c;
#endif

	extern planar_conver_ptr yv12_to_yv12;

	planar_conver yv12_to_yv12_c;

#if defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64)
	planar_conver yv12_to_yv12_mmx;
	planar_conver yv12_to_yv12_xmm;
#endif

#ifdef __cplusplus
}
#endif

#endif //__COLOR_SPACE_YV12_H__
