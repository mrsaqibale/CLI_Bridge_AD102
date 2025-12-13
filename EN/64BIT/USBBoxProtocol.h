// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the USBBOXPROTOCOL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// USBBOXPROTOCOL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef USBBOXPROTOCOL_EXPORTS
#define USBBOXPROTOCOL_API __declspec(dllexport)
#else
#define USBBOXPROTOCOL_API __declspec(dllimport)
#endif

typedef void (CALLBACK *USBVOICECALLACK)(BYTE *pbuffer,int nLength,int nReference,int nDeviceIndex,int nAdpcmPacket,DWORD dwDeviceID);	// USB语音数据回传adpcm
typedef void (CALLBACK *USBEVENTCALLBACK)(WORD wEventCode, int nReference,DWORD dwParam,DWORD dwDeviceID);								// USB事件回传

#define USBOBX_MSG		WM_USER+160			// 消息模式

#ifdef  __cplusplus	
extern "C"
{
#endif

	enum
	{
		CH_STATE_OFFLINE=0,						// 未插电话线状态
		CH_STATE_HOOKON,						// 挂机
		CH_STATE_HOOKOFF,						// 提机
		CH_STATE_RINGON ,						// 振铃状态
		CH_STATE_INRECORD,						// 来电录音状态
		CH_STATE_OUTRECORD,						// 去电录音状态
		CH_STATE_SOUND,							// 声控触发
		CH_STATE_MUTE,							// 声控静音

		CH_STATE_UNAVAILABLE       =255,		// 通道不可用”状态
	};

	enum
	{
		EVENT_STATE	= 0,						// 状态变化事件
		EVENT_VOLTAGE = 1,						// 电压变化事件
		EVENT_CALLERID = 2,						// 来电号码事件
		EVENT_DTMF=3,							// DTMF按键事件
		EVENT_RINGCOUNT=4,						// 铃声变化事件
		EVENT_RECORDTIME=5,						// 录音时长事件
		EVENT_NEWRECORDFILE=6,					// 新录音文件事件
		EVENT_STARTPLAYFILE=7,					// 开始播放语音文件
		EVENT_STOPPLAYFILE=8,					// 停止播放语音文件
		EVENT_STARTANSWERMACHINE=9,				// 开始答录机
		EVENT_BUSYTONE=10,						// 侦测到忙音
		EVENT_MISSEDINBOUNDCALL=11,				// 未接来电


		EVENT_DEVICEID	= 20,					// USB 设备ID
		EVENT_DEVICEVER =21,					// USB 设备软件版本
		EVENT_USBCONNECT=22,					// USB 设备连接
		EVENT_USBDISCONNECT=23,					// USB 设备断开

		EVENT_SOCKETDIAL = 50,					// SOCKET拨号
		EVENT_WEBSOCKETDIAL=51,					// WEBSOCKET拨号
		EVENT_POSTMSG=52,						// POST MESSAGE
		EVENT_POSTFILE=53,						// POST FILE
		EVENT_FTPFILE=54,						// FTP FILE
	};

	enum
	{
		USBBOX_TYPE_F1=0,						// 单线USB
		USBBOX_TYPE_F2=1,						// 2线USB
		USBBOX_TYPE_F4=2,						// 4线USB
		USBBOX_TYPE_F8=3,						// 8线USB
		USBBOX_TYPE_EAR=4,						// 单线耳机版本
	};

	enum 
	{
		USBBOX_RECORD_METHOD_VOLTAGE=0,			// 压控录音
		USBBOX_RECORD_METHOD_TRIGGER,			// 声控录音
		USBBOX_RECORD_METHOD_RELAY,				// 极性翻转
	};

	enum
	{
		USBBOX_RECORDFORMAT_WAVE	=0,			// 录音文件WAV格式 IMA-ADPCM 4KB/S
		USBBOX_RECORDFORMAT_MP3,				// 录音文件MP3格式
	};

	typedef struct  _MESSAGE_INFO
	{
		WORD  wEventCode;
		int   nReference;
		DWORD dwParam;
		DWORD dwDeviceID;
	}MESSAGE_INFO, *PMESSAGE_INFO;


	int WINAPI UsbBox_EnableDevice();										// 初始化USB录音设备
	int WINAPI UsbBox_DisableDevice();										// 释放USB录音设备
	int WINAPI UsbBox_SetDeviceType(int nType);								// 设置USB设备类型(1,2,4,8线)
	int WINAPI UsbBox_SetRecordMethod(int nReference,int nMethod);			// 设置USB设备录音类型 压控(默认) 声控
	int WINAPI UsbBox_SetRecordFormat(int nFormat);							// 设置USB设备录音格式(wave mp3)
	int WINAPI UsbBox_VoiceCallBack(USBVOICECALLACK pfun);					// 设定语音回传
	int WINAPI UsbBox_EventCallBack(USBEVENTCALLBACK pfun);					// 设定事件回传(callback模式)
	int WINAPI UsbBox_SetMsg(HWND hMsgWnd);									// 接收消息(消息模式)
	int WINAPI UsbBox_GetVoltage(int nReference);							// 获取线路电压
	int WINAPI UsbBox_GetRingCount(int nReference);							// 获取铃声次数
	int WINAPI UsbBox_GetRecordTime(int nReference);						// 获取录音时长
	int WINAPI UsbBox_GetCallerNumber(int nReference,char *pszCaller);		// 获取来电号码
	int WINAPI UsbBox_GetDtmfNumber(int nReference,char *pszCaller);		// 获取DTMF号码
	int WINAPI UsbBox_ClearCallerNumber(int nReference);					// 清除来电号码
	int WINAPI UsbBox_ClearDtmfNumber(int nReference);						// 清除DTMF号码
	int WINAPI UsbBox_GetState(int nReference);								// 获取线路状态
	int WINAPI UsbBox_Pickup(int nReference);								// 提机
	int WINAPI UsbBox_HookOn(int nReference);								// 挂机
	int WINAPI UsbBox_MicOn(int nReference);								// 麦克风开启(带耳机版本的录音盒功能)
	int WINAPI UsbBox_MicOff(int nReference);								// 麦克风关闭(带耳机版本的录音盒功能)
	int WINAPI UsbBox_Diall(int nReference,char *pszDtmf);					// 拨号
	int WINAPI UsbBox_AutoDial(int nReference,char *pszDtmf);				// 拨号(自动拨号 不需要调用其它的API)
	int WINAPI UsbBox_PlayFile(int nReference,char *pszPlayFile);			// 语音播放
	int WINAPI UsbBox_StopPlay(int nReference);								// 停止播放
	int WINAPI UsbBox_StartRecord(int nReference);							// 开始录音(一般不调用 设备会自动录音)
	int WINAPI UsbBox_StartRecordToFile(int nReference,char *pszFilePath);	// 开始录音(录音到指定文件)
	int WINAPI UsbBox_StopRecord(int nReference);							// 结束录音(一般不调用 设备会自动录音)
	int WINAPI UsbBox_SetAutoRecord(int nReference,int nAutoRecord);		// 设置是否自动录音
	int WINAPI UsbBox_SetRecordFolder(char *pszRecordFolder);				// 设置录音路径(文件夹 例如:D:\\)
	int WINAPI UsbBox_GetRecordFilePath(int nReference,char *pszPath);		// 获取录音文件完整路径
	int WINAPI UsbBox_ClearRecordFilePath(int nReference);					// 清除录音文件路径(建议UsbBox_GetRecordFilePath 获取文件之后调用)
	int WINAPI UsbBox_GetDeviceCount();										// 获取USB设备连接数量
	int WINAPI UsbBox_GetChannelCount();									// 获取USB设备通道最大数量(根据设置的设备类型计算)
	int WINAPI UsbBox_GetDeviceSerial(int nReference);						// 获取USB设备ID
	int WINAPI UsbBox_GetDeviceVersion(int nReference,char *pszVersion);	// 获取USB设备版本
	int WINAPI UsbBox_GetDllVersion(char *pszVersion);						// 获取开发包版本
	int WINAPI UsbBox_Adpcm2PCM(char *pszAdpcm,int nLength,short *pszPCM);	// ADPCM 转 PCM

	/**********************************************************************************************************************************/
	/* 参数设定部分                                                                                                                    */
	/**********************************************************************************************************************************/
	int WINAPI UsbBox_SetVoltage(int nReference,int nMinPickupVoltage,int nMaxPickupVoltage);	// 线路电压设定 
	int WINAPI UsbBox_SetTime(int nReference,int nPickupTime,int nHookOnTime,int nRingTime,int nOffTime,int nRelayTime);// 线路时间参数设定
	int WINAPI UsbBox_StartRecordByPickup(int nReference,int nStartByPickup);					// 提机开始录音
	int WINAPI UsbBox_SetWaitRecordTime(int nReference,int nWaitTime);							// 拨号等待几秒开始录音
	int WINAPI UsbBox_TriggerParam(int nReference,int nSoundTime,int nMuteTime,int nVolume);	// 声控参数
	int WINAPI UsbBox_SetOutNumber(int nReference,char *pszNumber);								// 设置出局号
	int WINAPI UsbBox_SetPlayVolume(int nReference,int nVolume);								// 设置放音音量
	int WINAPI UsbBox_SetRecordVolume(int nReference,int nVolume);								// 设置录音音量
	int WINAPI UsbBox_SetAGC(int nReference,int nAGC);											// 设置AGC

	int WINAPI UsbBox_StopSaveDtmfTime(int nTimeSeconds);										// 停止接收DTMF按键时间
	int WINAPI UsbBox_MissedCallTime(int nTimeMs);												// 承认未接时间
	int WINAPI UsbBox_SoftDialWaitTime(int nTimeSeconds);										// 软拨号等待时间
	int WINAPI UsbBox_DtmfCallerIDTime(int nTimeMs);											// DTMF来电号码等待时间
	int WINAPI UsbBox_EnableRecord(int nReference,int nAutoRecord);								// 是否自动录音

	/*********************************************************************************************************************************/
	/* 网络拨号选用API                                                                                                                */
	/*********************************************************************************************************************************/
	int WINAPI UsbBox_EnableNetDial_TCP(int nPort);												// 开通网络拨号(tcp协议)
	int WINAPI UsbBox_DisableNetDial_TCP();														// 禁止网络拨号(tcp协议)
	int WINAPI UsbBox_EnableNetDial_WS(int nPort);												// 开通网络拨号(websocket协议)
	int WINAPI UsbBox_DisableNetDial_WS();														// 禁止网络拨号(websocket协议)
	int WINAPI UsbBox_GetSocketDialCommand(char *pszCommand);									// 获取socket拨号命令
	int WINAPI UsbBox_GetWSDialCommand(char *pszCommand);										// 获取ws拨号命令

	/*********************************************************************************************************************************/
	/*语音宣告跟答录留言                                                                                                               */
	/*********************************************************************************************************************************/
	int WINAPI UsbBox_SetAnnouncement(int nReference,int nEnable);												// 是否开启来电语音宣告
	int WINAPI UsbBox_SetAnnouncementFile(int nReference,char *pszFilePath);									// 设置语音宣告文件路径
	int WINAPI UsbBox_SetAnswerMachine(int nReference,int nEnable);												// 是否开启答录机功能
	int WINAPI UsbBox_SetAnswerMchineFile(int nReference,char *pszFilePath);									// 答录机语音文件路径
	int WINAPI UsbBox_AnswerMachineTimer(int nReference,int nFromHour,int nFromMin,int nToHour,int nToMin);		// 答录机开启时间范围 HH:mm-HH:mm 20:00-07:30
	int WINAPI UsbBox_AnswerMachineParam(int nReference,int nWaitRingCount,int nMaxSeconds);					// 答录机参数 等待铃声次数  最大留言时间(单位 秒)

	/********************************************************************************************************************************/
	/* POST上传选用API                                                                                                               */
	/********************************************************************************************************************************/
	int WINAPI UsbBox_SetPostMessage(int nEnable);																// 是否开通POST上传消息
	int WINAPI UsbBox_SetPostFile(int nEnable);																	// 是否开通POST上传文件
	int WINAPI UsbBox_SetPostMessageURL(char *pszMessageURL);													// 消息上传接口
	int WINAPI UsbBox_SetPostFileURL(char *pszFileURL);															// 文件上传接口
	int WINAPI UsbBox_SetSystemInfo(char *pszSysName,char *pszSysID);											// 设置POST上传参数 系统名称 系统ID name utf8 format


	/*******************************************************************************************************************************/
	/* FTP上传选用API                                                                                                               */
	/*******************************************************************************************************************************/
	int WINAPI UsbBox_SetFTP(int nEnable);																		// 设置是否开通FTP上传
	int WINAPI UsbBox_SetFTPParam(char *pszHost,char *pszUserName,char *pszPassword,int nPort);					// 设置FTP参数


	// 是否开启dll日志
	int WINAPI UsbBox_EnableLog(int nEnable);


	// VR1 API
	int WINAPI VRBox_SetTelMsg(char *pszIP,char *pszTelNo);


	/************************************************************************/
	/* 以下接口是兼容老的AD800Device.dll 新开发不用管如下接口               */
	/************************************************************************/
	// windows消息
#define	WM_AD800MSG WM_USER + 1800

	// 回调函数定义
	typedef void (CALLBACK *AD800_EVENTCALLBACKPROC)(int iChannel, int iEvent, int iParam);
	typedef void (CALLBACK *AD800_AUDIOCALLBACKPROC)(int iChannel,int AudioIndex,BYTE *pAudioBuff,int AudioLength);
	typedef void (CALLBACK *AD800_FSKPROC)(int iChannel,BYTE *pFskBuff,int Length);


	// 端口状态
	enum AD800_STATUS
	{
		AD800_DEVICE_CONNECTION = 0,	// 设备连接状态

		AD800_LINE_STATUS ,		// 外线状态

		AD800_LINE_VOLTAGE ,	// 外线电压

		AD800_LINE_CALLERID ,	// 外线来电号码

		AD800_LINE_DTMF ,		// 电话机拨号

		AD800_REC_DATA ,		// 录音数据

		AD800_PLAY_FINISHED ,	// 放音完成

		AD800_VOICETRIGGER ,	// 语音触发状态

		AD800_BUSYTONE ,		// 忙音状态

		AD800_DTMF_FINISHED ,	// DTMF发送完成

		// 测试
		AD800_SNERROR ,			// 通信数据序号错误
		AD800_VOICESN_ERROR ,	// 语音数据序号错误
		AD800_RECSTATUS ,		// 录音状态
		AD800_ANALYSEDATA ,		// 显示缓存的数据包,就是没分析的数据包
		AD800_PLAYSTATUS ,		// 放音状态
		AD800_PLAYACK_ERROR ,	// 放音ACK超时
		AD800_BUSY ,			// 占线状态
		AD800_DISPHONE ,		// 话机断开状态
		AD800_EPROM ,			// eeprom操作
		AD800_READSN ,			// 读取设备sn完成
		AD800_READTYPE ,		// 读取设备类型
	};

	// 设备连接状态
	enum AD800_CONNECTION
	{
		AD800DEVICE_DISCONNECTED = 0,	// 断开
		AD800DEVICE_CONNECTED ,			// 连上
	};

	// 外线状态
	enum AD800_LINESTATUS
	{
		AD800LINE_POWEROFF = 0,
		AD800LINE_HOOKOFF ,
		AD800LINE_HOOKON ,
		AD800LINE_RING ,
		AD800LINE_ANC,				// 极性反转(计费信号)
	};

	// 初始化设备
	BOOL WINAPI AD800_Init(void);

	// 释放设备
	void WINAPI AD800_Free(void);


	// 有两种方式收DLL状态，一种是通过消息，别一种是回调函数
	// 设置回送消息句柄
	void WINAPI AD800_SetMsgHwnd(HWND hWnd);

	// 设置回调函数
	void WINAPI AD800_SetCallbackFun(AD800_EVENTCALLBACKPROC fun);
	void WINAPI AD800_SetAudioCallbackFun(AD800_AUDIOCALLBACKPROC fun);
	void WINAPI AD800_SetFskCallbackFun(AD800_FSKPROC fun);

	// 发送命令 - 测试用
	BOOL WINAPI AD800_SendCommand(int iChannel, BYTE *pCmd, UINT iLen);
	BOOL WINAPI AD800_ReadDeviceSN(int iDevice);
	BOOL WINAPI AD800_ReadDeviceType(int iDevice);
	DWORD WINAPI AD800_GetDeviceType(int iDevice);


	// 得到设备数
	int WINAPI AD800_GetTotalDev(void);

	// 得到通道数
	int WINAPI AD800_GetTotalCh(void);


	// 读取外线状态
	int WINAPI AD800_GetChState(int iChannel);

	// 读取来电号码
	BOOL WINAPI AD800_GetCallerId(int iChannel, char *pszBuff, UINT iLen);

	// 读取拨号
	BOOL WINAPI AD800_GetDialed(int iChannel, char *pszBuff, UINT iLen);

	// 读取版本号
	void WINAPI AD800_GetVer(int iDevice, char *pszBuff, UINT iLen);

	// 读取设备序号
	DWORD WINAPI AD800_GetDevSN(int iDevice);

	// 读取铃声次数
	int WINAPI AD800_GetRingCount(int iChannel);


	// 设置/读取 提机，挂机信号侦测参数(默认电压范围是 3 - 20v 3到20之间就认为是提机，大于20就认为是挂机, 侦测时间是提机500ms, 挂机300ms, 掉电200ms)
	BOOL WINAPI AD800_SetHookVoltage(int iChannel, BYTE szHookOffVol, BYTE szHookOnVol);
	BOOL WINAPI AD800_SetHookTime(int iChannel, int iHookOffTime, int iHookOnTime, int iPowerOffTime);
	BOOL WINAPI AD800_SetRingTime(int iChannel,int iRingOnTime,int iRingOffTime);

	void WINAPI AD800_GetHookVoltage(int iChannel, BYTE &szHookOffVol, BYTE &szHookOnVol);
	void WINAPI AD800_GetHookTime(int iChannel, int &iHookOffTime, int &iHookOnTime, int &iPowerOffTime);

	// 设置/读取 来电号码结束时间(默认时间是200ms)
	BOOL WINAPI AD800_SetRevCIDTime(int iChannel, int iCIDTime);
	void WINAPI AD800_GetRevCIDTime(int iChannel, int &iCIDTime);

	// 设置录音和放间音量
	BOOL WINAPI AD800_SetRecVolume(int iChannel, int iVol);
	BOOL WINAPI AD800_SetPlaybackVolume(int iChannel, int iVol);

	// 得到当前的音量值
	int WINAPI AD800_GetRecVolume(int iChannel);
	int WINAPI AD800_GetPlaybackVolume(int iChannel);


	// 开始录音
	BOOL WINAPI AD800_StartFileRec(int iChannel, char *pszFile);
	BOOL WINAPI AD800_StartMemRec(int iChannel, BYTE *pszBuff, UINT iLen);
	int WINAPI AD800_GetMemRecBytes(int iChannel);

	// 停止录音
	int WINAPI AD800_StopRec(int iChannel);


	// 放音
	BOOL WINAPI AD800_PlayFile(int iChannel, char *pszFile);
	BOOL WINAPI AD800_PlayMem(int iChannel, BYTE *pszBuff, UINT iLen);

	// 停止放音
	void WINAPI AD800_StopPlay(int iChannel);


	// 语音触发
	BOOL WINAPI AD800_VoiceTrigger(int iChannel);

	// 停止语音触发
	void WINAPI AD800_StopVoiceTrigger(int iChannel);

	// 语音触发参数
	BOOL WINAPI AD800_SetVoiceThreshold(int iChannel, int iTime, int iLevel);
	BOOL WINAPI AD800_SetSilenceThreshold(int iChannel, int iTime, int iLevel);

	void WINAPI AD800_GetVoiceThreshold(int iChannel, int &iTime, int &iLevel);
	void WINAPI AD800_GetSilenceThreshold(int iChannel, int &iTime, int &iLevel);


	// 忙音侦测
	BOOL WINAPI AD800_DetBusyTone(int iChannel);

	// 停止忙音侦测
	void WINAPI AD800_StopDetBusyTone(int iChannel);


	// 提挂和挂机
	BOOL WINAPI AD800_PickUp(int iChannel);
	BOOL WINAPI AD800_HangUp(int iChannel);

	// 发送DTMF
	BOOL WINAPI AD800_SendDTMF(int iChannel, char *pszBuff, UINT iLen,int iDelayMs);
	BOOL WINAPI AD800_DialByCard(int iChannel,char *pszBuff,UINT iLen);


	BOOL WINAPI AD800_MicMuteOn(int iChannel);		// 麦克风静音
	BOOL WINAPI AD800_MicMuteOff(int iChannel);	// 麦克风取消静音


	// 切断和连接电话机
	BOOL WINAPI AD800_DisconnectPhone(int iChannel);
	BOOL WINAPI AD800_ConnectPhone(int iChannel);


	// AGC控制
	BOOL WINAPI AD800_SetAGC(int iDevice, int iOnOff);



	/************************************************************************/
	/* 以下接口是兼容AD101Device.dll 新开发不用看如下接口                   */
	/************************************************************************/
#define WM_USBLINEMSG   WM_USER + 180

#define MCU_BACKID			0x07		// Return Device ID
#define MCU_BACKSTATE		0x08		// Return Device State
#define MCU_BACKCID			0x09		// Return Device CallerID
#define MCU_BACKDIGIT		0x0A		// Return Device Dial Digit
#define MCU_BACKDEVICE		0x0B		// Return Device Back Device ID
#define MCU_BACKPARAM		0x0C		// Return Device Paramter
#define MCU_BACKCPUID		0x0D		// Return Device CPU ID
#define MCU_BACKCOLLATERAL	0x0E		// Return Collateral phone dialed
#define MCU_BACKDISABLE		0xFF		// Return Device Init
#define MCU_BACKENABLE		0xEE
#define MCU_BACKMISSED		0xAA		// Missed call 
#define MCU_BACKTALK		0xBB		// Start Talk


	enum LINESTATE
	{
		HKONSTATEPRA		= 0x01, // hook on pr+  HOOKON_PRA
		HKONSTATEPRB		= 0x02, // hook on pr-  HOOKON_PRR
		HKONSTATENOPR		= 0x03, // have pr  HAVE_PR
		HKOFFSTATEPRA		= 0x04, // hook off pr+  HOOKOFF_PRA
		HKOFFSTATEPRB		= 0x05, // hook off pr-  HOOKOFF_PRR
		NO_LINE				= 0x06, // no line  NULL_LINE
		RINGONSTATE			= 0x07, // ring on  RING_ON
		RINGOFFSTATE		= 0x08, // ring off RING_OFF
		NOHKPRA				= 0x09, // NOHOOKPRA= 0x09, // no hook pr+
		NOHKPRB				= 0x0a, // NOHOOKPRR= 0x0a, // no hook pr-
		NOHKNOPR			= 0x0b, // NOHOOKNPR= 0x0b, // no hook no pr
	};



		typedef struct _AD101DEVICEPARAMETER
		{     
			int   nRingOn;   
			int   nRingOff;  
			int   nHookOn;   
			int   nHookOff; 
			int   nStopCID;
			int   nNoLine;			// Add this parameter in new AD101(MCU Version is 6.0)
		}AD101DEVICEPARAMETER,*LPAD101DEVICEPARAMETER;  


		////////////////////////////////////////////////////////////////////////////////////

		// LED Status 
		typedef enum
		{
			LED_CLOSE =1,
			LED_RED,
			LED_GREEN,
			LED_YELLOW,
			LED_REDSLOW,
			LED_GREENSLOW,
			LED_YELLOWSLOW,
			LED_REDQUICK,
			LED_GREENQUICK,
			LED_YELLOWQUICK,
		}LEDTYPE;
		//////////////////////////////////////////////////////////////////////////////////////////////

		// Line Status 
		typedef enum
		{
			LINEBUSY =0,
			LINEFREE,
		}ENUMLINEBUSY;

		// DTMF / FSK
		typedef enum
		{
			CID_FSK = 1,
			CID_DTMF ,
		};

		typedef void (CALLBACK *EVENTCALLBACKPROC) (int iLine, int iEvent, int iParam);

		// Init devices
		BOOL WINAPI AD101_InitDevice(const HWND hWnd);

		// Open all the ad101 devices
		int  WINAPI AD101_GetDevice();

		// Free devices 
		void WINAPI AD101_FreeDevice();

		// Get current AD101 device count
		int WINAPI AD101_GetCurDevCount();

		// Change handle of window that uses to receive message 
		void WINAPI AD101_ChangeWindowHandle(const HWND hWnd);

		// Set callback function
		void WINAPI AD101_SetEventCallbackFun(EVENTCALLBACKPROC fun);


		// Show or don't show collateral phone dialed number
		void WINAPI AD101_ShowCollateralPhoneDialed(BOOL bShow = TRUE);


		// Start reading systematic parameter
		BOOL WINAPI AD101_ReadParameter(const int nLine);

		// Get read systematic parameter
		BOOL WINAPI AD101_GetParameter(const int nLine,AD101DEVICEPARAMETER &tagParameter);

		// Set systematic parameter 
		BOOL WINAPI AD101_SetParameter(const int nLine,AD101DEVICEPARAMETER &tagParameter);


		// Get caller id number  
		int  WINAPI AD101_GetCallerID(const int nLine, char *szCallerIDBuffer, char *szName, char *szTime);

		// Get caller id type DTMF / FSK
		int	 WINAPI AD101_GetCallerIDType(const int nLine);

		// Clear caller id number
		int  WINAPI AD101_ClearCallerID(const int nLine);

		// Get dialed number 
		int  WINAPI AD101_GetDialDigit(const int nLine,char *szDialDigitBuffer);

		// Clear dialed number
		int  WINAPI AD101_ClearDialDigit(const int nLine);

		// Get collateral phone dialed number 
		int  WINAPI AD101_GetCollateralDialDigit(const int nLine,char *szDialDigitBuffer);

		// Get last line state 
		int  WINAPI AD101_GetLineState(const int nLine);

		// Get ring count
		int  WINAPI AD101_GetRingIndex(const int nLine);

		// Get talking time 
		int  WINAPI AD101_GetTalkTime(const int nLine);



		// Get cpu of device version 
		int  WINAPI AD101_GetCPUVersion(const int nLine,char *szCPUVersion);

		// Start reading cpu id of device 
		BOOL WINAPI AD101_ReadCPUID(const int nLine);

		// Get readed cpu id of device 
		int  WINAPI AD101_GetCPUID(const int nLine,char *szCPUID);



		// Control led 
		BOOL WINAPI AD101_SetLED(const int nLine,LEDTYPE enumLed);

		// Control line connected with ad101 device to busy or idel.
		BOOL WINAPI AD101_SetBusy(const int nLine,ENUMLINEBUSY enumLineBusy);

		// Set line to start talking than start timer
		void WINAPI AD101_SetLineStartTalk(const int nLine);

		// Set time to start talking after dialed number 
		void WINAPI AD101_SetDialOutStartTalkingTime(int nSecond);

		// Set ring end time
		void WINAPI AD101_SetRingOffTime(int nSecond);





































#ifdef  __cplusplus	
}
#endif