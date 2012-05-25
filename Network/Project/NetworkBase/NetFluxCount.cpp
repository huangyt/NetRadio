#include "NetFluxCount.h"

//=============================================================================
CNetFluxCount::CNetFluxCount(void)
{
	m_i64BeginTime = GetSystemTime();
	m_i64InFlux = 0;;
	m_i64OutFlux = 0;
}


CNetFluxCount::~CNetFluxCount(void)
{
}

//计算流量(单位：kbps/s)
int CNetFluxCount::CalcFluxCount(float &afOutputFlux, float &afInputFlux) const
{
	int nTimeCap = (int)(GetSystemTime() - m_i64BeginTime);
	if (nTimeCap > 1000)
	{
		afOutputFlux = (float)m_i64OutFlux * 8000 / 1024 / nTimeCap;
		afInputFlux = (float)m_i64InFlux * 8000 / 1024 / nTimeCap;

        return nTimeCap;	
	}
    return 0;
}

//重置状态
void CNetFluxCount::ResetCount()
{
	m_i64BeginTime = GetSystemTime();
	m_i64InFlux = 0;;
	m_i64OutFlux = 0;
}
