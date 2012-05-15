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
/// \file    : Endianness.h
/// \brief   : 字节序宏定义头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-16
///============================================================================

#ifndef __ENDIANNESS_H__
#define __ENDIANNESS_H__

//=============================================================================
static union 
{ 
	char c[4]; 
	unsigned long mylong; 
}endianess = {{ 'l', '?', '?', 'b' }};

#define ENDIANNESS ((char)endianess.mylong)

//=============================================================================
// SWAP16
#define SWAP16(s) 	((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))

// SWAP32
#define SWAP32(l) 	(((l) >> 24) | /
            	 	(((l) & 0x00ff0000) >> 8)  | /
             		(((l) & 0x0000ff00) << 8)  | /
             		((l) << 24))

// SWAP64
#define SWAP64(ll) 	(((ll) >> 56) |/
                    (((ll) & 0x00ff000000000000) >> 40) |/
                    (((ll) & 0x0000ff0000000000) >> 24) |/
                    (((ll) & 0x000000ff00000000) >> 8)  |/
                    (((ll) & 0x00000000ff000000) << 8)  |/
                    (((ll) & 0x0000000000ff0000) << 24) |/
                    (((ll) & 0x000000000000ff00) << 40) |/
                    (((ll) << 56)))

//=============================================================================
//
#define BIG_ENDIAN16(x)		do{ ENDIANNESS == 'b' ? x : SWAP16(x)}while(false)
#define LITTLE_ENDIAN16(x)	do{ ENDIANNESS == 'l' ? x : SWAP16(x)}while(false)
#define BIG_ENDIAN32(x) 	do{ ENDIANNESS == 'b' ? x : SWAP32(x)}while(false)
#define LITTLE_ENDIAN32(x)	do{ ENDIANNESS == 'l' ? x : SWAP32(x)}while(false)
#define BIG_ENDIAN64(x) 	do{ ENDIANNESS == 'b' ? x : SWAP64(x)}while(false)
#define LITTLE_ENDIAN64(x)	do{ ENDIANNESS == 'l' ? x : SWAP64(x)}while(false)


#endif //__ENDIANNESS_H__

