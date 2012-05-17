
/* This is an independent implementation of the RC6 algorithm that  */
/* Ron Rivest and RSA Labs have submitted as a candidate for the    */
/* NIST AES activity.  Refer to RSA Labs and Ron Rivest for any     */
/* copyright, patent or license issues for the RC6 algorithm.       */
/*                                                                  */
/* Copyright in this implementation is held by Dr B R Gladman but   */
/* I hereby give permission for its free direct or derivative use   */
/* subject to acknowledgment of its origin and compliance with any  */
/* constraints that are placed on the exploitation of RC6 by its    */
/* designers.                                                       */
/*                                                                  */
/* This implementation is a reference implementation and is not     */
/* designed for speed.  It follows the pseudo-code in the paper     */
/* describing RC6 closely.  Nevertheless it achieves a speed of     */
/* about 85 Mbits/second on a 200Mhz Pentium Pro                    */
/*                                                                  */
/* Dr Brian Gladman (gladman@seven77.demon.co.uk) 18th July 1998    */
/*                                                                  */
/* Published                                                        */
/* by Seraph Chutium                                                */
/* http://com.6to23.com/                                            */

#include "../std_defs.h"

static char *alg_name = "RC6";

char *cipher_name()
{
    return alg_name;
}

#ifndef _MSC_VER

#define rotr(x,n)   (((x) >> ((int)(n))) | ((x) << (32 - (int)(n))))
#define rotl(x,n)   (((x) << ((int)(n))) | ((x) >> (32 - (int)(n))))

#else

#include <stdlib.h>

#pragma intrinsic(_lrotr,_lrotl)
#define rotr(x,n)   _lrotr(x,n)
#define rotl(x,n)   _lrotl(x,n)

#endif

#define f_rnd(i,a,b,c,d)                                \
        t = rotl(b * (b + b + 1), 5);                   \
        u = rotl(d * (d + d + 1), 5);                   \
        a = rotl(a ^ t, u & 0x0000001f) + l_key[i];     \
        c = rotl(c ^ u, t & 0x0000001f) + l_key[i + 1]  \

#define i_rnd(i,a,b,c,d)                                \
        u = rotl(d * (d + d + 1), 5);                   \
        t = rotl(b * (b + b + 1), 5);                   \
        c = rotr(c - l_key[i + 1], t & 0x0000001f) ^ u; \
        a = rotr(a - l_key[i], u & 0x0000001f) ^ t      \

u4byte  l_key[44];  /* storage for the key schedule         */

/* initialise the key schedule from the user supplied key   */

u4byte *set_key(u4byte key_blk[], u4byte key_len)
{   u4byte  i, j, k, a, b, l[8];

    l_key[0] = 0xb7e15163;

    for(k = 1; k < 44; ++k)
        
        l_key[k] = l_key[k - 1] + 0x9e3779b9;

    for(k = 0; k < key_len; ++k)

        l[k] = key_blk[k];

    for(a = b = k = 0; k < 132; ++k)
    {
        i = k % 44; j = k % key_len;
        a = rotl(l_key[i] + a + b, 3); b += a;
        b = rotl(l[j] + b, b & 0x0000001f);
        l_key[i] = a; l[j] = b;
    }

    return l_key;
};

/* encrypt a block of text  */

void encrypt(u4byte in_blk[], u4byte out_blk[])
{   u4byte  a,b,c,d,t,u;

    a = in_blk[0]; b = in_blk[1] + l_key[0];
    c = in_blk[2]; d = in_blk[3] + l_key[1];

    f_rnd( 2,a,b,c,d); f_rnd( 4,b,c,d,a);
    f_rnd( 6,c,d,a,b); f_rnd( 8,d,a,b,c);
    f_rnd(10,a,b,c,d); f_rnd(12,b,c,d,a);
    f_rnd(14,c,d,a,b); f_rnd(16,d,a,b,c);
    f_rnd(18,a,b,c,d); f_rnd(20,b,c,d,a);
    f_rnd(22,c,d,a,b); f_rnd(24,d,a,b,c);
    f_rnd(26,a,b,c,d); f_rnd(28,b,c,d,a);
    f_rnd(30,c,d,a,b); f_rnd(32,d,a,b,c);
    f_rnd(34,a,b,c,d); f_rnd(36,b,c,d,a);
    f_rnd(38,c,d,a,b); f_rnd(40,d,a,b,c);

    out_blk[0] = a + l_key[42]; out_blk[1] = b;
    out_blk[2] = c + l_key[43]; out_blk[3] = d;
};

/* decrypt a block of text  */

void decrypt(u4byte in_blk[], u4byte out_blk[])
{   u4byte  a,b,c,d,t,u;

    d = in_blk[3]; c = in_blk[2] - l_key[43]; 
    b = in_blk[1]; a = in_blk[0] - l_key[42];

    i_rnd(40,d,a,b,c); i_rnd(38,c,d,a,b);
    i_rnd(36,b,c,d,a); i_rnd(34,a,b,c,d);
    i_rnd(32,d,a,b,c); i_rnd(30,c,d,a,b);
    i_rnd(28,b,c,d,a); i_rnd(26,a,b,c,d);
    i_rnd(24,d,a,b,c); i_rnd(22,c,d,a,b);
    i_rnd(20,b,c,d,a); i_rnd(18,a,b,c,d);
    i_rnd(16,d,a,b,c); i_rnd(14,c,d,a,b);
    i_rnd(12,b,c,d,a); i_rnd(10,a,b,c,d);
    i_rnd( 8,d,a,b,c); i_rnd( 6,c,d,a,b);
    i_rnd( 4,b,c,d,a); i_rnd( 2,a,b,c,d);

    out_blk[3] = d - l_key[1]; out_blk[2] = c; 
    out_blk[1] = b - l_key[0]; out_blk[0] = a; 
};
