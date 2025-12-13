// VR1CallBackDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VR1CallBackDemo.h"
#include "VR1CallBackDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About
CVR1CallBackDemoDlg *pDlg=NULL;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CVR1CallBackDemoDlg dialog
CVR1CallBackDemoDlg::CVR1CallBackDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVR1CallBackDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVR1CallBackDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_EventListCtrl);
}

BEGIN_MESSAGE_MAP(CVR1CallBackDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ENABLE, &CVR1CallBackDemoDlg::OnBnClickedButtonEnable)
	ON_BN_CLICKED(IDC_BUTTON_DISABLE, &CVR1CallBackDemoDlg::OnBnClickedButtonDisable)
END_MESSAGE_MAP()


// CVR1CallBackDemoDlg message handlers

BOOL CVR1CallBackDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	pDlg = this;

	pDlg->m_EventListCtrl.InsertColumn(0,		L"Date/Time",		LVCFMT_LEFT,150,0);
	pDlg->m_EventListCtrl.InsertColumn(1,		L"Event",			LVCFMT_LEFT,200,0);
	pDlg->m_EventListCtrl.InsertColumn(2,		L"Channel",			LVCFMT_LEFT,80,0);
	pDlg->m_EventListCtrl.InsertColumn(3,		L"Remark",			LVCFMT_LEFT,500,0);
	pDlg->m_EventListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES|LVS_EX_SUBITEMIMAGES|LVS_EX_DOUBLEBUFFER); 


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVR1CallBackDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVR1CallBackDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVR1CallBackDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVR1CallBackDemoDlg::OnBnClickedButtonEnable()
{
	UsbBox_EnableDevice();
	UsbBox_EventCallBack(VR1CallBackEvent);							 
	VRBox_SetTelMsg("192.168.22.106","601");
	UsbBox_EnableLog(1);
	GetDlgItem(IDC_BUTTON_ENABLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DISABLE)->EnableWindow(TRUE);
}

void CVR1CallBackDemoDlg::OnBnClickedButtonDisable()
{
	UsbBox_DisableDevice();
	GetDlgItem(IDC_BUTTON_ENABLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DISABLE)->EnableWindow(FALSE);
}

void CALLBACK CVR1CallBackDemoDlg::VR1CallBackEvent(WORD wEventCode, int nReference,DWORD dwParam,DWORD dwDeviceID)
{
	switch ( wEventCode )
	{
	case EVENT_STATE:
		{
			CString strDate;
			CString strState;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_STATE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			switch ( UsbBox_GetState(nReference))
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
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strState);
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
			UsbBox_GetCallerNumber(nReference,szCallerID);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_CALLERID");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strCallerID.Format(L"%S",szCallerID);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strCallerID);
		}
		break;
	case EVENT_DTMF:
		{
			CString strDate;
			CString strCallerID;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			char	szCallerID[256] = {0x00};
			UsbBox_GetDtmfNumber(nReference,szCallerID);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_DTMF");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strCallerID.Format(L"%S",szCallerID);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strCallerID);
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
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strUsbCount.Format(L"%d",UsbBox_GetDeviceCount());
			strMaxChannel.Format(L"%d",UsbBox_GetChannelCount());

			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strUsbIndex.Format(L"USB %02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_USBCONNECT");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strUsbIndex);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strUsbCount+L" "+strMaxChannel);
		}
		break;
	case EVENT_USBDISCONNECT:
		{
			CString strDate;
			CString strUsbCount;
			CString strMaxChannel;
			CString strUsbIndex;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strUsbCount.Format(L"%d",UsbBox_GetDeviceCount());
			strMaxChannel.Format(L"%d",UsbBox_GetChannelCount());

			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strUsbIndex.Format(L"USB %02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_USBDISCONNECT");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strUsbIndex);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strUsbCount+L" "+strMaxChannel);
		}
		break;
	default:
		break;
	}
}