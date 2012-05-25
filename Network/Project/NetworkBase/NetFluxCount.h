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
/// \file    : CNetFluxCount.h
/// \brief   : 网络流量统计函数
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-23
///============================================================================
#ifndef __NET_FLUX_COOUNT_H__
#define __NET_FLUX_COOUNT_H__

#include "TypeDefine.h"
#include "NetworkAPI.h"

//=============================================================================
class CNetFluxCount
{
public:
	CNetFluxCount(void);
	~CNetFluxCount(void);

public:
	//增加流入计数(单位：字节)
	__inline void AddInFluxCount(uint64_t ai64InFlux) 
	{ 
		m_i64InFlux += ai64InFlux;
	}

	//增加流出计数(单位：字节)
	__inline void AddOutFluxCount(uint64_t ai64OutFlux) 
	{ 
		m_i64OutFlux += ai64OutFlux;
	}

	//计算流量(单位：kbps/s)
	int CalcFluxCount(float &afOutputFlux, float &afInputFlux) const;

	//重置状态
	void ResetCount(void);

private:
	uint64_t m_i64BeginTime;		//开始时间
	uint64_t m_i64InFlux;			//流入字节数
	uint64_t m_i64OutFlux;			//流出字节数
};

#endif //__NET_FLUX_COOUNT_H__
