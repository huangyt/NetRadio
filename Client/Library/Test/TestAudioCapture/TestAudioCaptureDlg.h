
// TestAudioCaptureDlg.h : 头文件
//

#pragma once

#include "ICaptureEvent.h"
#include "IAudioCapture.h"
#include "IAudioPlayer.h"


// CTestAudioCaptureDlg 对话框
class CTestAudioCaptureDlg : public CDialogEx, ICaptureEvent
{
// 构造
public:
	CTestAudioCaptureDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTAUDIOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

protected:
	void OnCaptureEvent(ENUM_EVENT_TYPE enType, 
		const char* szEventData, uint32_t nDataSize, uint64_t nTimeStamp);

// 实现
protected:
	HICON m_hIcon;

	HMODULE m_hHandleCapture;
	HMODULE m_hHandlePlayer;

	IAudioCapture* m_pAudioCapture;
	IAudioPlayer* m_pAudioPlayer;

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
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
};
