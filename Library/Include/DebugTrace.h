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
/// \file    : DebugTrace.h
/// \brief   : 日志调试类头文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-16
///============================================================================
#ifndef __DEBUG_TRACE_H__
#define __DEBUG_TRACE_H__

#include "TypeDefine.h"
#include "CriticalSection.h"

//=============================================================================
#ifdef WIN32
#pragma warning(disable:4141 4996)
#endif

#ifdef	_WIN32
#ifndef _DEBUGTRACE_EXPORT
#ifdef	_DEBUG
#pragma comment(lib,"libDebugTraceD.lib")
#else
#pragma comment(lib,"libDebugTrace.lib")
#endif	//_TCPNETTRANS_EXPORT
#endif	//_TCPNETTRANS_EXPORT
#endif	//_WIN32

//=============================================================================
#ifdef ASSERT
#undef ASSERT
#endif

//  重新定义ASSERT宏  
#ifdef _DEBUG
#define ASSERT(f)		if(f)			\
							;			\
						else            \
							CDebugTrace::AssertFail((#f), __FILE__, __LINE__)
#else
#define ASSERT(f)
#endif

//=============================================================================
//RecordLog是流方式的日志输出
//eg: RecordLog(1,"姓名:" << lszName<<"年龄:"<< 26 << "性别" << TRUE);	
#define RecordLog(level, args) \
	if(!CDebugTrace::CanTrace(level)) ; else \
	CDebugTrace::EndTrace(CDebugTrace::BeginTrace(level, __FILE__, __LINE__) << args << '\n')

//TraceLogX系列是类似printf方式输出,X表示日志级别
//eg:TRACE0("姓名：%s,年龄：%d,性别:%d",lszName,26,TRUE);
#define TraceLogFatal \
	if (!CDebugTrace::CanTrace(CDebugTrace::FATAL_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(FATAL_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogAlert \
	if (!CDebugTrace::CanTrace(CDebugTrace::ALERT_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(ALERT_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogError \
	if (!CDebugTrace::CanTrace(CDebugTrace::ERROR_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(ERROR_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogWarn \
	if (!CDebugTrace::CanTrace(CDebugTrace::WARN_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(WARN_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogInfo \
	if (!CDebugTrace::CanTrace(CDebugTrace::INFO_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(INFO_LEVEL, __FILE__, __LINE__).TraceFormat

#define TraceLogDebug \
	if (!CDebugTrace::CanTrace(CDebugTrace::DEBUG_LEVEL)) 	;  else\
	CDebugTrace::BeginTrace(DEBUG_LEVEL, __FILE__, __LINE__).TraceFormat

//取消TRACE定义
#undef TRACE
#undef TRACE_FATAL
#undef TRACE_ALERT
#undef TRACE_ERROR
#undef TRACE_WARN
#undef TRACE_INFO
#undef TRACE_DEBUG

//重新定义TRACE语句
#ifdef _DEBUG			
//调试版
#define TRACE RecordLog	//打印日志

#define TRACE_FATAL TraceLogFatal
#define TRACE_ALERT TraceLogAlert
#define TRACE_ERROR	TraceLogError
#define TRACE_WARN  TraceLogWarn
#define TRACE_INFO  TraceLogInfo
#define TRACE_DEBUG TraceLogDebug
#else					
//发行版
#define TRACE(level, args) 
#define TRACE_FATAL ;
#define TRACE_ALERT ;
#define TRACE_ERROR	;
#define TRACE_WARN  ;
#define TRACE_INFO  ;
#define TRACE_DEBUG ;

#endif //_DEBUG

//=============================================================================
#define MAX_TRACE_BUFFER_SIZE	(1024*4)

#ifndef MAX_PATH
#define MAX_PATH 512
#endif

//=============================================================================
// class CDebugTrace
class CDebugTrace
{
public:
	// 打印选项
	enum Options
	{
		Timestamp		= 1,	///< 打印时间
		LogLevel		= 2,	///< 打印日志级别
		FileAndLine		= 4,	///< 打印文件名和行号
		AppendToFile	= 8,	///< 把日志追加到文件
		PrintToConsole	= 16,	///< 输出日志到控制台
	};

	// 日志级别
	typedef enum _TraceLevel
	{
		FATAL_LEVEL,			///< 致命错误
		ALERT_LEVEL,			///< 严重错误
		ERROR_LEVEL,			///< 一般性错误
		WARN_LEVEL,				///< 警告
		INFO_LEVEL,				///< 信息
		DEBUG_LEVEL,			///< 调试信息				
	}trace_level_t;

public:
	CDebugTrace(void);
	~CDebugTrace(void);

public:
	//设置日志级别
	static void SetTraceLevel(trace_level_t nLogLevel);

	//设置日志文件名
	static void SetLogFileName(const char* szLogFile);

	// 设置TRACE选项
	static void SetTraceOptions(uint32_t nOptions);

	//取得TRACE选项
	static uint32_t GetTraceOptions(void);

	//判断给定级别是否可以打印
	static BOOL CanTrace(trace_level_t nLogLevel);					

	//开始打印
	static CDebugTrace& BeginTrace(trace_level_t nLogLevel, 
		const char *pSrcFile,int iSrcLine);	

	//结束打印
	static void EndTrace(CDebugTrace& oDebugTrace);

	//断言失败处理函数
	static void AssertFail(const char* strCond, char* strFile, uint32_t nLine);

public:
	//按照函数printf的类似格式打印日志
	void TraceFormat(const char* pFmt,...);	

public:
	//以下分别输出各种数据类型
	CDebugTrace& operator << (bool bValue);
	CDebugTrace& operator << (char cValue);
	CDebugTrace& operator << (int8_t nValue);
	CDebugTrace& operator << (uint8_t nValue);	
	CDebugTrace& operator << (int16_t nValue);
	CDebugTrace& operator << (uint16_t nValue);
	CDebugTrace& operator << (int32_t nValue);
	CDebugTrace& operator << (uint32_t nValue);
	CDebugTrace& operator << (int64_t nValue);
	CDebugTrace& operator << (uint64_t nValue);
	CDebugTrace& operator << (float fValue);
	CDebugTrace& operator << (double fValue);
	CDebugTrace& operator << (const char *apStrVal);	

private:
	char m_szPrintBuffer[MAX_TRACE_BUFFER_SIZE];	///< 缓存区
	uint32_t m_nDataSize;							///< 缓存长度

	CCriticalSection m_oCriticalSection;			///< 临界区

private:
	static trace_level_t m_nLogLevel;				///< 日志级别
	static char		m_szLogFile[MAX_PATH];			///< 日志文件
	static uint32_t m_nTraceOptions;				///< 打印日志选项
};


#endif //__DEBUG_TRACE_H__
