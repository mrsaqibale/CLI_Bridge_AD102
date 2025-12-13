using System.Text;
using System.Text.Json;
using UsbDeviceCore;

namespace UsbBridgeConsole;

class Program
{
    private static DeviceService? _deviceService;
    private static UsbBoxDeviceType _defaultDeviceType = UsbBoxDeviceType.F2;
    private static bool _running = true;

    static void Main(string[] args)
    {
        // Parse command line args for device type
        if (args.Length > 0)
        {
            if (Enum.TryParse<UsbBoxDeviceType>(args[0], true, out var deviceType))
            {
                _defaultDeviceType = deviceType;
            }
        }

        // Check environment variable
        var envDeviceType = Environment.GetEnvironmentVariable("USB_DEVICE_TYPE");
        if (!string.IsNullOrEmpty(envDeviceType))
        {
            if (Enum.TryParse<UsbBoxDeviceType>(envDeviceType, true, out var envType))
            {
                _defaultDeviceType = envType;
            }
        }

        _deviceService = new DeviceService();
        _deviceService.OnDeviceEvent += DeviceService_OnDeviceEvent;

        // Write startup message to stderr (so it doesn't interfere with JSON output)
        Console.Error.WriteLine("USB Bridge Console started. Waiting for JSON commands on stdin...");
        Console.Error.WriteLine("Send commands like: {\"cmd\":\"usbbox.enable\",\"deviceType\":\"F2\"}");
        Console.Error.WriteLine("Press Ctrl+C to exit.");
        Console.Error.Flush();

        // Auto-start if configured
        var autoStart = Environment.GetEnvironmentVariable("USB_AUTO_START");
        if (autoStart == "1" || autoStart?.ToLower() == "true")
        {
            _deviceService.StartUsbBox(_defaultDeviceType);
        }

        // Start command reader thread
        var readerThread = new Thread(ReadCommands)
        {
            IsBackground = true
        };
        readerThread.Start();

        // Keep main thread alive
        while (_running)
        {
            Thread.Sleep(100);
        }

        _deviceService.StopUsbBox();
        _deviceService.StopAD101();
        _deviceService.StopAD800();
    }

    private static void DeviceService_OnDeviceEvent(object? sender, DeviceEvent e)
    {
        try
        {
            var jsonEvent = new
            {
                type = "event",
                api = e.ApiFamily.ToString().ToLower(),
                eventCode = e.EventCode,
                line = e.Line,
                param = e.Param,
                deviceId = e.DeviceId,
                status = e.Status,
                callerId = e.CallerId,
                dtmf = e.Dtmf,
                meaning = e.Meaning,
                ts = e.Timestamp.ToString("yyyy-MM-ddTHH:mm:ss.fffZ")
            };

            var json = JsonSerializer.Serialize(jsonEvent);
            Console.WriteLine(json);
            Console.Out.Flush();
        }
        catch (Exception ex)
        {
            WriteError($"Error serializing event: {ex.Message}");
        }
    }

    private static void ReadCommands()
    {
        string? line;
        while (_running)
        {
            try
            {
                line = Console.ReadLine();
                if (line == null)
                {
                    // stdin closed, exit
                    _running = false;
                    break;
                }

                if (string.IsNullOrWhiteSpace(line))
                    continue;

                var command = JsonSerializer.Deserialize<JsonElement>(line);
                if (!command.TryGetProperty("cmd", out var cmdProp))
                {
                    WriteError("Missing 'cmd' property");
                    continue;
                }

                var cmd = cmdProp.GetString();
                if (string.IsNullOrEmpty(cmd))
                {
                    WriteError("Empty command");
                    continue;
                }

                ProcessCommand(cmd, command);
            }
            catch (JsonException ex)
            {
                WriteError($"Invalid JSON: {ex.Message}");
            }
            catch (Exception ex)
            {
                WriteError($"Error processing command: {ex.Message}");
            }
        }
    }

