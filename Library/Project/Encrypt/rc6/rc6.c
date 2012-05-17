
/* This is an independent implementation of the encryption algorithm:   */
/*                                                                      */
/*         RC6 by Ron Rivest and RSA Labs                               */
/*                                                                      */
/* which is a candidate algorithm in the Advanced Encryption Standard   */
/* programme of the US National Institute of Standards and Technology.  */
/*                                                                      */
/* Copyright in this implementation is held by Dr B R Gladman but I     */
/* hereby give permission for its free direct or derivative use subject */
/* to acknowledgment of its origin and compliance with any conditions   */
/* that the originators of the algorithm place on its exploitation.     */
/*                                                                      */
/* Dr Brian Gladman (gladman@seven77.demon.co.uk) 14th January 1999     */
/*                                                                      */
/* Published                                                            */
/* by Seraph Chutium                                                    */
/* http://com.6to23.com/                                                */

/* Timing data for RC6 (rc6.c)

128 bit key:
Key Setup:    1632 cycles
Encrypt:       270 cycles =    94.8 mbits/sec
Decrypt:       226 cycles =   113.3 mbits/sec
Mean:          248 cycles =   103.2 mbits/sec

192 bit key:
Key Setup:    1885 cycles
Encrypt:       267 cycles =    95.9 mbits/sec
Decrypt:       235 cycles =   108.9 mbits/sec
Mean:          251 cycles =   102.0 mbits/sec

256 bit key:
Key Setup:    1877 cycles
Encrypt:       270 cycles =    94.8 mbits/sec
Decrypt:       227 cycles =   112.8 mbits/sec
Mean:          249 cycles =   103.0 mbits/sec

*/

//#include "../std_defs.h"
#include "rc6.h"

#ifndef _MSC_VER

#define rotr(x,n)   (((x) >> ((int)(n))) | ((x) << (32 - (int)(n))))
#define rotl(x,n)   (((x) << ((int)(n))) | ((x) >> (32 - (int)(n))))

#else

#include <stdlib.h>

#pragma intrinsic(_lrotr,_lrotl)
#define rotr(x,n)   _lrotr(x,n)
#define rotl(x,n)   _lrotl(x,n)

#endif

#define f_rnd(i,a,b,c,d)                    \
        u = rotl(d * (d + d + 1), 5);       \
        t = rotl(b * (b + b + 1), 5);       \
        a = rotl(a ^ t, u) + cx->l_key[i];      \
        c = rotl(c ^ u, t) + cx->l_key[i + 1]

#define i_rnd(i,a,b,c,d)                    \
        u = rotl(d * (d + d + 1), 5);       \
        t = rotl(b * (b + b + 1), 5);       \
        c = rotr(c - cx->l_key[i + 1], t) ^ u;  \
        a = rotr(a - cx->l_key[i], u) ^ t

//unsigned long  l_key[44];  /* storage for the key schedule         */

/* initialise the key schedule from the user supplied key   */

void rc6_set_key(const unsigned long in_key[], const unsigned long key_len,rc6_ctx *cx)
{   
	unsigned long  i, j, k, a, b, l[8], t;

    cx->l_key[0] = 0xb7e15163;

    for(k = 1; k < 44; ++k)
        
        cx->l_key[k] = cx->l_key[k - 1] + 0x9e3779b9;

    for(k = 0; k < key_len / 32; ++k)

        l[k] = in_key[k];

    t = (key_len / 32) - 1; // t = (key_len / 32);

    a = b = i = j = 0;

    for(k = 0; k < 132; ++k)
    {   a = rotl(cx->l_key[i] + a + b, 3); b += a;
        b = rotl(l[j] + b, b);
        cx->l_key[i] = a; l[j] = b;
        i = (i == 43 ? 0 : i + 1); // i = (i + 1) % 44;  
        j = (j == t ? 0 : j + 1);  // j = (j + 1) % t;
    }
};

/* encrypt a block of text  */

void rc6_encrypt(const unsigned long in_blk[4], unsigned long out_blk[4],rc6_ctx *cx)
{  
	unsigned long  a,b,c,d,t,u;

    a = in_blk[0]; b = in_blk[1] + cx->l_key[0];
    c = in_blk[2]; d = in_blk[3] + cx->l_key[1];

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

    out_blk[0] = a + cx->l_key[42]; out_blk[1] = b;
    out_blk[2] = c + cx->l_key[43]; out_blk[3] = d;
};

/* decrypt a block of text  */

void rc6_decrypt(const unsigned long in_blk[4], unsigned long out_blk[4],rc6_ctx *cx)
{   unsigned long  a,b,c,d,t,u;

    d = in_blk[3]; c = in_blk[2] - cx->l_key[43]; 
    b = in_blk[1]; a = in_blk[0] - cx->l_key[42];

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

    out_blk[3] = d - cx->l_key[1]; out_blk[2] = c; 
    out_blk[1] = b - cx->l_key[0]; out_blk[0] = a; 
};
