// UsbTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UsbTest.h"
#include "UsbTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About
CUsbTestDlg *pDlg = NULL;
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

// CUsbTestDlg dialog
CUsbTestDlg::CUsbTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUsbTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bMonitor = FALSE;
	m_nMonitorReference = 0xFF;
	m_nMonitorLength = 0;
	memset(m_szMonitorBufer,0x00,sizeof(m_szMonitorBufer));

	memset(m_PCM16Buff,	0x00,sizeof(m_PCM16Buff));
	m_PenG.CreatePen ( PS_SOLID, 0, RGB(0, 255, 0) );
	m_brsBkGnd.CreateSolidBrush ( RGB(0,0,0) );
}

void CUsbTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_EventListCtrl);
}

BEGIN_MESSAGE_MAP(CUsbTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ENABLE,		&CUsbTestDlg::OnBnClickedButtonEnable)
	ON_BN_CLICKED(IDC_BUTTON_DISABLE,		&CUsbTestDlg::OnBnClickedButtonDisable)
	ON_BN_CLICKED(IDC_BUTTON_SETTYPE,		&CUsbTestDlg::OnBnClickedButtonSettype)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR,			&CUsbTestDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_PICKUP,		&CUsbTestDlg::OnBnClickedButtonPickup)
	ON_BN_CLICKED(IDC_BUTTON_HOOKON,		&CUsbTestDlg::OnBnClickedButtonHookon)
	ON_BN_CLICKED(IDC_BUTTON_AUTOCALL,		&CUsbTestDlg::OnBnClickedButtonAutocall)
	ON_BN_CLICKED(IDC_BUTTON_MICON,			&CUsbTestDlg::OnBnClickedButtonMicon)
	ON_BN_CLICKED(IDC_BUTTON_MICOFF,		&CUsbTestDlg::OnBnClickedButtonMicoff)
	ON_BN_CLICKED(IDC_BUTTON_GETUSBCOUNT,	&CUsbTestDlg::OnBnClickedButtonGetusbcount)
	ON_BN_CLICKED(IDC_BUTTON_GETCHCOUNT,	&CUsbTestDlg::OnBnClickedButtonGetchcount)
	ON_MESSAGE(USBOBX_MSG,					&CUsbTestDlg::OnUsbBoxDeviceMessage)
	ON_MESSAGE(WM_AD800MSG,					&CUsbTestDlg::OnAD800Message)
	ON_MESSAGE(WM_USBLINEMSG,				&CUsbTestDlg::OnAD101Message)
	ON_BN_CLICKED(IDC_BUTTON_MONITOR,		&CUsbTestDlg::OnBnClickedButtonMonitor)
	ON_BN_CLICKED(IDC_BUTTON_STOPMONITOR,	&CUsbTestDlg::OnBnClickedButtonStopmonitor)
	ON_BN_CLICKED(IDC_BUTTON_OLDDLL_START,  &CUsbTestDlg::OnBnClickedButtonOlddllStart)
	ON_BN_CLICKED(IDC_BUTTON_OLDDLL_STOP,  &CUsbTestDlg::OnBnClickedButtonOlddllStop)
	ON_BN_CLICKED(IDC_BUTTON_OLDDLL_START2, &CUsbTestDlg::OnBnClickedButtonOlddllStart2)
	ON_BN_CLICKED(IDC_BUTTON_OLDDLL_STOP2, &CUsbTestDlg::OnBnClickedButtonOlddllStop2)
END_MESSAGE_MAP()


