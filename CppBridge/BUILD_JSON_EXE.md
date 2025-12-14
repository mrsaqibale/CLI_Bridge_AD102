# Build JSON Version EXE (CppBridge.exe)

## Configuration

The project is now set up so:
- **Debug builds:** `CppBridge.exe` (JSON version - for testing)
- **Release builds:** `CppBridge.exe` (JSON version - for deployment)

The menu version (`CppBridgeMenu.exe`) is only built in Debug for testing.

## Build Steps

1. **Open Visual Studio**
2. **Set Configuration to:** `Release`
3. **Set Platform to:** `x64`
4. **Build → Build Solution** (or `Ctrl+Shift+B`)

## Output

The EXE will be at:
```
CppBridge\bin\x64\Release\CppBridge.exe
```

## Copy DLLs

After building, copy DLLs:
```powershell
Copy-Item EN\64BIT\*.dll CppBridge\bin\x64\Release\ -Force
```

## Test

```powershell
cd CppBridge\bin\x64\Release
.\CppBridge.exe
```

You should see:
- Auto-enables device (F2)
- Sends JSON to stdout when calls come/end
- Runs continuously listening for events

## JSON Output

**Call comes:**
```json
{"type":"event","status":"Ring","line":0,"callerId":"21302"}
```

**Call ends:**
```json
{"type":"event","status":"Free","line":0}
```

## Deploy

Copy these files to target system:
- `CppBridge.exe`
- `USBBoxProtocol.dll`
- `AD101Device.dll`
- `AD800Device.dll`
- `lame_enc.dll`

Plus install:
- Visual C++ 2005 Redistributable (x64)
- Npcap (for wpcap.dll)

