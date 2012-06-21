#include "StdAfx.h"
#include "XvidCodec.h"

/// 使用汇编
#define XVID_USE_ASSEMBLER	1

/// 计算步长
#define CALC_BI_STRIDE(width, bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)

/* Maximum number of frames to encode */
#define ABS_MAXFRAMENR -1 /* no limit */

//=============================================================================
typedef struct
{
	int frame;

	int type;
	int mode;
	int modifier;

	unsigned int greyscale;
	unsigned int chroma_opt;
	unsigned int bvop_threshold;
	unsigned int cartoon_mode;
} zone_t;

#define MAX_ZONES			64
static zone_t	ZONES[MAX_ZONES];
static 	int		NUM_ZONES = 0;

static 	int		ARG_NUM_APP_THREADS = 1;
static 	int		ARG_CPU_FLAGS = 0;
static 	int		ARG_STATS = 0;
static 	int		ARG_SSIM = -1;
static 	int		ARG_PSNRHVSM = 0;
static 	char*	ARG_SSIM_PATH = NULL;
static 	int		ARG_DUMP = 0;
static 	int		ARG_LUMIMASKING = 0;
static 	int		ARG_BITRATE = 0;
static 	int		ARG_TARGETSIZE = 0;
static 	int		ARG_SINGLE = 1;
static 	char*	ARG_PASS1 = 0;
static 	char*	ARG_PASS2 = 0;
//static int ARG_QUALITY = ME_ELEMENTS - 1;
static 	int		ARG_QUALITY = 6;
static 	float	ARG_FRAMERATE = 0.f;
static 	int		ARG_DWRATE = 25;
static 	int		ARG_DWSCALE = 1;
static 	int		ARG_MAXFRAMENR = ABS_MAXFRAMENR;
static 	int		ARG_MAXKEYINTERVAL = 300;
static 	int		ARG_STARTFRAMENR = 0;
static 	char*	ARG_INPUTFILE = NULL;
static 	int		ARG_INPUTTYPE = 0;
static 	int		ARG_SAVEMPEGSTREAM = 0;
static 	int		ARG_SAVEINDIVIDUAL = 0;
static 	char*	ARG_OUTPUTFILE = NULL;
static 	char*	ARG_AVIOUTPUTFILE = NULL;
static 	char*	ARG_MKVOUTPUTFILE = NULL;
static 	char*	ARG_TIMECODEFILE = NULL;
static 	int		ARG_BQRATIO = 150;
static 	int		ARG_BQOFFSET = 100;
static	int		ARG_MAXBFRAMES = 0;
static 	int		ARG_PACKED = 1;
static 	int		ARG_DEBUG = 0;
static 	int		ARG_VOPDEBUG = 0;
static 	int		ARG_TRELLIS = 1;
static 	int		ARG_QTYPE = 0;
static 	int		ARG_QMATRIX = 0;
static 	int		ARG_GMC = 0;
static 	int		ARG_INTERLACING = 0;
static 	int		ARG_QPEL = 0;
static 	int		ARG_TURBO = 0;
static 	int		ARG_VHQMODE = 1;
static 	int		ARG_BVHQ = 0;
static 	int		ARG_QMETRIC = 0;
static 	int		ARG_CLOSED_GOP = 1;
static 	int		ARG_CHROMAME = 1;
static 	int		ARG_PAR = 1;
static 	int		ARG_PARHEIGHT;
static 	int		ARG_PARWIDTH;
static 	int		ARG_QUANTS[6] = {2, 31, 2, 31, 2, 31};
static 	int		ARG_FRAMEDROP = 0;
static 	double	ARG_CQ = 0;
static 	int		ARG_FULL1PASS = 0;
static 	int		ARG_REACTION = 16;
static 	int		ARG_AVERAGING = 100;
static 	int		ARG_SMOOTHER = 100;
static 	int		ARG_KBOOST = 10;
static 	int		ARG_KREDUCTION = 20;
static 	int		ARG_KTHRESH = 1;
static  int		ARG_CHIGH = 0;
static 	int		ARG_CLOW = 0;
static 	int		ARG_OVERSTRENGTH = 5;
static 	int		ARG_OVERIMPROVE = 5;
static 	int		ARG_OVERDEGRADE = 5;
static 	int		ARG_OVERHEAD = 0;
static 	int		ARG_VBVSIZE = 0;
static 	int		ARG_VBVMAXRATE = 0;
static 	int		ARG_VBVPEAKRATE = 0;
static 	int		ARG_THREADS = 0;
static 	int		ARG_SLICES = 1;
static 	int		ARG_VFR = 0;
static 	int		ARG_PROGRESS = 0;
static 	int		ARG_COLORSPACE = (XVID_CSP_RGB | XVID_CSP_VFLIP);