// CUsbTestDlg message handlers
BOOL CUsbTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	pDlg = this;
	m_EventListCtrl.InsertColumn(0,		L"Date/Time",		LVCFMT_LEFT,150,0);
	m_EventListCtrl.InsertColumn(1,		L"Event",			LVCFMT_LEFT,200,0);
	m_EventListCtrl.InsertColumn(2,		L"Channel",			LVCFMT_LEFT,80,0);
	m_EventListCtrl.InsertColumn(3,		L"Remark",			LVCFMT_LEFT,500,0);
	m_EventListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES|LVS_EX_SUBITEMIMAGES|LVS_EX_DOUBLEBUFFER); 

	WINDOWPLACEMENT wndpl; 
	GetDlgItem(IDC_STATIC_WAVE)->GetWindowPlacement( &wndpl );
	m_SoundPicture.left		= 0;
	m_SoundPicture.top		= 0;
	m_SoundPicture.right	= wndpl.rcNormalPosition.right -wndpl.rcNormalPosition.left ;
	m_SoundPicture.bottom	= wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;

	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO1);
	comboBox->SetCurSel(USBBOX_TYPE_EAR);
	comboBox =(CComboBox*)GetDlgItem(IDC_COMBO2);
	comboBox->SetCurSel(0);

	SetDlgItemText(IDC_EDIT_DTMFNUMBER,L"123456789");

	CString str;GetWindowText(str);
#ifdef _M_IX86	
	str += L" 32BIT";
#else	
	str += L" 64BIT";
#endif

	SetWindowText(str);
	return TRUE;  
}

void CUsbTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUsbTestDlg::OnPaint()
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

HCURSOR CUsbTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUsbTestDlg::OnBnClickedButtonEnable()
{
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO1);
	
	// 特别注意调用顺序
	UsbBox_EnableDevice();
	UsbBox_EventCallBack(USBEventCallBack);					// set event callback function
//	UsbBox_SetMsg(m_hWnd);									// usb box message sdk
	UsbBox_VoiceCallBack(USBVoiceCallBack);					// set audio datapack callback function

	UsbBox_SetDeviceType(comboBox->GetCurSel());			// set device type
	UsbBox_SetRecordFormat(USBBOX_RECORDFORMAT_WAVE);		// set wave record
	UsbBox_SetRecordFolder("D:\\");							// set record folder
	
	for ( int nReference=0;nReference<32;nReference++)
	{
		UsbBox_SetRecordMethod(nReference,USBBOX_RECORD_METHOD_VOLTAGE);	// set voltage record

		UsbBox_SetVoltage(nReference,3,20);
		UsbBox_SetTime(nReference,80,100,250,1000,100);
		UsbBox_StartRecordByPickup(nReference,0);
		UsbBox_SetWaitRecordTime(nReference,3);// dial any dtmf then start record after 3 seconds
		UsbBox_TriggerParam(nReference,100,5000,10);
		UsbBox_SetOutNumber(nReference,"");
		UsbBox_SetPlayVolume(nReference,10);
		UsbBox_SetRecordVolume(nReference,10);
		UsbBox_SetAGC(nReference,1);
		UsbBox_EnableRecord(nReference,0);

	//	UsbBox_SetAnnouncement(nReference,1);
	//	UsbBox_SetAnnouncementFile(nReference,"c:\\来电语音宣告.wav");
	//	UsbBox_SetAnswerMachine(nReference,1);
	//	UsbBox_SetAnswerMchineFile(nReference,"c:\\留言提示音.wav");
	//	UsbBox_AnswerMachineTimer(nReference,20,0,7,30);// 20:00-07:30
	//	UsbBox_AnswerMachineParam(nReference,3,60);
	}

	UsbBox_StopSaveDtmfTime(7);
	UsbBox_MissedCallTime(7000);
	UsbBox_SoftDialWaitTime(10);
	UsbBox_DtmfCallerIDTime(1000);

	// disable or enable
	UsbBox_EnableNetDial_TCP(4700);
	UsbBox_EnableNetDial_WS(1818);

//	UsbBox_SetPostMessage(1);
//	UsbBox_SetPostFile(1);
//	UsbBox_SetSystemInfo("usbbox","88996655");
//	UsbBox_SetPostMessageURL("http://192.168.22.91:80/api/Event?");
//	UsbBox_SetPostFileURL("http://192.168.22.91:80/api/Files");

	UsbBox_SetFTP(1);
	UsbBox_SetFTPParam("192.168.22.13","ken","1234",21);

	GetDlgItem(IDC_BUTTON_ENABLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DISABLE)->EnableWindow(TRUE);
}

