#include <sys/types.h> 
#include <sys/timeb.h>
#include "NetworkAPI.h"

#ifndef _WIN32
uint32_t GetTickCount(void)
{
	struct timeb loTimeb;
	memset(&loTimeb, 0 , sizeof(timeb));
	ftime(&loTimeb); 
	return ((DWORD)loTimeb.time * 1000) + loTimeb.millitm;
}
#endif

const char* GetTimeString(time_t* pTime)
{
	static char szTime[20] = {0};
	strftime(szTime, 20, "%Y-%m-%d %H:%M:%S", localtime(pTime));
	return szTime;
}

uint64_t GetSystemTime(void)
{
	struct timeb loTimeb;
	//memset(&loTimeb, 0 , sizeof(timeb));
	ftime(&loTimeb);
	return ((INT64)loTimeb.time * 1000) + loTimeb.millitm;
}


//=============================================================================
uint32_t GetSocketAddr(const sockaddr_in& addr)
{
#ifdef _WIN32
	return addr.sin_addr.S_un.S_addr;
#else
	return addr.sin_addr.s_addr;
#endif
}

void SetSocketAddr(sockaddr_in& addr, uint32_t nAddr)
{
#ifdef _WIN32
	addr.sin_addr.S_un.S_addr = nAddr;
#else
	addr.sin_addr.s_addr = nAddr;
#endif	
}

const char* GetIPAddr(uint32_t nAddr)
{
	sockaddr_in addr;

#ifdef _WIN32
	addr.sin_addr.S_un.S_addr = nAddr;
#else
	addr.sin_addr.s_addr = nAddr;
#endif	

	return inet_ntoa(addr.sin_addr);
}

BOOL IsIPAddr(const char* szAddr)
{
	struct in_addr addr;
	memset(&addr, 0, sizeof(addr));

#ifdef _WIN32
	addr.S_un.S_addr = inet_addr( szAddr );
#else
	addr.s_addr = inet_addr( szAddr );
#endif

	if (strcmp(inet_ntoa(addr), szAddr ) != 0 ) 
	{
		return FALSE;
	}

	return TRUE;
}

long GetIdcIP(uint32_t nIPAddr)
{
#ifdef _WIN32
	struct in_addr sin_addr1;
	struct in_addr sin_addr2;

	sin_addr1.S_un.S_addr = nIPAddr;
	sin_addr2.S_un.S_un_w.s_w1 = sin_addr1.S_un.S_un_w.s_w1;
	sin_addr2.S_un.S_un_b.s_b3 = sin_addr1.S_un.S_un_b.s_b3;
	sin_addr2.S_un.S_un_b.s_b4 = 0;

	return sin_addr2.S_un.S_addr;

#else
	UNION_ADDR addr1;
	UNION_ADDR addr2;

	addr1.s_addr = nIPAddr;
	addr2.s_un_w.s_w1 = addr1.s_un_w.s_w1;
	addr2.s_un_b.s_b3 = addr1.s_un_b.s_b3;
	addr2.s_un_b.s_b4 = 0;

	return addr2.s_addr;
#endif
}

BOOL IsSameIdc(uint32_t nIPAddr1, uint32_t nIPAddr2)
{
#ifdef _WIN32
	struct in_addr sin_addr1;
	struct in_addr sin_addr2;
	sin_addr1.S_un.S_addr = nIPAddr1;
	sin_addr2.S_un.S_addr = nIPAddr2;

	if ((sin_addr1.S_un.S_un_w.s_w1 == sin_addr2.S_un.S_un_w.s_w1)
		&& (sin_addr1.S_un.S_un_b.s_b3 == sin_addr2.S_un.S_un_b.s_b3))
	{
		return TRUE;
	}
#else

	UNION_ADDR addr1;
	UNION_ADDR addr2;
	addr1.s_addr = nIPAddr1;
	addr2.s_addr = nIPAddr2;

	if ((addr1.s_un_w.s_w1 == addr2.s_un_w.s_w1)
		&& (addr1.s_un_b.s_b3 == addr2.s_un_b.s_b3))
	{
		return TRUE;
	}
#endif
	return FALSE;
}

uint16_t GetLocalIP(struct sockaddr_in* pAddr, uint16_t nAddrCount)
{
	if(NULL == pAddr || 0 == nAddrCount)
		return 0;

	memset(pAddr, 0, nAddrCount*sizeof(struct sockaddr_in));
	uint16_t nCount = 0;

#ifdef _WIN32
	//获取IP地址
	char szName[80] = {0};
	if(gethostname(szName, sizeof(szName)) == -1)
	{
		return 0;
	}

	//通过主机名获取IP列表
	struct hostent* pHostent = gethostbyname(szName);
	if(NULL == pHostent)
	{
		return 0;
	}

	//从列表中将IP信息复制出来
	for(uint16_t i = 0; i < 10 && nCount < nAddrCount; ++i)
	{
		if (0 == pHostent->h_addr_list[i])
			break;

		memcpy(&(pAddr[i].sin_addr), pHostent->h_addr_list[i], 
			sizeof(struct in_addr));

		pAddr[i].sin_family	= AF_INET;
		nCount++;
	}
#else
	struct ifaddrs* ifap0 = NULL;
	struct ifaddrs* ifap = NULL ;

	if(getifaddrs(&ifap0))
	{ 
		return 0;  
	}

	for(ifap = ifap0; ifap != NULL; ifap=ifap->ifa_next)
	{
		if(ifap->ifa_addr == NULL || 
			0 == (ifap->ifa_flags & IFF_UP)) 
		{
			continue;
		}

		//只获取IPV4地址
		if(AF_INET == ifap->ifa_addr->sa_family )
		{
			struct sockaddr_in* addr4 = (struct sockaddr_in*)ifap->ifa_addr;
			//排除 "127.0.0.1"
			if(16777343 != addr4->sin_addr.s_addr)
			{
				memcpy(&(pAddr[nCount]), addr4, sizeof(struct sockaddr_in));
				nCount++;
			}

			if(nCount >= nAddrCount)
				break;
		}
	}
#endif
	return nCount;
}
