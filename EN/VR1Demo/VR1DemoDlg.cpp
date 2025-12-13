// VR1DemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VR1Demo.h"
#include "VR1DemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVR1DemoDlg 对话框




CVR1DemoDlg::CVR1DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVR1DemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVR1DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_EventListCtrl);
}

BEGIN_MESSAGE_MAP(CVR1DemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(USBOBX_MSG,&CVR1DemoDlg::OnUsbBoxDeviceMessage)

	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ENABLE, &CVR1DemoDlg::OnBnClickedButtonEnable)
	ON_BN_CLICKED(IDC_BUTTON_DISABLE, &CVR1DemoDlg::OnBnClickedButtonDisable)
END_MESSAGE_MAP()


// CVR1DemoDlg 消息处理程序

BOOL CVR1DemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	m_EventListCtrl.InsertColumn(0,		L"Date/Time",		LVCFMT_LEFT,150,0);
	m_EventListCtrl.InsertColumn(1,		L"Event",			LVCFMT_LEFT,200,0);
	m_EventListCtrl.InsertColumn(2,		L"Channel",			LVCFMT_LEFT,80,0);
	m_EventListCtrl.InsertColumn(3,		L"Remark",			LVCFMT_LEFT,500,0);
	m_EventListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES|LVS_EX_SUBITEMIMAGES|LVS_EX_DOUBLEBUFFER); 


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVR1DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVR1DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CVR1DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVR1DemoDlg::OnBnClickedButtonEnable()
{
	// 特别注意调用顺序
	UsbBox_EnableDevice();
	UsbBox_SetMsg(m_hWnd);									// usb box message sdk

	VRBox_SetTelMsg("192.168.22.106","601");
	GetDlgItem(IDC_BUTTON_ENABLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DISABLE)->EnableWindow(TRUE);
}

void CVR1DemoDlg::OnBnClickedButtonDisable()
{
	UsbBox_DisableDevice();
	GetDlgItem(IDC_BUTTON_ENABLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DISABLE)->EnableWindow(FALSE);
}


LRESULT CVR1DemoDlg::OnUsbBoxDeviceMessage(WPARAM wParam,LPARAM lParam)
{
	MESSAGE_INFO tagMessageInfo;memcpy(&tagMessageInfo,(MESSAGE_INFO*)wParam,sizeof(MESSAGE_INFO));
	switch ( tagMessageInfo.wEventCode )
	{
	case EVENT_STATE:
		{
			CString strDate;
			CString strState;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",tagMessageInfo.nReference+1);
			m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_STATE");
			m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			switch ( UsbBox_GetState(tagMessageInfo.nReference))
			{
			case CH_STATE_HOOKON:
				strState =L"Free";	
				break;
			case CH_STATE_HOOKOFF:
				strState =L"Pickup";	
				break;
			case CH_STATE_RINGON:
				strState =L"Ring";	
				break;
			case CH_STATE_INRECORD:
				strState =L"Inbound Answer";	
				break;
			case CH_STATE_OUTRECORD:
				strState =L"Outbound Answer";	
				break;
			case CH_STATE_UNAVAILABLE:
				strState =L"Unknown";	
				break;
			default:
				strState =L"Unknown";	
				break;
			}
			m_EventListCtrl.SetItemText(nItemCnt,3,strState);
		}
		break;
	case EVENT_VOLTAGE:
		break;
	case EVENT_CALLERID:
		{
			CString strDate;
			CString strCallerID;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			char	szCallerID[256] = {0x00};
			UsbBox_GetCallerNumber(tagMessageInfo.nReference,szCallerID);
			int nItemCnt = m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",tagMessageInfo.nReference+1);
			m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_CALLERID");
			m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strCallerID.Format(L"%S",szCallerID);
			m_EventListCtrl.SetItemText(nItemCnt,3,strCallerID);
		}
		break;
	case EVENT_DTMF:
		{
			CString strDate;
			CString strCallerID;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			char	szCallerID[256] = {0x00};
			UsbBox_GetDtmfNumber(tagMessageInfo.nReference,szCallerID);
			int nItemCnt = m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",tagMessageInfo.nReference+1);
			m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_DTMF");
			m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strCallerID.Format(L"%S",szCallerID);
			m_EventListCtrl.SetItemText(nItemCnt,3,strCallerID);
		}
		break;
	case EVENT_RINGCOUNT:
		break;
	case EVENT_RECORDTIME:
		break;
	case EVENT_NEWRECORDFILE:
		break;
	case EVENT_DEVICEID:
		break;
	case EVENT_DEVICEVER:
		break;
	case EVENT_USBCONNECT:
		{
			CString strDate;
			CString strUsbCount;
			CString strMaxChannel;
			CString strUsbIndex;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = m_EventListCtrl.GetItemCount();
			strUsbCount.Format(L"%d",UsbBox_GetDeviceCount());
			strMaxChannel.Format(L"%d",UsbBox_GetChannelCount());

			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strUsbIndex.Format(L"USB %02d",tagMessageInfo.nReference+1);
			m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_USBCONNECT");
			m_EventListCtrl.SetItemText(nItemCnt,2,strUsbIndex);
			m_EventListCtrl.SetItemText(nItemCnt,3,strUsbCount+L" "+strMaxChannel);
		}
		break;
	case EVENT_USBDISCONNECT:
		{
			CString strDate;
			CString strUsbCount;
			CString strMaxChannel;
			CString strUsbIndex;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = m_EventListCtrl.GetItemCount();
			strUsbCount.Format(L"%d",UsbBox_GetDeviceCount());
			strMaxChannel.Format(L"%d",UsbBox_GetChannelCount());

			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strUsbIndex.Format(L"USB %02d",tagMessageInfo.nReference+1);
			m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_USBDISCONNECT");
			m_EventListCtrl.SetItemText(nItemCnt,2,strUsbIndex);
			m_EventListCtrl.SetItemText(nItemCnt,3,strUsbCount+L" "+strMaxChannel);
		}
		break;
	default:
		break;
	}
	return 0;
}
