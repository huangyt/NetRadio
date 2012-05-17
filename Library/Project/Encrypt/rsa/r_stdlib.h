

#ifndef _R_STDLIB_H_
#define _R_STDLIB_H_ 1

#ifdef __cplusplus
extern "C" {
#endif


void R_memset (POINTER output, int value, unsigned int len);


void R_memcpy (POINTER output, POINTER input, unsigned int len);


int R_memcmp (POINTER firstBlock, POINTER secondBlock, unsigned int len);
#ifdef __cplusplus
}
#endif

#endif