void CUsbTestDlg::OnBnClickedButtonDisable()
{
	UsbBox_DisableDevice();
	GetDlgItem(IDC_BUTTON_ENABLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DISABLE)->EnableWindow(FALSE);
}

void CUsbTestDlg::OnBnClickedButtonSettype()
{
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO1);
	UsbBox_SetDeviceType(comboBox->GetCurSel());
}

void CUsbTestDlg::OnBnClickedButtonClear()
{
	m_EventListCtrl.DeleteAllItems();
}

void CUsbTestDlg::OnBnClickedButtonPickup()
{
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO2);
	UsbBox_Pickup(comboBox->GetCurSel());
}

void CUsbTestDlg::OnBnClickedButtonHookon()
{
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO2);
	UsbBox_HookOn(comboBox->GetCurSel());
}

void CUsbTestDlg::OnBnClickedButtonAutocall()
{
	char szDtmf[32] = {0x00};
	GetDlgItemTextA(m_hWnd,IDC_EDIT_DTMFNUMBER,szDtmf,32);
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO2);
	UsbBox_AutoDial(comboBox->GetCurSel(),szDtmf);
}

void CUsbTestDlg::OnBnClickedButtonMicon()
{
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO2);
	UsbBox_MicOn(comboBox->GetCurSel());
}

void CUsbTestDlg::OnBnClickedButtonMicoff()
{
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO2);
	UsbBox_MicOff(comboBox->GetCurSel());
}

void CUsbTestDlg::OnBnClickedButtonGetusbcount()
{
	CString strUsbCount;
	strUsbCount.Format(L"%d",UsbBox_GetDeviceCount());
	SetDlgItemText(IDC_EDIT_USBCOUNT,strUsbCount);
}

void CUsbTestDlg::OnBnClickedButtonGetchcount()
{
	CString strMaxChannelCount;
	strMaxChannelCount.Format(L"%d",UsbBox_GetChannelCount());
	SetDlgItemText(IDC_EDIT_CHANNELCOUNT,strMaxChannelCount);
}

void CUsbTestDlg::OnBnClickedButtonMonitor()
{
	CComboBox *comboBox =(CComboBox*)GetDlgItem(IDC_COMBO2);
	m_nMonitorReference = comboBox->GetCurSel();
	m_nMonitorLength = 0;
	m_bMonitor = TRUE;
	memset(m_szMonitorBufer,0x00,sizeof(m_szMonitorBufer));
	
	wchar_t szFilePath[MAX_PATH] = {0x00};
	wsprintf(szFilePath,L"D:\\monitor_ch%d.pcm",m_nMonitorReference+1);
	DeleteFile(szFilePath);
}

void CUsbTestDlg::OnBnClickedButtonStopmonitor()
{
	m_nMonitorReference = 0xFF;
	m_bMonitor = FALSE;
	m_nMonitorLength = 0;
	memset(m_szMonitorBufer,0x00,sizeof(m_szMonitorBufer));
}



void CUsbTestDlg::OnDestroy()
{
	CDialog::OnDestroy();
	UsbBox_DisableDevice();
	m_PenG.DeleteObject();
	m_brsBkGnd.DeleteObject();
}

