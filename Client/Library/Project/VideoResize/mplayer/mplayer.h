#ifndef __MPLAYER_H__
#define __MPLAYER_H__

#include "postproc\\swscale.h"

#ifndef _LIBMPLAYER_EXPORTS
#ifdef _DEBUG
#pragma comment(lib,"libMPlayerD.lib")
#pragma message("LINK libMPlayerD.lib")
#else
#pragma comment(lib,"libMPlayer.lib")
#pragma message("LINK libMPlayer.lib")
#endif //_DEBUG
#endif //_LIBMPLAYER_EXPORTS

#ifdef __cplusplus
extern "C" {
#endif

void init_mplayer(int mmx,int mmx2,int _3dnow,int _3dnowExt,int sse,int sse2);

#ifdef __cplusplus
}
#endif

#endif //__MPLAYER_H__