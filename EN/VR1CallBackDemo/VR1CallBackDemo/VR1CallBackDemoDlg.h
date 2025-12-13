// VR1CallBackDemoDlg.h : header file
//

#pragma once

#ifdef _M_IX86										//Win32ƽ̨
#include "..\USBBoxProtocol\USBBoxProtocol\USBBoxProtocol.h"
#include "afxcmn.h"
#pragma comment(lib, "..\\Release\\USBBoxProtocol.lib")
#else												//x64ƽ̨
#include "..\USBBoxProtocol\USBBoxProtocol\USBBoxProtocol.h"
#pragma comment(lib, "..\\x64\\x64_release\\USBBoxProtocol.lib")
#endif


// CVR1CallBackDemoDlg dialog
class CVR1CallBackDemoDlg : public CDialog
{
// Construction
public:
	CVR1CallBackDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VR1CALLBACKDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	static void CALLBACK VR1CallBackEvent(WORD wEventCode, int nReference,DWORD dwParam,DWORD dwDeviceID);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonEnable();
	afx_msg void OnBnClickedButtonDisable();
	CListCtrl m_EventListCtrl;
};
