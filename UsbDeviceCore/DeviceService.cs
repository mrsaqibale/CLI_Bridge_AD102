using System.Collections.Concurrent;
using System.Text;

namespace UsbDeviceCore;

public class DeviceService
{
    private readonly ConcurrentDictionary<int, LineInfo> _lines = new();
    private bool _usbBoxEnabled;
    private bool _ad101Enabled;
    private bool _ad800Enabled;
    private UsbBoxDeviceType _currentDeviceType = UsbBoxDeviceType.F2;

    public event EventHandler<DeviceEvent>? OnDeviceEvent;

    private bool _callbacksRegistered = false;

    public DeviceService()
    {
        // Don't register callbacks in constructor - do it lazily when device is enabled
    }

    private void RegisterUsbBoxCallbacks()
    {
        if (_callbacksRegistered) return;

        try
        {
            UsbBoxInterop.UsbEventCallback eventCallback = (eventCode, reference, param, deviceId) =>
            {
                var evt = new DeviceEvent
                {
                    ApiFamily = ApiFamily.UsbBox,
                    EventCode = eventCode,
                    Line = reference,
                    Param = param,
                    DeviceId = deviceId,
                    Timestamp = DateTime.Now
                };

                UpdateLineInfo(reference, evt);
                ParseEvent(evt);
                OnDeviceEvent?.Invoke(this, evt);
            };

            UsbBoxInterop.UsbVoiceCallback voiceCallback = (buffer, length, reference, deviceIndex, adpcmPacket, deviceId) =>
            {
                // Voice data callback - can be extended later
            };

            UsbBoxInterop.EventCallbackInstance = eventCallback;
            UsbBoxInterop.VoiceCallbackInstance = voiceCallback;

            UsbBoxInterop.UsbBox_EventCallBack(eventCallback);
            UsbBoxInterop.UsbBox_VoiceCallBack(voiceCallback);
            _callbacksRegistered = true;
        }
        catch (DllNotFoundException)
        {
            // DLL not found - will be registered when device is enabled
        }
        catch (System.Exception)
        {
            // Other errors - will be registered when device is enabled
        }
    }

    private void UpdateLineInfo(int line, DeviceEvent evt)
    {
        var lineInfo = _lines.GetOrAdd(line, _ => new LineInfo { LineIndex = line });

        lineInfo.LastEventCode = evt.EventCode;

        switch ((UsbBoxEventCode)evt.EventCode)
        {
            case UsbBoxEventCode.UsbConnect:
                lineInfo.DeviceConnected = true;
                evt.Status = "Connected";
                break;
            case UsbBoxEventCode.UsbDisconnect:
                lineInfo.DeviceConnected = false;
                evt.Status = "Disconnected";
                break;
            case UsbBoxEventCode.State:
                var state = (ChannelState)evt.Param;
                lineInfo.Status = MapChannelStateToLineStatus(state);
                evt.Status = lineInfo.Status.ToString();
                break;
            case UsbBoxEventCode.CallerId:
                var callerId = GetCallerId(line);
                if (!string.IsNullOrEmpty(callerId))
                {
                    lineInfo.LastCallerId = callerId;
                    evt.CallerId = callerId;
                }
                break;
            case UsbBoxEventCode.Dtmf:
                var dtmf = GetDtmfNumber(line);
                if (!string.IsNullOrEmpty(dtmf))
                {
                    lineInfo.LastDtmf = dtmf;
                    evt.Dtmf = dtmf;
                }
                break;
            case UsbBoxEventCode.RingCount:
                lineInfo.RingCount = (int)evt.Param;
                break;
            case UsbBoxEventCode.Voltage:
                lineInfo.Voltage = (int)evt.Param;
                break;
        }
    }

    private LineStatus MapChannelStateToLineStatus(ChannelState state)
    {
        return state switch
        {
            ChannelState.HookOn => LineStatus.Free,
            ChannelState.HookOff => LineStatus.Offhook,
            ChannelState.RingOn => LineStatus.Ringing,
            ChannelState.InRecord or ChannelState.OutRecord => LineStatus.InCall,
            _ => LineStatus.Unknown
        };
    }

    private void ParseEvent(DeviceEvent evt)
    {
        evt.Meaning = ((UsbBoxEventCode)evt.EventCode) switch
        {
            UsbBoxEventCode.State => $"State changed to {(ChannelState)evt.Param}",
            UsbBoxEventCode.Voltage => $"Voltage: {evt.Param}",
            UsbBoxEventCode.CallerId => "Caller ID received",
            UsbBoxEventCode.Dtmf => "DTMF received",
            UsbBoxEventCode.RingCount => $"Ring count: {evt.Param}",
            UsbBoxEventCode.UsbConnect => "USB device connected",
            UsbBoxEventCode.UsbDisconnect => "USB device disconnected",
            UsbBoxEventCode.BusyTone => "Busy tone detected",
            UsbBoxEventCode.MissedInboundCall => "Missed inbound call",
            _ => $"Event code {evt.EventCode}"
        };
    }

