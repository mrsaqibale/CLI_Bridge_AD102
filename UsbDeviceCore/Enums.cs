namespace UsbDeviceCore;

public enum UsbBoxDeviceType
{
    F1 = 0,  // 1 channel USB
    F2 = 1,  // 2 channel USB
    F4 = 2,  // 4 channel USB
    F8 = 3,  // 8 channel USB
    EAR = 4  // Wireless version
}

public enum ChannelState
{
    Offline = 0,
    HookOn = 1,
    HookOff = 2,
    RingOn = 3,
    InRecord = 4,
    OutRecord = 5,
    Sound = 6,
    Mute = 7,
    Unavailable = 255
}

public enum UsbBoxEventCode
{
    State = 0,
    Voltage = 1,
    CallerId = 2,
    Dtmf = 3,
    RingCount = 4,
    RecordTime = 5,
    NewRecordFile = 6,
    StartPlayFile = 7,
    StopPlayFile = 8,
    StartAnswerMachine = 9,
    BusyTone = 10,
    MissedInboundCall = 11,
    DeviceId = 20,
    DeviceVersion = 21,
    UsbConnect = 22,
    UsbDisconnect = 23,
    SocketDial = 50,
    WebSocketDial = 51,
    PostMsg = 52,
    PostFile = 53,
    FtpFile = 54
}

public enum LineStatus
{
    Free,
    Ringing,
    InCall,
    Offhook,
    Busy,
    Unknown
}

public enum ApiFamily
{
    UsbBox,
    AD101,
    AD800
}