//=============================================================================
// class CXvidEncoder
CXvidEncoder::CXvidEncoder(void)
	: m_HandleEncode(NULL)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
	, m_nVideoQuant(DEFAULT_CODEC_VIDEO_QUANT)
	, m_nFrameRate(DEFAULT_CODEC_FRAME_RATE)
{
}


CXvidEncoder::~CXvidEncoder(void)
{
	Destroy();
}

/// 创建
BOOL CXvidEncoder::Create(void)
{
	int xerr;
	//xvid_plugin_cbr_t cbr;
    xvid_plugin_single_t single;
	xvid_plugin_2pass1_t rc2pass1;
	xvid_plugin_2pass2_t rc2pass2;
	xvid_plugin_ssim_t ssim;
    xvid_plugin_lumimasking_t masking;
	//xvid_plugin_fixed_t rcfixed;
	xvid_enc_plugin_t plugins[8];
	xvid_enc_create_t xvid_enc_create;
	int i;

	xvid_gbl_init_t xvid_gbl_init;

	/*------------------------------------------------------------------------
	 * Xvid core initialization
	 *----------------------------------------------------------------------*/
	/* Set version -- version checking will done by xvidcore */
	memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init));
	xvid_gbl_init.version = XVID_VERSION;
    xvid_gbl_init.debug = ARG_DEBUG;


	/* Do we have to enable ASM optimizations ? */
#ifdef XVID_USE_ASSEMBLER
#ifdef ARCH_IS_IA64
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE | XVID_CPU_ASM;
#else
	xvid_gbl_init.cpu_flags = 0;
#endif
#else
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE;
#endif

	/* Initialize Xvid core -- Should be done once per __process__ */
	xvid_global(NULL, XVID_GBL_INIT, &xvid_gbl_init, NULL);

	/*------------------------------------------------------------------------
	 * Xvid encoder initialization
	 *----------------------------------------------------------------------*/

	/* Version again */
	memset(&xvid_enc_create, 0, sizeof(xvid_enc_create));
	xvid_enc_create.version = XVID_VERSION;

	/* Width and Height of input frames */
	xvid_enc_create.width = m_nVideoWidth;
	xvid_enc_create.height = m_nVideoHeight;
	xvid_enc_create.profile = XVID_PROFILE_S_L2; /* Unrestricted */

	/* init plugins  */