void CALLBACK CUsbTestDlg::USBVoiceCallBack(BYTE *pbuffer,int nLength,int nReference,int nDeviceIndex,int nAdpcm,DWORD dwDeviceID)
{
	// audio datapack
	if ( !pDlg->m_bMonitor ) return ;

	if ( pDlg->m_nMonitorLength == 0 && nAdpcm != 0 )
	{
		return ;
	}

	if ( pDlg->m_nMonitorReference != nReference )
	{
		return;
	}

	memcpy(pDlg->m_szMonitorBufer+pDlg->m_nMonitorLength,pbuffer,nLength);
	pDlg->m_nMonitorLength += nLength;

	if ( pDlg->m_nMonitorLength >= 2048 )
	{
		// adpcm to pcm
		UsbBox_Adpcm2PCM((char*)pDlg->m_szMonitorBufer,pDlg->m_nMonitorLength,pDlg->m_PCM16Buff);

		pDlg->DrawSoundPicture(pDlg->m_PCM16Buff,pDlg->m_nMonitorLength/2);

		// save pcm 
 		wchar_t szFilePath[MAX_PATH] = {0x00};
		wsprintf(szFilePath,L"D:\\monitor_ch%d.pcm",nReference+1);
		HANDLE hFile = CreateFile(szFilePath,GENERIC_READ|GENERIC_WRITE,	FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
		if ( INVALID_HANDLE_VALUE != hFile )
		{
			DWORD dwWrite = 0x00;
			SetFilePointer(hFile,NULL,0,FILE_END);
			WriteFile(hFile,(BYTE*)pDlg->m_PCM16Buff,pDlg->m_nMonitorLength*4,&dwWrite,0L);
			CloseHandle(hFile);
		}

		pDlg->m_nMonitorLength = 0;
		memset(pDlg->m_szMonitorBufer,0x00,sizeof(pDlg->m_szMonitorBufer));
		memset(pDlg->m_PCM16Buff,0x00,sizeof(pDlg->m_PCM16Buff));
	}
}

void CALLBACK CUsbTestDlg::USBEventCallBack(WORD wEventCode, int nReference,DWORD dwParam,DWORD dwDeviceID)
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
			case CH_STATE_OFFLINE:
				strState =L"Empty";	
				break;
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
				strState =L"Inbound Record";	
				break;
			case CH_STATE_OUTRECORD:
				strState =L"Outbound Record";	
				break;
			case CH_STATE_SOUND:
				strState =L"Trigger";	
				break;
			case CH_STATE_MUTE:
				strState =L"Mute";	
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
		{
			CString strDate;
			CString strVoltage;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_VOLTAGE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strVoltage.Format(L"%02d V",UsbBox_GetVoltage(nReference));
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strVoltage);
		}
		break;
	case EVENT_CALLERID:
		{
			CString strDate;
			CString strCallerID;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			char	szCallerID[64] = {0x00};
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
			CString strDTMF;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			char	szDtmf[64] = {0x00};
			UsbBox_GetDtmfNumber(nReference,szDtmf);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_DTMF");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strDTMF.Format(L"%S",szDtmf);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strDTMF);
		}
		break;
	case EVENT_RINGCOUNT:
		{
			CString strDate;
			CString strRingCount;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_RINGCOUNT");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strRingCount.Format(L"%02d",UsbBox_GetRingCount(nReference));
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strRingCount);
		}
		break;
	case EVENT_RECORDTIME:
		{
			CString strDate;
			CString strRecordTime;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_RECORDTIME");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strRecordTime.Format(L"%d ms",UsbBox_GetRecordTime(nReference));
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strRecordTime);
		}
		break;
	case EVENT_NEWRECORDFILE:
		{
			CString strDate;
			CString strFilePath;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			char	szFilePath[MAX_PATH] = {0x00};
			UsbBox_GetRecordFilePath(nReference,szFilePath);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_NEWRECORDFILE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strFilePath.Format(L"%S",szFilePath);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strFilePath);

		}
		break;
	case EVENT_STARTPLAYFILE:
		{
			CString strDate;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_STARTPLAYFILE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
		}
		break;
	case EVENT_STOPPLAYFILE:
		{
			CString strDate;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_STOPPLAYFILE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
		}
		break;
	case EVENT_STARTANSWERMACHINE:
		{
			CString strDate;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_STARTANSWERMACHINE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
		}
		break;
	case EVENT_BUSYTONE:
		{
			CString strDate;
			CString strCH;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_BUSYTONE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
		}
		break;
	case EVENT_MISSEDINBOUNDCALL:
		{
			CString strDate;
			CString strCallerID;
			CString strCH;
  
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_MISSEDINBOUNDCALL");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
		}
		break;

	case EVENT_DEVICEID:
		{
			CString strDate;
			CString strID;
			CString strUsbIndex;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strUsbIndex.Format(L"USB %02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_DEVICEID");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strUsbIndex);
			strID.Format(L"%08d",UsbBox_GetDeviceSerial(nReference));
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strID);
		}
		break;
	case EVENT_DEVICEVER:
		{
			CString strDate;
			CString strVer;
			CString strUsbIndex;
			char	szVer[32]	= {0x00};
			SYSTEMTIME st;GetLocalTime(&st);
			UsbBox_GetDeviceVersion(nReference,szVer);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strUsbIndex.Format(L"USB %02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_DEVICEVER");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strUsbIndex);
			strVer.Format(L"%S",szVer);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strVer);
		}
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
			pDlg->SetDlgItemText(IDC_EDIT_USBCOUNT,strUsbCount);
			pDlg->SetDlgItemText(IDC_EDIT_CHANNELCOUNT,strMaxChannel);

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
			pDlg->SetDlgItemText(IDC_EDIT_USBCOUNT,strUsbCount);
			pDlg->SetDlgItemText(IDC_EDIT_CHANNELCOUNT,strMaxChannel);
		}
		break;
	case EVENT_SOCKETDIAL:
		{
			char szsocketCommand[512] = {0x00};
			CString strDate;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			UsbBox_GetSocketDialCommand(szsocketCommand);
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_SOCKETDIAL");
		}
		break;
	case EVENT_WEBSOCKETDIAL:
		{
			char szwsCommand[512] = {0x00};
			CString strDate;
			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			UsbBox_GetWSDialCommand(szwsCommand);
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_WEBSOCKETDIAL");
		}
		break;

	case EVENT_POSTMSG:
		{
			CString strDate;
 			CString strCH;

			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_POSTMSG");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);

			if ( 0 == dwParam ) pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,L"success");
			else				pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,L"fail");
		}
		break;

	case EVENT_POSTFILE:
		{
			CString strDate;
 			CString strCH;

			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_POSTFILE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);

			if ( 0 == dwParam ) pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,L"success");
			else				pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,L"fail");
		}
		break;

	case EVENT_FTPFILE:
		{
			CString strDate;
			CString strCH;

			SYSTEMTIME st;GetLocalTime(&st);
			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nReference+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"EVENT_FTPFILE");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);

			if ( 0 == dwParam ) pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,L"success");
			else				pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,L"fail");
		}
		break;

	default:
		break;
	}
}

