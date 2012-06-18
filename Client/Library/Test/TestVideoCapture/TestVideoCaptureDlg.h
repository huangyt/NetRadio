
// TestVideoCaptureDlg.h : 头文件
//

#pragma once
#include "ICaptureEvent.h"
#include "IVideoCapture.h"

// CTestVideoCaptureDlg 对话框
class CTestVideoCaptureDlg : public CDialogEx, ICaptureEvent
{
// 构造
public:
	CTestVideoCaptureDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTVIDEOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

protected:
	virtual void OnCaptureEvent(ENUM_EVENT_TYPE enType, 
		const char* szEventData, uint16_t nDataSize, uint64_t nTimeStamp);

// 实现
protected:
	HICON m_hIcon;
	HANDLE m_hHandle;
	IVideoCapture* m_pVideoCapture;


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
