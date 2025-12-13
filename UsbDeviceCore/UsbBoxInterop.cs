using System.Runtime.InteropServices;
using System.Text;

namespace UsbDeviceCore;

internal static class UsbBoxInterop
{
    private const string DllName = "USBBoxProtocol.dll";

    // Callback delegates - must be kept alive to prevent GC
    public delegate void UsbVoiceCallback(IntPtr buffer, int length, int reference, int deviceIndex, int adpcmPacket, uint deviceId);
    public delegate void UsbEventCallback(ushort eventCode, int reference, uint param, uint deviceId);

    // Keep callbacks alive
    public static UsbVoiceCallback? VoiceCallbackInstance;
    public static UsbEventCallback? EventCallbackInstance;

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_EnableDevice();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_DisableDevice();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_SetDeviceType(int nType);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_SetRecordMethod(int nReference, int nMethod);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_SetRecordFormat(int nFormat);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_VoiceCallBack(UsbVoiceCallback pfun);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_EventCallBack(UsbEventCallback pfun);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_SetMsg(IntPtr hMsgWnd);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_GetVoltage(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_GetRingCount(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_GetRecordTime(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_GetCallerNumber(int nReference, StringBuilder pszCaller);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_GetDtmfNumber(int nReference, StringBuilder pszCaller);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_ClearCallerNumber(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_ClearDtmfNumber(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_GetState(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_Pickup(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_HookOn(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_MicOn(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_MicOff(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_Diall(int nReference, string pszDtmf);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_AutoDial(int nReference, string pszDtmf);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_PlayFile(int nReference, string pszPlayFile);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_StopPlay(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_StartRecord(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_StartRecordToFile(int nReference, string pszFilePath);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_StopRecord(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_SetAutoRecord(int nReference, int nAutoRecord);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_SetRecordFolder(string pszRecordFolder);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_GetRecordFilePath(int nReference, StringBuilder pszPath);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_GetDeviceCount();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_GetChannelCount();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int UsbBox_GetDeviceSerial(int nReference);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_GetDeviceVersion(int nReference, StringBuilder pszVersion);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int UsbBox_GetDllVersion(StringBuilder pszVersion);
}

