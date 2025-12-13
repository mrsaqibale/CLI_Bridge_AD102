// UsbTestDlg.h : header file
//

#pragma once
#include "afxcmn.h"


#ifdef _M_IX86										//Win32ƽ̨
#include "..\USBBoxProtocol\USBBoxProtocol\USBBoxProtocol.h"
#pragma comment(lib, "..\\Release\\USBBoxProtocol.lib")
#else												//x64ƽ̨
#include "..\USBBoxProtocol\USBBoxProtocol\USBBoxProtocol.h"
#pragma comment(lib, "..\\x64\\x64_release\\USBBoxProtocol.lib")
#endif


// CUsbTestDlg dialog
class CUsbTestDlg : public CDialog
{
// Construction
public:
	CUsbTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_USBTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	static void CALLBACK USBVoiceCallBack(BYTE *pbuffer,int nLength,int nReference,int nDeviceIndex,int nAdpcm,DWORD dwDeviceID);
	static void CALLBACK USBEventCallBack(WORD wEventCode, int nReference,DWORD dwParam,DWORD dwDeviceID);

	BOOL		m_bMonitor;
	int			m_nMonitorReference;
	int			m_nMonitorLength;
	BYTE		m_szMonitorBufer[4096];

	SHORT		m_PCM16Buff[0x8000];	
	CPen		m_PenG;
	CBrush		m_brsBkGnd;
	CPen		*m_pOldInputPen;
	CRect		m_SoundPicture;

	void DrawSoundPicture(short *pShortData,DWORD dwDrawBytes);

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
	CListCtrl m_EventListCtrl;
	afx_msg void OnBnClickedButtonEnable();
	afx_msg void OnBnClickedButtonDisable();
	afx_msg void OnBnClickedButtonSettype();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonPickup();
	afx_msg void OnBnClickedButtonHookon();
	afx_msg void OnBnClickedButtonAutocall();
	afx_msg void OnBnClickedButtonMicon();
	afx_msg void OnBnClickedButtonMicoff();
	afx_msg void OnBnClickedButtonGetusbcount();
	afx_msg void OnBnClickedButtonGetchcount();
	afx_msg LRESULT OnUsbBoxDeviceMessage(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnAD800Message(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnAD101Message(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonMonitor();
	afx_msg void OnBnClickedButtonStopmonitor();
	afx_msg void OnBnClickedButtonOlddllStart();
	afx_msg void OnBnClickedButtonOlddllStop();
	afx_msg void OnBnClickedButtonOlddllStart2();
	afx_msg void OnBnClickedButtonOlddllStop2();
};