LRESULT CUsbTestDlg::OnUsbBoxDeviceMessage(WPARAM wParam,LPARAM lParam)
{
	MESSAGE_INFO tagMessageInfo;memcpy(&tagMessageInfo,(MESSAGE_INFO*)wParam,sizeof(MESSAGE_INFO));
	switch ( tagMessageInfo.wEventCode )
	{
	case EVENT_STATE:
		break;
	case EVENT_VOLTAGE:
		break;
	case EVENT_CALLERID:
		break;
	case EVENT_DTMF:
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
		break;
	case EVENT_USBDISCONNECT:
		break;
	default:
		break;
	}
	return 0;
}


void CUsbTestDlg::DrawSoundPicture(short *pShortData,DWORD dwDrawBytes)
{
	// 计算每一秒钟所占的区域.在1秒的区域内画图
	CDC *pWndDC = GetDlgItem(IDC_STATIC_WAVE)->GetDC();
	if ( NULL == pWndDC ) return ;

	m_pOldInputPen	= (CPen*)pWndDC->SelectObject ( &m_PenG );

	pWndDC->SetBkMode ( TRANSPARENT );
	pWndDC->FillRect ( &m_SoundPicture, &m_brsBkGnd);
	pWndDC->Draw3dRect ( &m_SoundPicture, RGB(128,64,0), RGB(128,64,0) );

	int nCenterY = m_SoundPicture.CenterPoint().y;
	int y		 = nCenterY + (int) ( pShortData[0] * m_SoundPicture.Height() / 0xffff );

	pWndDC->MoveTo ( 0, y );

	float fStep		= (float)m_SoundPicture.Width() / (float)(dwDrawBytes);
	float fLineX	= 0;

	for ( DWORD i=1; i<dwDrawBytes; i++ )
	{
		fLineX += fStep;
		y = nCenterY + (int) ( pShortData[i] * m_SoundPicture.Height() / 0xffff );
		pWndDC->LineTo ( (int)fLineX, y );
	}

	pWndDC->SelectObject(m_pOldInputPen);
	ReleaseDC(pWndDC);
}



