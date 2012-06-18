#include <dshow.h>
#include "GraphBuilder.h"

//=============================================================================
CGraphBuilder::CGraphBuilder(void)
	: m_pGraphBulider(NULL)
	, m_pMediaControl(NULL)
	, m_pEvent(NULL)
	, m_GraphState(GS_Stopped)
{
}

CGraphBuilder::~CGraphBuilder(void)
{
    DestroyGraphBuilder();
}

BOOL CGraphBuilder::CreateGraphBuilder(void)
{
	BOOL bResult = FALSE;
    if(NULL != m_pGraphBulider)
    {
        return FALSE;
    }

	do
	{
		if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL, 
			CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraphBulider)))
		{
			if(NULL == m_pMediaControl)
			{
				if(FAILED(m_pGraphBulider->QueryInterface(IID_IMediaControl, 
					(void **)&m_pMediaControl)))
				{
					break;
				}
			}

			if(NULL == m_pEvent)
			{
				if(FAILED(m_pGraphBulider->QueryInterface(IID_IMediaEventEx, 
					(void **)&m_pEvent)))
				{
					break;
				}
			}
		}
		bResult = TRUE;
	}while(FALSE);

	if(!bResult)
	{
		DestroyGraphBuilder();
	}
	return bResult;
}

void CGraphBuilder::DestroyGraphBuilder()
{
    SAFE_RELEASE(m_pGraphBulider);
    SAFE_RELEASE(m_pMediaControl);
    SAFE_RELEASE(m_pEvent);
}

BOOL CGraphBuilder::Run(void)
{
	if (m_pGraphBulider && m_pMediaControl)
	{
		if (!IsRunning())
		{
			if (SUCCEEDED(m_pMediaControl->Run()))
			{
                m_GraphState = GS_Running;
                return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGraphBuilder::Stop(void)
{
	if (m_pGraphBulider && m_pMediaControl)
	{
		if (!IsStopped())
		{	
			if (SUCCEEDED(m_pMediaControl->Stop()))
			{
				m_GraphState = GS_Stopped;
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGraphBuilder::Pause(void)
{
	if (m_pGraphBulider && m_pMediaControl)
	{
		if (!IsPaused())
		{	
			if (SUCCEEDED(m_pMediaControl->Pause()))
			{
                m_GraphState = GS_Paused;
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGraphBuilder::IsRunning(void) const
{
	return (GS_Running == m_GraphState);
}

BOOL CGraphBuilder::IsStopped(void) const
{
	return (GS_Stopped == m_GraphState);
}

BOOL CGraphBuilder::IsPaused(void) const
{
	return (GS_Paused == m_GraphState);
}

BOOL CGraphBuilder::SetNotifyWindow(HWND inWindow, UINT nMsgID)
{
	if (m_pEvent)
	{
		m_pEvent->SetNotifyWindow((OAHWND)inWindow, nMsgID, 0);
        m_hWnd = inWindow;
		return TRUE;
	}
	return FALSE;
}

void CGraphBuilder::HandleEvent(WPARAM inWParam, LPARAM inLParam)
{
	if (m_pEvent)
	{
		LONG eventCode = 0, eventParam1 = 0, eventParam2 = 0;
		while (SUCCEEDED(m_pEvent->GetEvent(&eventCode, &eventParam1, &eventParam2, 0)))
		{
			m_pEvent->FreeEventParams(eventCode, eventParam1, eventParam2);
			switch (eventCode)
			{
			case EC_COMPLETE:	
			case EC_USERABORT:
			case EC_ERRORABORT:
                OnNotify(eventCode);
                break;
            case EC_DEVICE_LOST:
                if (eventParam2 == 0)
                {
                    OnNotify(EC_DEVICE_LOST);
                }
    			break;
			default:
				break;
			}
		}
	}
}