//    xvid_enc_create.zones = ZONES;
//    xvid_enc_create.num_zones = NUM_ZONES;

	xvid_enc_create.plugins = plugins;
	xvid_enc_create.num_plugins = 0;

	if (ARG_SINGLE) {
		memset(&single, 0, sizeof(xvid_plugin_single_t));
		single.version = XVID_VERSION;
		single.bitrate = ARG_BITRATE;
		single.reaction_delay_factor = ARG_REACTION;
		single.averaging_period = ARG_AVERAGING;
		single.buffer = ARG_SMOOTHER;
		

		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_single;
		plugins[xvid_enc_create.num_plugins].param = &single;
		xvid_enc_create.num_plugins++;
		//if (!ARG_BITRATE)
		//	prepare_cquant_zones();
	}

	if (ARG_PASS2) {
		memset(&rc2pass2, 0, sizeof(xvid_plugin_2pass2_t));
		rc2pass2.version = XVID_VERSION;
		rc2pass2.filename = ARG_PASS2;
		rc2pass2.bitrate = ARG_BITRATE;

		rc2pass2.keyframe_boost = ARG_KBOOST;
		rc2pass2.curve_compression_high = ARG_CHIGH;
		rc2pass2.curve_compression_low = ARG_CLOW;
		rc2pass2.overflow_control_strength = ARG_OVERSTRENGTH;
		rc2pass2.max_overflow_improvement = ARG_OVERIMPROVE;
		rc2pass2.max_overflow_degradation = ARG_OVERDEGRADE;
		rc2pass2.kfreduction = ARG_KREDUCTION;
		rc2pass2.kfthreshold = ARG_KTHRESH;
		rc2pass2.container_frame_overhead = ARG_OVERHEAD;

//		An example of activating VBV could look like this 
		rc2pass2.vbv_size     =  ARG_VBVSIZE;
		rc2pass2.vbv_initial  =  (ARG_VBVSIZE*3)/4;
		rc2pass2.vbv_maxrate  =  ARG_VBVMAXRATE;
		rc2pass2.vbv_peakrate =  ARG_VBVPEAKRATE;


		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_2pass2;
		plugins[xvid_enc_create.num_plugins].param = &rc2pass2;
		xvid_enc_create.num_plugins++;
	}

	//if (stats_pass1) {
	//	memset(&rc2pass1, 0, sizeof(xvid_plugin_2pass1_t));
	//	rc2pass1.version = XVID_VERSION;
	//	rc2pass1.filename = stats_pass1;
	//	if (ARG_FULL1PASS)
	//		prepare_full1pass_zones();
	//	plugins[xvid_enc_create.num_plugins].func = xvid_plugin_2pass1;
	//	plugins[xvid_enc_create.num_plugins].param = &rc2pass1;
	//	xvid_enc_create.num_plugins++;
	//}

	/* Zones stuff */
	xvid_enc_create.zones = NULL;
	xvid_enc_create.num_zones = NUM_ZONES;
	//xvid_enc_create.zones = (xvid_enc_zone_t*)malloc(sizeof(xvid_enc_zone_t) * NUM_ZONES);
	//xvid_enc_create.num_zones = NUM_ZONES;
	//for (i=0; i < xvid_enc_create.num_zones; i++) {
	//	xvid_enc_create.zones[i].frame = ZONES[i].frame;
	//	xvid_enc_create.zones[i].base = 100;
	//	xvid_enc_create.zones[i].mode = ZONES[i].mode;
	//	xvid_enc_create.zones[i].increment = ZONES[i].modifier;
	//}


	if (ARG_LUMIMASKING) {
		memset(&masking, 0, sizeof(xvid_plugin_lumimasking_t));
		masking.method = (ARG_LUMIMASKING==2);
		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_lumimasking;
		plugins[xvid_enc_create.num_plugins].param = &masking;
		xvid_enc_create.num_plugins++;
	}

	if (ARG_DUMP) {
		plugins[xvid_enc_create.num_plugins].func = xvid_plugin_dump;
		plugins[xvid_enc_create.num_plugins].param = NULL;
		xvid_enc_create.num_plugins++;
	}

	if (ARG_SSIM>=0 || ARG_SSIM_PATH != NULL) {
        memset(&ssim, 0, sizeof(xvid_plugin_ssim_t));

        plugins[xvid_enc_create.num_plugins].func = xvid_plugin_ssim;

		if( ARG_SSIM >=0){
			ssim.b_printstat = 1;
			ssim.acc = ARG_SSIM;
		} else {
			ssim.b_printstat = 0;
			ssim.acc = 2;
		}

		if(ARG_SSIM_PATH != NULL){		
			ssim.stat_path = ARG_SSIM_PATH;
		}

        ssim.cpu_flags = ARG_CPU_FLAGS;
		ssim.b_visualize = 0;
		plugins[xvid_enc_create.num_plugins].param = &ssim;
		xvid_enc_create.num_plugins++;
	}

	if (ARG_PSNRHVSM>0) {
        plugins[xvid_enc_create.num_plugins].func = xvid_plugin_psnrhvsm;
		plugins[xvid_enc_create.num_plugins].param = NULL;
		xvid_enc_create.num_plugins++;
	}

#if 0
	if (ARG_DEBUG) {
		plugins[xvid_enc_create.num_plugins].func = rawenc_debug;
		plugins[xvid_enc_create.num_plugins].param = NULL;
		xvid_enc_create.num_plugins++;
	}