    private static void ProcessCommand(string cmd, JsonElement command)
    {
        if (_deviceService == null)
        {
            WriteError("Device service not initialized");
            return;
        }

        bool success = false;
        string? error = null;

        try
        {
            switch (cmd.ToLower())
            {
                case "usbbox.enable":
                    var deviceTypeStr = command.TryGetProperty("deviceType", out var dtProp) 
                        ? dtProp.GetString() 
                        : _defaultDeviceType.ToString();
                    if (Enum.TryParse<UsbBoxDeviceType>(deviceTypeStr, true, out var deviceType))
                    {
                        success = _deviceService.StartUsbBox(deviceType);
                        if (!success && string.IsNullOrEmpty(error))
                        {
                            error = _deviceService.LastError ?? "Failed to enable device";
                        }
                    }
                    else
                    {
                        error = "Invalid device type";
                    }
                    break;

                case "usbbox.disable":
                    success = _deviceService.StopUsbBox();
                    break;

                case "usbbox.pickup":
                    if (command.TryGetProperty("line", out var pickupLine))
                    {
                        success = _deviceService.Pickup(pickupLine.GetInt32());
                    }
                    else
                    {
                        error = "Missing 'line' property";
                    }
                    break;

                case "usbbox.hookon":
                    if (command.TryGetProperty("line", out var hookLine))
                    {
                        success = _deviceService.HookOn(hookLine.GetInt32());
                    }
                    else
                    {
                        error = "Missing 'line' property";
                    }
                    break;

                case "usbbox.dial":
                    if (command.TryGetProperty("line", out var dialLine) &&
                        command.TryGetProperty("number", out var numberProp))
                    {
                        var number = numberProp.GetString();
                        if (!string.IsNullOrEmpty(number))
                        {
                            success = _deviceService.Dial(dialLine.GetInt32(), number);
                        }
                        else
                        {
                            error = "Empty number";
                        }
                    }
                    else
                    {
                        error = "Missing 'line' or 'number' property";
                    }
                    break;

                case "usbbox.getcallerid":
                    if (command.TryGetProperty("line", out var callerLine))
                    {
                        var callerId = _deviceService.GetCallerId(callerLine.GetInt32());
                        var result = new
                        {
                            type = "result",
                            ok = true,
                            cmd = cmd,
                            callerId = callerId
                        };
                        Console.WriteLine(JsonSerializer.Serialize(result));
                        Console.Out.Flush();
                        return;
                    }
                    else
                    {
                        error = "Missing 'line' property";
                    }
                    break;

                case "usbbox.getstate":
                    if (command.TryGetProperty("line", out var stateLine))
                    {
                        var state = _deviceService.GetState(stateLine.GetInt32());
                        var result = new
                        {
                            type = "result",
                            ok = true,
                            cmd = cmd,
                            state = state
                        };
                        Console.WriteLine(JsonSerializer.Serialize(result));
                        Console.Out.Flush();
                        return;
                    }
                    else
                    {
                        error = "Missing 'line' property";
                    }
                    break;

                case "usbbox.getlineinfo":
                    if (command.TryGetProperty("line", out var infoLine))
                    {
                        var lineInfo = _deviceService.GetLineInfo(infoLine.GetInt32());
                        var result = new
                        {
                            type = "result",
                            ok = true,
                            cmd = cmd,
                            lineInfo = lineInfo != null ? new
                            {
                                lineIndex = lineInfo.LineIndex,
                                status = lineInfo.Status.ToString(),
                                lastCallerId = lineInfo.LastCallerId,
                                lastDtmf = lineInfo.LastDtmf,
                                deviceConnected = lineInfo.DeviceConnected,
                                ringCount = lineInfo.RingCount,
                                voltage = lineInfo.Voltage
                            } : null
                        };
                        Console.WriteLine(JsonSerializer.Serialize(result));
                        Console.Out.Flush();
                        return;
                    }
                    else
                    {
                        error = "Missing 'line' property";
                    }
                    break;

                case "ad101.start":
                    success = _deviceService.StartAD101(IntPtr.Zero);
                    break;

                case "ad101.stop":
                    _deviceService.StopAD101();
                    success = true;
                    break;

                case "ad800.start":
                    success = _deviceService.StartAD800(IntPtr.Zero);
                    break;

                case "ad800.stop":
                    _deviceService.StopAD800();
                    success = true;
                    break;

                default:
                    error = $"Unknown command: {cmd}";
                    break;
            }
        }
        catch (Exception ex)
        {
            error = ex.Message;
            success = false;
        }

        var response = new
        {
            type = "result",
            ok = success && error == null,
            cmd = cmd,
            error = error
        };

        Console.WriteLine(JsonSerializer.Serialize(response));
        Console.Out.Flush();
    }

    private static void WriteError(string message)
    {
        var error = new
        {
            type = "error",
            message = message,
            ts = DateTime.UtcNow.ToString("yyyy-MM-ddTHH:mm:ss.fffZ")
        };
        var json = JsonSerializer.Serialize(error);
        Console.Error.WriteLine(json);
        Console.Error.Flush();
    }
}