void CUsbTestDlg::OnBnClickedButtonOlddllStart()
{
	AD800_Init();
	AD800_SetMsgHwnd(m_hWnd);
	GetDlgItem(IDC_BUTTON_OLDDLL_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_OLDDLL_STOP)->EnableWindow(TRUE);
}

void CUsbTestDlg::OnBnClickedButtonOlddllStop()
{
	AD800_Free();
	GetDlgItem(IDC_BUTTON_OLDDLL_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_OLDDLL_STOP)->EnableWindow(FALSE);
}

LRESULT CUsbTestDlg::OnAD800Message(WPARAM wParam,LPARAM lParam)
{
	int iChannel = HIWORD(wParam);
	int iEvent = LOWORD(wParam);

	switch ( iEvent )
	{
	case AD800_DEVICE_CONNECTION:
		if ( AD800DEVICE_CONNECTED == lParam )
		{
			int iDev = iChannel;
			char szVer[32] = {0};
			AD800_GetVer(iDev, szVer, 20);

			CString strSN;
			strSN.Format(L"%d", AD800_GetDevSN(iDev));

			::OutputDebugString(L"Connected\r\n");
		}

		else
		{
			::OutputDebugString(L"Disconnected\r\n");
		}
		break;

	case AD800_LINE_STATUS:
		{
			switch ( lParam )
			{
			case AD800LINE_POWEROFF:
				{
					CString strText;
					strText.Format(L"Channel %d state=no line\r\n",iChannel+1);
					::OutputDebugString(strText);
				}
				break;
			case AD800LINE_HOOKOFF:
				{
					CString strText;
					strText.Format(L"Channel %d state=hook off\r\n",iChannel+1);
					::OutputDebugString(strText);
				}
				break;
			case AD800LINE_HOOKON:
				{
					CString strText;
					strText.Format(L"Channel %d state=hook on\r\n",iChannel+1);
					::OutputDebugString(strText);
				}
				break;

			case AD800LINE_RING:
				{
					CString strText;
					strText.Format(L"Channel %d state=ring\r\n",iChannel+1);
					::OutputDebugString(strText);
				}
				break;

			case AD800LINE_ANC:
				break;

			default:
				break;	
			}
		}
		break;

	case AD800_LINE_VOLTAGE: // 外线电压
		{
			CString strText;
			strText.Format(L"Channel %d Voltage:%d V\r\n", iChannel+1,lParam);
			::OutputDebugString(strText);
		}
		break;

	case AD800_LINE_CALLERID:
		{
			CString strText;
			char szBuff[64] = {0};
			AD800_GetCallerId(iChannel, szBuff, 64);
			strText.Format(L"Channel %d callerid:%S\r\n",iChannel+1,szBuff);
			::OutputDebugString(strText);
		}
		break;

	case AD800_LINE_DTMF:
		{
			CString strText;
			char szBuff[64] = {0};
			AD800_GetDialed(iChannel, szBuff, 64);
			strText.Format(L"Channel %d dtmf:%S\r\n",iChannel+1,szBuff);
			::OutputDebugString(strText);
		}
		break;

	case AD800_SNERROR:
		{
		}
		break;

	case AD800_VOICESN_ERROR:
		{
		}
		break;

	case AD800_RECSTATUS:
		{
		}
		break;


	case AD800_ANALYSEDATA:
		{
		}
		break;


	case AD800_PLAYSTATUS:
		{
		}
		break;

	case AD800_PLAYACK_ERROR:
		break;

	case AD800_PLAY_FINISHED:
		break;

	case AD800_BUSY:
		break;

	case AD800_DISPHONE:
		break;

	case AD800_EPROM:
		break;

	case AD800_VOICETRIGGER:
		break;

	case AD800_BUSYTONE:
		break;

	case AD800_DTMF_FINISHED:
		break;

	case AD800_REC_DATA:
		break;

	case AD800_READSN:
		break;

	case AD800_READTYPE:
		break;

	default:
		break;
	}

	return 0L;
}