#endif

	xvid_enc_create.num_threads = ARG_THREADS;
	xvid_enc_create.num_slices  = ARG_SLICES;

	/* Frame rate  */
	xvid_enc_create.fincr = ARG_DWSCALE;
	xvid_enc_create.fbase = ARG_DWRATE;

	/* Maximum key frame interval */
    if (ARG_MAXKEYINTERVAL > 0) {
        xvid_enc_create.max_key_interval = ARG_MAXKEYINTERVAL;
    }else {
	    xvid_enc_create.max_key_interval = (int) ARG_FRAMERATE *10;
    }

	xvid_enc_create.min_quant[0]=ARG_QUANTS[0];
	xvid_enc_create.min_quant[1]=ARG_QUANTS[2];
	xvid_enc_create.min_quant[2]=ARG_QUANTS[4];
	xvid_enc_create.max_quant[0]=ARG_QUANTS[1];
	xvid_enc_create.max_quant[1]=ARG_QUANTS[3];
	xvid_enc_create.max_quant[2]=ARG_QUANTS[5];

	/* Bframes settings */
	xvid_enc_create.max_bframes = 0;
	xvid_enc_create.bquant_ratio = ARG_BQRATIO;
	xvid_enc_create.bquant_offset = ARG_BQOFFSET;

	/* Frame drop ratio */
	xvid_enc_create.frame_drop_ratio = ARG_FRAMEDROP;

	/* Start frame number */
	xvid_enc_create.start_frame_num = 0;

	/* Global encoder options */
	xvid_enc_create.global = 0;

	if (ARG_PACKED)
		xvid_enc_create.global |= XVID_GLOBAL_PACKED;

	if (ARG_CLOSED_GOP)
		xvid_enc_create.global |= XVID_GLOBAL_CLOSED_GOP;

	if (ARG_STATS)
		xvid_enc_create.global |= XVID_GLOBAL_EXTRASTATS_ENABLE;

	/* I use a small value here, since will not encode whole movies, but short clips */
	xerr = xvid_encore(NULL, XVID_ENC_CREATE, &xvid_enc_create, NULL);

	/* Retrieve the encoder instance from the structure */
	m_HandleEncode = xvid_enc_create.handle;

	//free(xvid_enc_create.zones);

	return (xerr);
}

/// 销毁
void CXvidEncoder::Destroy(void)
{
	if(NULL != m_HandleEncode)
	{
		xvid_encore(m_HandleEncode, XVID_ENC_DESTROY, NULL, NULL);
		m_HandleEncode = NULL;
	}
}

