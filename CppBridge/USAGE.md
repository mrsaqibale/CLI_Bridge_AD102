# C++ Bridge Usage Guide

## ✅ App is Running!

Your C++ bridge is now working and waiting for JSON commands.

## How to Use

### Option 1: Command Line (Recommended for Testing)

1. **Stop the Visual Studio debugger** (if running)
2. **Open PowerShell** in the `CppBridge` folder
3. **Run the bridge:**
   ```powershell
   cd bin\x64\Debug
   .\CppBridge.exe
   ```

4. **In another terminal, send commands:**
   ```powershell
   echo '{"cmd":"enable","deviceType":"F2"}' | .\CppBridge.exe
   ```

### Option 2: Pipe Commands

```powershell
# Single command
echo '{"cmd":"enable","deviceType":"F2"}' | .\CppBridge.exe

# Multiple commands (use test script)
.\test-bridge.ps1
```

### Option 3: Use with Electron

See `electron-example.js` for full integration example.

## Available Commands

### Enable Device
```json
{"cmd":"enable","deviceType":"F2"}
```
Device types: `"F1"`, `"F2"`, `"F4"`, `"F8"`

### Disable Device
```json
{"cmd":"disable"}
```

### Pickup Line
```json
{"cmd":"pickup","line":0}
```

### Hang Up (Hook On)
```json
{"cmd":"hookon","line":0}
```

### Dial Number
```json
{"cmd":"dial","line":0,"number":"1234567890"}
```

## Expected Output

### Events (from callbacks):
```json
{"type":"event","eventCode":22,"line":0,"status":"Connected"}
{"type":"event","eventCode":0,"line":0,"state":3,"status":"Ringing","callerId":"1234567890"}
{"type":"event","eventCode":2,"line":0,"status":"CallerID","callerId":"1234567890"}
```

### Results (from commands):
```json
{"type":"result","ok":true,"cmd":"enable"}
{"type":"result","ok":false,"cmd":"enable","error":-1}
```

## Event Codes

- `0` = EVENT_STATE (line state changed)
- `2` = EVENT_CALLERID (caller ID received)
- `3` = EVENT_DTMF (DTMF digits)
- `22` = EVENT_USBCONNECT (device connected)
- `23` = EVENT_USBDISCONNECT (device disconnected)

## State Values

- `0` = CH_STATE_OFFLINE
- `1` = CH_STATE_HOOKON (Free)
- `2` = CH_STATE_HOOKOFF (Offhook)
- `3` = CH_STATE_RINGON (Ringing)
- `4` = CH_STATE_INRECORD (Inbound call)
- `5` = CH_STATE_OUTRECORD (Outbound call)

## Next Steps

1. **Test basic commands** using the test script
2. **Connect your USB device** and test with real hardware
3. **Integrate with Electron** using the example code
4. **Build Release version** for deployment

## Troubleshooting

- **No events?** Make sure device is connected and enabled
- **Error -1?** Device not found or wrong device type
- **No caller ID?** Wait for ring event, caller ID comes after ringing starts

