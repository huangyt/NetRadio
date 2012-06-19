
// TestVideoCaptureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestVideoCapture.h"
#include "TestVideoCaptureDlg.h"
#include "afxdialogex.h"
#include "SetupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestVideoCaptureDlg 对话框




CTestVideoCaptureDlg::CTestVideoCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestVideoCaptureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hHandleCapture = NULL;
	m_hHandlePlayer = NULL;

	m_pVideoCapture = NULL;
	m_pVideoPlayer = NULL;
}

void CTestVideoCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestVideoCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestVideoCaptureDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestVideoCaptureDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestVideoCaptureDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON7, &CTestVideoCaptureDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestVideoCaptureDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTestVideoCaptureDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestVideoCaptureDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &CTestVideoCaptureDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// CTestVideoCaptureDlg 消息处理程序

BOOL CTestVideoCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestVideoCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestVideoCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestVideoCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestVideoCaptureDlg::OnBnClickedButton1()
{
	m_hHandleCapture = LoadLibrary(L"VideoCapture.dll");
	if(NULL != m_hHandleCapture)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandleCapture, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IVideoCaputre, (void**)&m_pVideoCapture);
		}
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton2()
{
	if(NULL != m_hHandleCapture)
	{
		if(NULL != m_pVideoCapture)
		{
			m_pVideoCapture->StopCapture();
			m_pVideoCapture->Close();

			typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
			DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
				m_hHandleCapture, "DestroyInterface");
			if(NULL != DestroyInterface)
			{
				DestroyInterface(CLSID_IVideoCaputre, (void*)m_pVideoCapture);
				m_pVideoCapture = NULL;
			}
		}

		FreeLibrary(m_hHandleCapture);
		m_hHandleCapture = NULL;
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton3()
{
	if(NULL != m_pVideoCapture)
	{
		if(m_pVideoCapture->Open((ICaptureEvent*)this))
		{
			m_pVideoCapture->StartCapture();
		}
	}
}

void CTestVideoCaptureDlg::OnCaptureEvent(ENUM_EVENT_TYPE enType, 
	const char* szEventData, uint32_t nDataSize, uint64_t nTimeStamp)
{
	if(enType == ENUM_EVENT_VIDEO)
	{
		if(NULL != m_pVideoPlayer)
		{
			m_pVideoPlayer->OnVideoData(szEventData, nDataSize, nTimeStamp);
		}
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton4()
{
	m_hHandlePlayer = LoadLibrary(L"VideoPlayer.dll");
	if(NULL != m_hHandlePlayer)
	{
		typedef IRESULT (*CreateFuncPtr)(const CLSID&, void**);
		CreateFuncPtr CreateInterface = (CreateFuncPtr)GetProcAddress(
			m_hHandlePlayer, "CreateInterface");
		if(NULL != CreateInterface)
		{
			CreateInterface(CLSID_IVideoPlayer, (void**)&m_pVideoPlayer);
		}
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton5()
{
	if(NULL != m_hHandlePlayer)
	{
		if(NULL != m_pVideoPlayer)
		{
			m_pVideoPlayer->StopPlay();
			m_pVideoPlayer->Close();

			typedef IRESULT (*DestroyFuncPtr)(const CLSID&, void*);
			DestroyFuncPtr DestroyInterface = (DestroyFuncPtr)GetProcAddress(
				m_hHandlePlayer, "DestroyInterface");
			if(NULL != DestroyInterface)
			{
				DestroyInterface(CLSID_IVideoPlayer, (void*)m_pVideoPlayer);
				m_pVideoPlayer = NULL;
			}
		}

		FreeLibrary(m_hHandlePlayer);
		m_hHandlePlayer = NULL;
	}
}


void CTestVideoCaptureDlg::OnBnClickedButton6()
{
	if(NULL != m_pVideoPlayer)
	{
		CWnd* pWnd = GetDlgItem(IDC_STATIC_VIDEO);
		if(NULL != pWnd)
		{
			if(m_pVideoPlayer->Open(pWnd->GetSafeHwnd()))
			{
				m_pVideoPlayer->StartPlay();
			}
		}
	}
}

void CTestVideoCaptureDlg::OnBnClickedButton7()
{
	CSetupDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(NULL != m_pVideoPlayer)
		{
			m_pVideoPlayer->SetVideoFormat(dlg.m_nVideoWidth, dlg.m_nVideoHeight, 
				dlg.m_nFrameRate);
		}
	}
}


void CTestVideoCaptureDlg::OnBnClickedButton8()
{
	CSetupDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(NULL != m_pVideoCapture)
		{
			m_pVideoCapture->SetVideoFormat(dlg.m_nVideoWidth, dlg.m_nVideoHeight, 
				dlg.m_nFrameRate);
		}
	}
}