/// 压缩
int32_t CXvidEncoder::Encodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	int32_t ret;

	xvid_enc_frame_t xvid_enc_frame;
	xvid_enc_stats_t xvid_enc_stats;

	/* Version for the frame and the stats */
	memset(&xvid_enc_frame, 0, sizeof(xvid_enc_frame));
	xvid_enc_frame.version = XVID_VERSION;

	memset(&xvid_enc_stats, 0, sizeof(xvid_enc_stats));
	xvid_enc_stats.version = XVID_VERSION;

	/* Bind output buffer */
	xvid_enc_frame.bitstream = pDestBuffer;
	xvid_enc_frame.length = -1;

	/* Initialize input image fields */
	if (pSrcBuffer) 
	{
		xvid_enc_frame.input.plane[0] = (void*)pSrcBuffer;
		xvid_enc_frame.input.csp = ARG_COLORSPACE;
		xvid_enc_frame.input.stride[0] = m_nVideoWidth * 3;
	} 
	else
	{
		xvid_enc_frame.input.csp = XVID_CSP_NULL;
	}

	/* Set up core's general features */
	xvid_enc_frame.vol_flags = 0;
	if (ARG_STATS)
		xvid_enc_frame.vol_flags |= XVID_VOL_EXTRASTATS;
	if (ARG_QTYPE) 
	{
		xvid_enc_frame.vol_flags |= XVID_VOL_MPEGQUANT;
		xvid_enc_frame.quant_intra_matrix = NULL;
		xvid_enc_frame.quant_inter_matrix = NULL;
	}

	if (ARG_PAR)
		xvid_enc_frame.par = ARG_PAR;
	else {
		xvid_enc_frame.par = XVID_PAR_EXT;
		xvid_enc_frame.par_width = ARG_PARWIDTH;
		xvid_enc_frame.par_height = ARG_PARHEIGHT;
	}


	if (ARG_QPEL) {
		xvid_enc_frame.vol_flags |= XVID_VOL_QUARTERPEL;
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE16 | XVID_ME_QUARTERPELREFINE8;
	}
	if (ARG_GMC) {
		xvid_enc_frame.vol_flags |= XVID_VOL_GMC;
		xvid_enc_frame.motion |= XVID_ME_GME_REFINE;
	}

	/* Set up core's general features */
	//xvid_enc_frame.vop_flags = vop_presets[ARG_QUALITY];

	if (ARG_INTERLACING) {
		xvid_enc_frame.vol_flags |= XVID_VOL_INTERLACING;
		if (ARG_INTERLACING == 2)
			xvid_enc_frame.vop_flags |= XVID_VOP_TOPFIELDFIRST;
	}

	xvid_enc_frame.vop_flags |= XVID_VOP_HALFPEL;
	xvid_enc_frame.vop_flags |= XVID_VOP_HQACPRED;

    if (ARG_VOPDEBUG) {
        xvid_enc_frame.vop_flags |= XVID_VOP_DEBUG;
    }

    if (ARG_TRELLIS) {
        xvid_enc_frame.vop_flags |= XVID_VOP_TRELLISQUANT;
    }

	/* Frame type -- taken from function call parameter */
	/* Sometimes we might want to force the last frame to be a P Frame */
	xvid_enc_frame.type = XVID_TYPE_AUTO;

	/* Force the right quantizer -- It is internally managed by RC plugins */
	xvid_enc_frame.quant = 0;

	if (ARG_CHROMAME)
		xvid_enc_frame.motion |= XVID_ME_CHROMA_PVOP + XVID_ME_CHROMA_BVOP;

	/* Set up motion estimation flags */
	//xvid_enc_frame.motion |= motion_presets[ARG_QUALITY];

	if (ARG_TURBO)
		xvid_enc_frame.motion |= XVID_ME_FASTREFINE16 | XVID_ME_FASTREFINE8 | 
								 XVID_ME_SKIP_DELTASEARCH | XVID_ME_FAST_MODEINTERPOLATE | 
								 XVID_ME_BFRAME_EARLYSTOP;

	if (ARG_BVHQ) 
		xvid_enc_frame.vop_flags |= XVID_VOP_RD_BVOP;

	if (ARG_QMETRIC == 1)
		xvid_enc_frame.vop_flags |= XVID_VOP_RD_PSNRHVSM;

	switch (ARG_VHQMODE) /* this is the same code as for vfw */
	{
	case 1: /* VHQ_MODE_DECISION */
		xvid_enc_frame.vop_flags |= XVID_VOP_MODEDECISION_RD;
		break;

	case 2: /* VHQ_LIMITED_SEARCH */
		xvid_enc_frame.vop_flags |= XVID_VOP_MODEDECISION_RD;
		xvid_enc_frame.motion |= XVID_ME_HALFPELREFINE16_RD;
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE16_RD;
		break;

	case 3: /* VHQ_MEDIUM_SEARCH */
		xvid_enc_frame.vop_flags |= XVID_VOP_MODEDECISION_RD;
		xvid_enc_frame.motion |= XVID_ME_HALFPELREFINE16_RD;
		xvid_enc_frame.motion |= XVID_ME_HALFPELREFINE8_RD;
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE16_RD;
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE8_RD;
		xvid_enc_frame.motion |= XVID_ME_CHECKPREDICTION_RD;
		break;

	case 4: /* VHQ_WIDE_SEARCH */
		xvid_enc_frame.vop_flags |= XVID_VOP_MODEDECISION_RD;
		xvid_enc_frame.motion |= XVID_ME_HALFPELREFINE16_RD;
		xvid_enc_frame.motion |= XVID_ME_HALFPELREFINE8_RD;
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE16_RD;
		xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE8_RD;
		xvid_enc_frame.motion |= XVID_ME_CHECKPREDICTION_RD;
		xvid_enc_frame.motion |= XVID_ME_EXTSEARCH_RD;
		break;

	default :
		break;
	}

	/* Not sure what this does */
	// force keyframe spacing in 2-pass 1st pass
	if (ARG_QUALITY == 0)
		xvid_enc_frame.type = XVID_TYPE_IVOP;

	/* frame-based stuff */
	//apply_zone_modifiers(&xvid_enc_frame, framenum);


	/* Encode the frame */
	ret = xvid_encore(m_HandleEncode, XVID_ENC_ENCODE, &xvid_enc_frame,
					  &xvid_enc_stats);

	//*key = (xvid_enc_frame.out_flags & XVID_KEYFRAME);
	//*stats_type = xvid_enc_stats.type;
	//*stats_quant = xvid_enc_stats.quant;
	//*stats_length = xvid_enc_stats.length;
	//sse[0] = xvid_enc_stats.sse_y;
	//sse[1] = xvid_enc_stats.sse_u;
	//sse[2] = xvid_enc_stats.sse_v;

	return (ret);
}

