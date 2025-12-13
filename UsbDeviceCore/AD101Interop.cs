using System.Runtime.InteropServices;
using System.Text;

namespace UsbDeviceCore;

internal static class AD101Interop
{
    private const string DllName = "AD101Device.dll";

    public delegate void AD101EventCallbackProc(int iLine, int iEvent, int iParam);

    public static AD101EventCallbackProc? EventCallbackInstance;

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern bool AD101_InitDevice(IntPtr hWnd);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_GetDevice();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD101_FreeDevice();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_GetCurDevCount();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD101_ChangeWindowHandle(IntPtr hWnd);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD101_SetEventCallbackFun(AD101EventCallbackProc fun);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int AD101_GetCallerID(int nLine, StringBuilder szCallerIDBuffer, StringBuilder szName, StringBuilder szTime);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_GetCallerIDType(int nLine);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_ClearCallerID(int nLine);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int AD101_GetDialDigit(int nLine, StringBuilder szDialDigitBuffer);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_ClearDialDigit(int nLine);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_GetLineState(int nLine);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_GetRingIndex(int nLine);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD101_GetTalkTime(int nLine);
}

