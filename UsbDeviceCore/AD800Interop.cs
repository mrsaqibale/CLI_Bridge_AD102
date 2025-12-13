using System.Runtime.InteropServices;
using System.Text;

namespace UsbDeviceCore;

internal static class AD800Interop
{
    private const string DllName = "AD800Device.dll";

    public delegate void AD800EventCallbackProc(int iChannel, int iEvent, int iParam);
    public delegate void AD800AudioCallbackProc(int iChannel, int AudioIndex, IntPtr pAudioBuff, int AudioLength);
    public delegate void AD800FskProc(int iChannel, IntPtr pFskBuff, int Length);

    public static AD800EventCallbackProc? EventCallbackInstance;
    #pragma warning disable CS0649 // Field is assigned via P/Invoke
    public static AD800AudioCallbackProc? AudioCallbackInstance;
    public static AD800FskProc? FskCallbackInstance;
    #pragma warning restore CS0649

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern bool AD800_Init();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD800_Free();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD800_SetMsgHwnd(IntPtr hWnd);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD800_SetCallbackFun(AD800EventCallbackProc fun);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD800_SetAudioCallbackFun(AD800AudioCallbackProc fun);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void AD800_SetFskCallbackFun(AD800FskProc fun);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD800_GetTotalDev();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD800_GetTotalCh();

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern int AD800_GetChState(int iChannel);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern bool AD800_GetCallerId(int iChannel, StringBuilder pszBuff, uint iLen);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern bool AD800_GetDialed(int iChannel, StringBuilder pszBuff, uint iLen);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern bool AD800_PickUp(int iChannel);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern bool AD800_HangUp(int iChannel);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern bool AD800_SendDTMF(int iChannel, string pszBuff, uint iLen, int iDelayMs);

    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern bool AD800_DialByCard(int iChannel, string pszBuff, uint iLen);
}

