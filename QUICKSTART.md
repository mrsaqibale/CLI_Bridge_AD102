# Quick Start Guide - How to Run

## Option 1: Using Visual Studio (Recommended)

1. **Open the Solution**
   - Double-click `UsbDeviceBridge.sln` to open in Visual Studio
   - Or right-click → "Open with Visual Studio"

2. **Build the Solution**
   - Press `Ctrl+Shift+B` or go to Build → Build Solution
   - Wait for build to complete

3. **Run the UI Application**
   - Right-click `UsbBridgeUi` project → "Set as Startup Project"
   - Press `F5` or click the green "Start" button
   - The UI window will open

4. **Run the Console Bridge**
   - Right-click `UsbBridgeConsole` project → "Set as Startup Project"
   - Press `F5` or click the green "Start" button
   - Or run from command line (see Option 2)

## Option 2: Using Command Line (dotnet CLI)

### Prerequisites
- Install .NET 8 SDK from: https://dotnet.microsoft.com/download/dotnet/8.0
- After installation, restart your terminal/PowerShell

### Build the Solution

```powershell
# Navigate to the solution directory
cd C:\Users\DOCTOR\Documents\gpos\bridge

# Build all projects
dotnet build UsbDeviceBridge.sln
```

### Run the UI Application

```powershell
# Option A: Run directly (development mode)
cd UsbBridgeUi
dotnet run

# Option B: Run the built executable
cd UsbBridgeUi\bin\Debug\net8.0-windows
.\UsbBridgeUi.exe
```

### Run the Console Bridge

```powershell
# Option A: Run directly (development mode)
cd UsbBridgeConsole
dotnet run

# Option B: Run the built executable
cd UsbBridgeConsole\bin\Debug\net8.0
.\UsbBridgeConsole.exe
```

### Test Console Bridge with Commands

```powershell
# Start the console bridge
cd UsbBridgeConsole
dotnet run

# In another terminal, or pipe commands:
echo '{"cmd":"usbbox.enable","deviceType":"F2"}' | dotnet run --project UsbBridgeConsole
```

## Option 3: Using PowerShell Scripts (After Building)

After building, you can use the publish scripts:

```powershell
# Navigate to scripts folder
cd scripts

# Publish UI (creates standalone executable)
.\publish-ui-x64.ps1

# Then run from publish folder
cd ..\publish\UsbBridgeUi-win-x64
.\UsbBridgeUi.exe
```

## Troubleshooting

### "dotnet command not found"
- Install .NET 8 SDK: https://dotnet.microsoft.com/download/dotnet/8.0
- Restart your terminal after installation
- Verify: `dotnet --version` should show 8.x.x

### "DLL not found" errors
- Ensure these DLLs are in the same folder as the .exe:
  - `USBBoxProtocol.dll`
  - `AD101Device.dll`
  - `AD800Device.dll`
  - `lame_enc.dll` (optional)

- The project files are configured to copy DLLs automatically, but if they're missing:
  - Copy DLLs from the root folder to: `UsbBridgeUi\bin\Debug\net8.0-windows\`
  - Or: `UsbBridgeConsole\bin\Debug\net8.0\`

### "BadImageFormatException"
- This means DLL bitness (x64/x86) doesn't match your build
- Try building for x64:
  ```powershell
  dotnet build UsbDeviceBridge.sln -c Release /p:Platform=x64
  ```

### Build Errors
- Make sure you're using .NET 8 SDK
- Check that all project files are in the correct folders
- Try cleaning and rebuilding:
  ```powershell
  dotnet clean UsbDeviceBridge.sln
  dotnet build UsbDeviceBridge.sln
  ```

## First Run Checklist

1. ✅ DLLs are in the root directory (already done)
2. ✅ .NET 8 SDK is installed
3. ✅ Solution builds successfully
4. ✅ Run UI app to test device connection
5. ✅ Run console bridge to test JSON commands

## UI Application Usage

1. Select device type (F1/F2/F4/F8) from dropdown
2. Click "Enable Device" to start
3. Watch the status panel for line information
4. Use "Pickup", "Hook On", "Dial" buttons to control lines
5. View events in the log listbox

## Console Bridge Usage

The console bridge reads JSON from stdin and writes JSON to stdout:

**Input (stdin):**
```json
{"cmd":"usbbox.enable","deviceType":"F2"}
{"cmd":"usbbox.pickup","line":0}
{"cmd":"usbbox.dial","line":0,"number":"1234567890"}
```

**Output (stdout):**
```json
{"type":"event","api":"usbbox","eventCode":22,"line":0,"status":"Connected",...}
{"type":"result","ok":true,"cmd":"usbbox.pickup"}
```

## Need Help?

- Check the main README.md for detailed documentation
- Verify DLLs are 64-bit if building for x64
- Check Windows Event Viewer for DLL loading errors

