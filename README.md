# USB Device Bridge

A .NET 8 bridge application for integrating USB telephony device DLLs (USBBoxProtocol.dll, AD101Device.dll, AD800Device.dll) with Electron applications.

## Solution Structure

The solution contains three projects:

1. **UsbDeviceCore** - Core library with P/Invoke declarations and business logic
2. **UsbBridgeUi** - WinForms test application with UI for device testing
3. **UsbBridgeConsole** - Headless console bridge for Electron integration (JSON I/O)

## Prerequisites

- .NET 8 SDK or later
- Windows OS (required for WinAPI interop)
- Vendor DLLs:
  - `USBBoxProtocol.dll`
  - `AD101Device.dll`
  - `lame_enc.dll` (optional, for MP3 encoding)

## Building

### Build All Projects

```powershell
dotnet build UsbDeviceBridge.sln
```

### Build for Specific Platform

The DLLs must match the target platform (x64 or x86). Build accordingly:

```powershell
# x64
dotnet build UsbDeviceBridge.sln -c Release /p:Platform=x64

# x86
dotnet build UsbDeviceBridge.sln -c Release /p:Platform=x86
```

## DLL Placement

The vendor DLLs must be placed next to the built executables. The project files are configured to copy DLLs from the solution root to the output directories.

**Important**: Ensure the DLL bitness (x64/x86) matches your build target.

## Running the UI Application

```powershell
cd UsbBridgeUi
dotnet run
```

Or run the built executable:
```powershell
.\UsbBridgeUi\bin\Release\net8.0-windows\UsbBridgeUi.exe
```

### UI Features

- Device type selection (F1/F2/F4/F8/EAR)
- Enable/Disable device buttons
- Line control: Pickup, Hook On, Dial
- Real-time line status display
- Event log viewer
- Connection status indicator

## Running the Console Bridge

The console bridge reads JSON commands from stdin and writes JSON events to stdout.

### Basic Usage

```powershell
cd UsbBridgeConsole
dotnet run
```

### Command Line Arguments

```powershell
# Specify device type
dotnet run -- F2

# Or use environment variables
$env:USB_DEVICE_TYPE="F2"
$env:USB_AUTO_START="1"
dotnet run
```

### JSON Command Format

Send commands via stdin (newline-delimited JSON):

```json
{"cmd":"usbbox.enable","deviceType":"F2"}
{"cmd":"usbbox.disable"}
{"cmd":"usbbox.pickup","line":0}
{"cmd":"usbbox.hookon","line":0}
{"cmd":"usbbox.dial","line":0,"number":"03001234567"}
{"cmd":"usbbox.getcallerid","line":0}
{"cmd":"usbbox.getstate","line":0}
{"cmd":"usbbox.getlineinfo","line":0}
{"cmd":"ad101.start"}
{"cmd":"ad101.stop"}
{"cmd":"ad800.stop"}
```

### JSON Response Format

Results are written to stdout:

```json
{"type":"result","ok":true,"cmd":"usbbox.dial"}
{"type":"result","ok":false,"cmd":"usbbox.pickup","error":"Missing 'line' property"}
```

Events are written to stdout:

```json
{"type":"event","api":"usbbox","eventCode":22,"line":0,"status":"Connected","callerId":null,"dtmf":null,"ts":"2024-01-01T12:00:00.000Z"}
{"type":"event","api":"usbbox","eventCode":0,"line":0,"param":2,"status":"Offhook","ts":"2024-01-01T12:00:01.000Z"}
{"type":"event","api":"usbbox","eventCode":2,"line":0,"callerId":"1234567890","ts":"2024-01-01T12:00:02.000Z"}
```

Errors are written to stderr:

```json
{"type":"error","message":"Invalid JSON","ts":"2024-01-01T12:00:00.000Z"}
```

## Publishing for Electron

### Publish UI Application

```powershell
.\scripts\publish-ui-x64.ps1
```

This creates a self-contained deployment in `publish/UsbBridgeUi-win-x64/`.

### Publish Console Bridge

