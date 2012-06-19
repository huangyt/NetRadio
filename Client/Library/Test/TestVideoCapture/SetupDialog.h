#pragma once


// CSetupDialog dialog

class CSetupDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSetupDialog)

public:
	CSetupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetupDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	long m_nVideoWidth;
	long m_nVideoHeight;
	long m_nFrameRate;
	afx_msg void OnBnClickedOk();
};
