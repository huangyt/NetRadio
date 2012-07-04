#ifndef SWSCALE_MULTICPU_H
#define SWSCALE_MULTICPU_H

typedef int BOOL;

#ifdef __cplusplus
extern "C" {
#endif

extern int CPUCount;	// Dual core, multiple CPU support
extern int FreeCPUCount;

int GetCPUCount(void);
void decCPUCount(void);
void incCPUCount(void);

#ifdef __cplusplus
}
#endif

#endif