```powershell
.\scripts\publish-console-x64.ps1
```

This creates a self-contained deployment in `publish/UsbBridgeConsole-win-x64/`.

### Manual Publishing

```powershell
# Console bridge (x64)
dotnet publish UsbBridgeConsole/UsbBridgeConsole.csproj -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true

# UI application (x64)
dotnet publish UsbBridgeUi/UsbBridgeUi.csproj -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true
```

## Electron Integration

### Spawning the Bridge Process

```javascript
const { spawn } = require('child_process');
const path = require('path');

const bridgePath = path.join(__dirname, 'UsbBridgeConsole-win-x64', 'UsbBridgeConsole.exe');
const bridge = spawn(bridgePath, [], {
  stdio: ['pipe', 'pipe', 'pipe']
});

// Send commands
bridge.stdin.write(JSON.stringify({
  cmd: 'usbbox.enable',
  deviceType: 'F2'
}) + '\n');

// Receive events
bridge.stdout.on('data', (data) => {
  const lines = data.toString().split('\n').filter(l => l.trim());
  lines.forEach(line => {
    try {
      const event = JSON.parse(line);
      if (event.type === 'event') {
        console.log('Device event:', event);
      } else if (event.type === 'result') {
        console.log('Command result:', event);
      }
    } catch (e) {
      console.error('Parse error:', e);
    }
  });
});

// Handle errors
bridge.stderr.on('data', (data) => {
  const error = JSON.parse(data.toString());
  console.error('Bridge error:', error);
});
```

## API Reference

### DeviceService Methods

#### USBBox API

- `StartUsbBox(UsbBoxDeviceType deviceType)` - Initialize and enable USB device
- `StopUsbBox()` - Disable USB device
- `Pickup(int line)` - Pick up the phone on specified line
- `HookOn(int line)` - Hang up the phone on specified line
- `Dial(int line, string number)` - Dial a number on specified line
- `GetCallerId(int line)` - Get caller ID for line
- `GetDtmfNumber(int line)` - Get DTMF digits for line
- `GetState(int line)` - Get current state of line
- `GetDeviceCount()` - Get number of connected devices
- `GetChannelCount()` - Get total number of channels
- `GetLineInfo(int line)` - Get full line information
- `SetMessageWindow(IntPtr hWnd)` - Set window handle for message mode

#### AD101 API

- `StartAD101(IntPtr hWnd)` - Initialize AD101 device
- `StopAD101()` - Free AD101 device

#### AD800 API

- `StartAD800(IntPtr hWnd)` - Initialize AD800 device
- `StopAD800()` - Free AD800 device

### Events

The `DeviceService.OnDeviceEvent` event fires for all device events:

```csharp
deviceService.OnDeviceEvent += (sender, e) => {
    Console.WriteLine($"Event: {e.ApiFamily} {e.EventCode} Line {e.Line}");
    Console.WriteLine($"Caller ID: {e.CallerId}");
    Console.WriteLine($"DTMF: {e.Dtmf}");
};
```

## Event Codes

### USBBox Events

- `0` - State change
- `1` - Voltage change
- `2` - Caller ID received
- `3` - DTMF received
- `4` - Ring count change
- `5` - Record time
- `22` - USB device connected
- `23` - USB device disconnected
- `10` - Busy tone detected
- `11` - Missed inbound call

## Troubleshooting

### DLL Not Found

Ensure the vendor DLLs are in the same directory as the executable. Check the build output folder.

### Wrong Bitness

If you get "BadImageFormatException", the DLL bitness doesn't match your build target. Rebuild for the correct platform (x64 or x86).

### Callbacks Not Firing

Callbacks are kept alive via static fields, but ensure the DeviceService instance is not garbage collected. Keep a reference to it.

### No Events Received

1. Verify device is connected
2. Check that `StartUsbBox()` returned true
3. Ensure callbacks are registered (done automatically in constructor)
4. Check Windows event viewer for DLL errors

## License

This bridge application is provided as-is for integration with vendor telephony device DLLs.
