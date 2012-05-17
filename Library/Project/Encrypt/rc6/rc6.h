// RC6.h: interface for the CRC6 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RC6_H__3F6072B4_54DB_47CC_BB8C_1D071046DFC0__INCLUDED_)
#define AFX_RC6_H__3F6072B4_54DB_47CC_BB8C_1D071046DFC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct {
	unsigned long l_key[44];
} rc6_ctx;

//…Ë÷√√‹‘ø
void rc6_set_key(const unsigned long in_key[], const unsigned long key_len,rc6_ctx *cx);

//º”√‹
void rc6_encrypt(const unsigned long in_blk[4], unsigned long out_blk[4],rc6_ctx *cx);

//Ω‚√‹
void rc6_decrypt(const unsigned long in_blk[4], unsigned long out_blk[4],rc6_ctx *cx);

#if defined(__cplusplus)
}
#endif

#endif // !defined(AFX_RC6_H__3F6072B4_54DB_47CC_BB8C_1D071046DFC0__INCLUDED_)
