// SetupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TestVideoCapture.h"
#include "SetupDialog.h"
#include "afxdialogex.h"


// CSetupDialog dialog

IMPLEMENT_DYNAMIC(CSetupDialog, CDialogEx)

CSetupDialog::CSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetupDialog::IDD, pParent)
	, m_nVideoWidth(320)
	, m_nVideoHeight(240)
	, m_nFrameRate(30)
{

}

CSetupDialog::~CSetupDialog()
{
}

void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_nVideoWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nVideoHeight);
	DDX_Text(pDX, IDC_EDIT_RATE, m_nFrameRate);
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetupDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetupDialog message handlers
void CSetupDialog::OnBnClickedOk()
{
	UpdateData(TRUE);
	CDialogEx::OnOK();
}