/// 设置视频帧信息
BOOL CXvidEncoder::SetFrameInfo(uint16_t nVideoWidth, uint16_t nVideoHeight)
{
	if(NULL != m_HandleEncode)
		return FALSE;

	m_nVideoWidth = nVideoWidth;
	m_nVideoHeight = nVideoHeight;
	return TRUE;
}

/// 获得视频帧信息
BOOL CXvidEncoder::GetFrameInfo(uint16_t& nVideoWidth, uint16_t& nVideoHeight) const
{
	nVideoWidth = m_nVideoWidth;
	nVideoHeight = m_nVideoHeight;
	return TRUE;
}

/// 设置视频质量
BOOL CXvidEncoder::SetVideoQuant(uint16_t nQuant)
{
	if(NULL != m_HandleEncode)
		return FALSE;

	m_nVideoQuant = nQuant;
	return TRUE;
}

/// 获得视频质量
uint16_t CXvidEncoder::GetVideoQuant(void) const
{
	return m_nVideoQuant;
}

/// 设置帧率
BOOL CXvidEncoder::SetFrameRate(uint16_t nFrameRate)
{
	if(NULL != m_HandleEncode)
		return FALSE;

	m_nFrameRate = nFrameRate;
	return TRUE;
}

/// 获得帧率
uint16_t CXvidEncoder::GetFrameRate(void) const
{
	return m_nFrameRate;
}

//=============================================================================
// class CXvidDecoder
CXvidDecoder::CXvidDecoder(void)
	: m_HandleDecode(NULL)
	, m_nVideoWidth(0)
	, m_nVideoHeight(0)
{
}


CXvidDecoder::~CXvidDecoder(void)
{
	Destroy();
}

/// 创建
BOOL CXvidDecoder::Create(void)
{
	int ret;

	xvid_gbl_init_t   xvid_gbl_init;
	xvid_dec_create_t xvid_dec_create;
	xvid_gbl_info_t   xvid_gbl_info;

	/* Reset the structure with zeros */
	memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init_t));
	memset(&xvid_dec_create, 0, sizeof(xvid_dec_create_t));
	memset(&xvid_gbl_info, 0, sizeof(xvid_gbl_info));

	/*------------------------------------------------------------------------
	 * Xvid core initialization
	 *----------------------------------------------------------------------*/

	xvid_gbl_info.version = XVID_VERSION;
	xvid_global(NULL, XVID_GBL_INFO, &xvid_gbl_info, NULL);

	/* Version */
	xvid_gbl_init.version = XVID_VERSION;

	/* Assembly setting */
#ifdef XVID_USE_ASSEMBLER
#ifdef ARCH_IS_IA64
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE | XVID_CPU_IA64;
#else
	xvid_gbl_init.cpu_flags = 0;//XVID_CPU_FORCE | XVID_CPU_MMX;;