LRESULT CUsbTestDlg::OnAD101Message(WPARAM wParam,LPARAM lParam)
{
	int  nMsg   = LOWORD(wParam);
	int  nLine  = HIWORD(wParam);

	switch(nMsg)
	{
	case MCU_BACKDISABLE:
		break;

	case MCU_BACKENABLE:
		break;

	case MCU_BACKID:
		{
		}
		break;

	case MCU_BACKSTATE:
		{
			switch (lParam)
			{
			case HKONSTATEPRA:
			//	list->SetItemText(nLine,3,"HOOK ON PR+");
				break;

			case HKONSTATEPRB:
			//	list->SetItemText(nLine,3,"HOOK ON PR-");
				break;

			case HKONSTATENOPR:
			//	list->SetItemText(nLine,3,"HOOK ON NOPR");
				break;

			case HKOFFSTATEPRA:
				{
				//	list->SetItemText(nLine,3,"HOOK OFF PR+");
				}
				break;

			case HKOFFSTATEPRB:
				{
				}
				break;

			case NO_LINE:
				{
				}
				break;

			case RINGONSTATE:
				{
				}
				break;

			case RINGOFFSTATE:
				break;

			case NOHKPRA:
				break;

			case NOHKPRB:
				break;

			case NOHKNOPR:
				{
				}

				break;

			default:
				break;
			}
		}
		break;

	case MCU_BACKCID:
		{
			SYSTEMTIME st;GetLocalTime(&st);
			CString strDate;
			CString strCH;
			CString strCallerID;

			char szCallerID[128];
			char szName[128];
			char szTime[128];

			memset(szCallerID,0x00,sizeof(szCallerID));
			memset(szName,0x00,sizeof(szName));
			memset(szTime,0x00,sizeof(szTime));

			int nLen = AD101_GetCallerID(nLine,szCallerID,szName,szTime);

			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nLine+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"AD101_MCU_BACKCID");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strCallerID.Format(L"%S",szCallerID);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strCallerID);

		}
		break;

	case MCU_BACKDIGIT:
		{
			char szDialDigit[128];
			memset(szDialDigit,0x00,sizeof(szDialDigit));
			int nLen =AD101_GetDialDigit(nLine,szDialDigit);


			SYSTEMTIME st;GetLocalTime(&st);
			CString strDate;
			CString strCH;
			CString strCallerID;
 

			int nItemCnt = pDlg->m_EventListCtrl.GetItemCount();
			strDate.Format(L"%04d%02d%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			strCH.Format(L"%02d",nLine+1);
			pDlg->m_EventListCtrl.InsertItem(nItemCnt,strDate,0);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,1,L"AD101_MCU_BACKDIGIT");
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,2,strCH);
			strCallerID.Format(L"%S",szDialDigit);
			pDlg->m_EventListCtrl.SetItemText(nItemCnt,3,strCallerID);

		}
		break;


	case MCU_BACKCOLLATERAL:
		{
		}
		break;

	case MCU_BACKDEVICE:
		{
		}
		break;

	case MCU_BACKPARAM:
		{
		}
		break;

	case MCU_BACKCPUID:
		{
		}
		break;

	case MCU_BACKMISSED:
		{
		}
		break;

	case MCU_BACKTALK:
		{
		}
		break;

	default:
		break;
	}

	return 0L;
}


void CUsbTestDlg::OnBnClickedButtonOlddllStart2()
{
	if ( !AD101_InitDevice(m_hWnd) )
	{
		MessageBox(L"AD101_InitDevice ERROR",L"ERROR",MB_ICONERROR);
		GetDlgItem(IDC_BUTTON_OLDDLL_START2)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_OLDDLL_STOP2)->EnableWindow(FALSE);
	}
	else
	{
		MessageBox(L"AD101_InitDevice OK",L"OK",MB_ICONINFORMATION);
		GetDlgItem(IDC_BUTTON_OLDDLL_START2)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_OLDDLL_STOP2)->EnableWindow(TRUE);
	}
}



void CUsbTestDlg::OnBnClickedButtonOlddllStop2()
{
	AD101_FreeDevice();
}