    public bool StartUsbBox(UsbBoxDeviceType deviceType)
    {
        try
        {
            // Register callbacks if not already registered
            RegisterUsbBoxCallbacks();

            _currentDeviceType = deviceType;
            int result = UsbBoxInterop.UsbBox_SetDeviceType((int)deviceType);
            if (result != 0) return false;

            result = UsbBoxInterop.UsbBox_EnableDevice();
            _usbBoxEnabled = result == 0;
            return _usbBoxEnabled;
        }
        catch (DllNotFoundException ex)
        {
            System.Diagnostics.Debug.WriteLine($"DLL not found: {ex.Message}");
            return false;
        }
        catch (System.Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error starting USB box: {ex.Message}");
            return false;
        }
    }

    public bool StopUsbBox()
    {
        try
        {
            int result = UsbBoxInterop.UsbBox_DisableDevice();
            _usbBoxEnabled = false;
            return result == 0;
        }
        catch
        {
            return false;
        }
    }

    public bool Pickup(int line)
    {
        try
        {
            return UsbBoxInterop.UsbBox_Pickup(line) == 0;
        }
        catch
        {
            return false;
        }
    }

    public bool HookOn(int line)
    {
        try
        {
            return UsbBoxInterop.UsbBox_HookOn(line) == 0;
        }
        catch
        {
            return false;
        }
    }

    public bool Dial(int line, string number)
    {
        try
        {
            return UsbBoxInterop.UsbBox_Diall(line, number) == 0;
        }
        catch
        {
            return false;
        }
    }

    public string? GetCallerId(int line)
    {
        try
        {
            var sb = new StringBuilder(256);
            int result = UsbBoxInterop.UsbBox_GetCallerNumber(line, sb);
            if (result == 0 && sb.Length > 0)
            {
                return sb.ToString();
            }
        }
        catch
        {
        }
        return null;
    }

    public string? GetDtmfNumber(int line)
    {
        try
        {
            var sb = new StringBuilder(256);
            int result = UsbBoxInterop.UsbBox_GetDtmfNumber(line, sb);
            if (result == 0 && sb.Length > 0)
            {
                return sb.ToString();
            }
        }
        catch
        {
        }
        return null;
    }

    public int GetState(int line)
    {
        try
        {
            return UsbBoxInterop.UsbBox_GetState(line);
        }
        catch
        {
            return -1;
        }
    }

    public int GetDeviceCount()
    {
        try
        {
            return UsbBoxInterop.UsbBox_GetDeviceCount();
        }
        catch
        {
            return 0;
        }
    }

    public int GetChannelCount()
    {
        try
        {
            return UsbBoxInterop.UsbBox_GetChannelCount();
        }
        catch
        {
            return 0;
        }
    }

    public LineInfo? GetLineInfo(int line)
    {
        _lines.TryGetValue(line, out var info);
        return info;
    }

    public IEnumerable<LineInfo> GetAllLines()
    {
        return _lines.Values.OrderBy(l => l.LineIndex);
    }

    public bool StartAD101(IntPtr hWnd)
    {
        try
        {
            bool result = AD101Interop.AD101_InitDevice(hWnd);
            if (result)
            {
                AD101Interop.AD101EventCallbackProc callback = (line, evt, param) =>
                {
                    var deviceEvent = new DeviceEvent
                    {
                        ApiFamily = ApiFamily.AD101,
                        EventCode = evt,
                        Line = line,
                        Param = (uint)param,
                        Timestamp = DateTime.Now
                    };
                    OnDeviceEvent?.Invoke(this, deviceEvent);
                };
                AD101Interop.EventCallbackInstance = callback;
                AD101Interop.AD101_SetEventCallbackFun(callback);
                _ad101Enabled = true;
            }
            return result;
        }
        catch
        {
            return false;
        }
    }

    public void StopAD101()
    {
        try
        {
            AD101Interop.AD101_FreeDevice();
            _ad101Enabled = false;
        }
        catch
        {
        }
    }

    public bool StartAD800(IntPtr hWnd)
    {
        try
        {
            bool result = AD800Interop.AD800_Init();
            if (result)
            {
                if (hWnd != IntPtr.Zero)
                {
                    AD800Interop.AD800_SetMsgHwnd(hWnd);
                }

                AD800Interop.AD800EventCallbackProc callback = (channel, evt, param) =>
                {
                    var deviceEvent = new DeviceEvent
                    {
                        ApiFamily = ApiFamily.AD800,
                        EventCode = evt,
                        Line = channel,
                        Param = (uint)param,
                        Timestamp = DateTime.Now
                    };
                    OnDeviceEvent?.Invoke(this, deviceEvent);
                };
                AD800Interop.EventCallbackInstance = callback;
                AD800Interop.AD800_SetCallbackFun(callback);
                _ad800Enabled = true;
            }
            return result;
        }
        catch
        {
            return false;
        }
    }

    public void StopAD800()
    {
        try
        {
            AD800Interop.AD800_Free();
            _ad800Enabled = false;
        }
        catch
        {
        }
    }

    public void SetMessageWindow(IntPtr hWnd)
    {
        try
        {
            UsbBoxInterop.UsbBox_SetMsg(hWnd);
        }
        catch
        {
        }
    }

    public bool IsUsbBoxEnabled => _usbBoxEnabled;
    public bool IsAD101Enabled => _ad101Enabled;
    public bool IsAD800Enabled => _ad800Enabled;
}