#endif
#else
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE;
#endif

	xvid_gbl_init.debug = 0;

	xvid_global(NULL, 0, &xvid_gbl_init, NULL);

	/*------------------------------------------------------------------------
	 * Xvid decoder initialization
	 *----------------------------------------------------------------------*/

	/* Version */
	xvid_dec_create.version = XVID_VERSION;

	/*
	 * Image dimensions -- set to 0, xvidcore will resize when ever it is
	 * needed
	 */
	xvid_dec_create.width = m_nVideoWidth;
	xvid_dec_create.height = m_nVideoHeight;

	xvid_dec_create.num_threads = ARG_THREADS;

	ret = xvid_decore(NULL, XVID_DEC_CREATE, &xvid_dec_create, NULL);

	m_HandleDecode = xvid_dec_create.handle;

	return (ret == 0);
}

/// 销毁
void CXvidDecoder::Destroy(void)
{
	if(NULL != m_HandleDecode)
	{
		xvid_decore(m_HandleDecode, XVID_DEC_DESTROY, NULL, NULL);
		m_HandleDecode = NULL;
	}
}

/// 解码
int32_t CXvidDecoder::Decodec(const char* pSrcBuffer, uint32_t nSrcBuffSize, 
	char* pDestBuffer, uint32_t nDestBufferSize)
{
	uint32_t ret = 0;

	if(NULL != m_HandleDecode)
	{
		char* pBuffer = const_cast<char*>(pSrcBuffer);
		uint32_t nBufferSize = nSrcBuffSize;
		xvid_dec_stats_t xvid_dec_stats;

		do
		{
			if(0 != m_nVideoWidth && 0 != m_nVideoHeight)
			{
				// 检查输出Buffer Size
				uint32_t nMinSize = m_nVideoWidth * m_nVideoHeight * 24 / 8;
				if(nDestBufferSize < nMinSize)
				{
					break;
				}
			}

			int nDecodeByte = DecodeFrame(pBuffer, nBufferSize, pDestBuffer, &xvid_dec_stats);
			if(nDecodeByte < 0)
				return 0;

			if(xvid_dec_stats.type == XVID_TYPE_VOL)
			{
				m_nVideoWidth = xvid_dec_stats.data.vol.width;
				m_nVideoHeight = xvid_dec_stats.data.vol.height;
			}

			if(nDecodeByte > 0)
			{
				pBuffer += nDecodeByte;
				nBufferSize-= nDecodeByte;
			}
			else
			{
				break;
			}

		}while (xvid_dec_stats.type <= 0 && nBufferSize > 1);

		ret = m_nVideoWidth * m_nVideoHeight * 3; 
	}
	return ret;
}

int CXvidDecoder::DecodeFrame(char *istream, int istream_size, 
	char *ostream, xvid_dec_stats_t *xvid_dec_stats)
{
		int ret;

	xvid_dec_frame_t xvid_dec_frame;

	/* Reset all structures */
	memset(&xvid_dec_frame, 0, sizeof(xvid_dec_frame_t));
	memset(xvid_dec_stats, 0, sizeof(xvid_dec_stats_t));

	/* Set version */
	xvid_dec_frame.version = XVID_VERSION;
	xvid_dec_stats->version = XVID_VERSION;

	/* No general flags to set */
	xvid_dec_frame.general          = 0;

	/* Input stream */
	xvid_dec_frame.bitstream        = istream;
	xvid_dec_frame.length           = istream_size;

	/* Output frame structure */
	xvid_dec_frame.output.plane[0]  = ostream;
	xvid_dec_frame.output.stride[0] = m_nVideoWidth * 3;
	xvid_dec_frame.output.csp = ARG_COLORSPACE;

	ret = xvid_decore(m_HandleDecode, XVID_DEC_DECODE, &xvid_dec_frame, xvid_dec_stats);

	return(ret);
}

/// 设置视频帧信息
BOOL CXvidDecoder::SetFrameInfo(uint16_t nVideoWidth, uint16_t nVideoHeight)
{
	m_nVideoWidth = nVideoWidth;
	m_nVideoHeight = nVideoHeight;
	return TRUE;
}

/// 获得视频帧信息
BOOL CXvidDecoder::GetFrameInfo(uint16_t& nVideoWidth, uint16_t& nVideoHeight) const
{
	nVideoWidth = m_nVideoWidth;
	nVideoHeight = m_nVideoHeight;
	return TRUE;
}