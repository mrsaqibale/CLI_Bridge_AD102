// VR1DemoDlg.h : 头文件
//

#pragma once

#ifdef _M_IX86										//Win32平台
#include "..\USBBoxProtocol\USBBoxProtocol\USBBoxProtocol.h"
#include "afxcmn.h"
#pragma comment(lib, "..\\Release\\USBBoxProtocol.lib")
#else												//x64平台
#include "..\USBBoxProtocol\USBBoxProtocol\USBBoxProtocol.h"
#pragma comment(lib, "..\\x64\\x64_release\\USBBoxProtocol.lib")
#endif

// CVR1DemoDlg 对话框
class CVR1DemoDlg : public CDialog
{
// 构造
public:
	CVR1DemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VR1DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUsbBoxDeviceMessage(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonEnable();
	afx_msg void OnBnClickedButtonDisable();
	CListCtrl m_EventListCtrl;
};
