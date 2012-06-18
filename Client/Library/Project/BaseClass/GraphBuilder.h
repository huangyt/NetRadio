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
/// \file    : GraphBuilder.h
/// \brief   : GraphBuilder类头文件
/// \author  : letion
/// \version : 2.0
/// \date    : 2008-09-05
///============================================================================
#ifndef __GRAPH_BUILDER_H__
#define __GRAPH_BUILDER_H__

//=============================================================================
/// 安全释放指针
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif // SAFE_DELETE

/// 安全释放对象
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif // SAFE_RELEASE

//=============================================================================
class CGraphBuilder
{
public:
    CGraphBuilder(void);
    virtual ~CGraphBuilder(void);

public:
	virtual BOOL Run(void);        
	virtual BOOL Stop(void);
	virtual BOOL Pause(void);
	virtual BOOL IsRunning(void) const;
	virtual BOOL IsStopped(void) const;
	virtual BOOL IsPaused(void) const;
	virtual BOOL SetNotifyWindow(HWND inWindow, UINT nMsgID);

protected:
    virtual BOOL CreateGraphBuilder(void);
    virtual void DestroyGraphBuilder(void);
	virtual void HandleEvent(WPARAM inWParam, LPARAM inLParam);
    virtual void OnNotify(int nNotifyCode) = 0;

protected:
	IGraphBuilder* m_pGraphBulider;
	IMediaControl* m_pMediaControl;
	IMediaEventEx* m_pEvent;
    HWND           m_hWnd;
    UINT           m_nMsgID;

private:
    enum {GS_Stopped, GS_Paused, GS_Running} m_GraphState;     
};

#endif //__GRAPH_BUILDER_H__