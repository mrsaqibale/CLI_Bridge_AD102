# Deployment Guide

## Step 1: Test UI Locally (Current Machine)

First, test the UI application on your current machine:

```powershell
cd UsbBridgeUi
dotnet run
```

This will:
- Open a Windows Forms window
- Let you test the interface
- Show that the app works (even without device connected)

**Note:** The "Enable Device" button may fail if no device is connected - this is expected.

## Step 2: Build for Deployment

After testing the UI, build the executables for the target system:

```powershell
# Run the build script
.\build-for-deployment.ps1
```

This creates a `deployment` folder with:
- `UsbBridgeConsole\` - Console app (for Electron)
- `UsbBridgeUi\` - UI app (for testing)

## Step 3: Deploy to Target System

1. Copy the entire `deployment` folder to your target system (the one with USB device)
2. On the target system, navigate to the folder
3. Run the UI first to test:
   ```
   .\UsbBridgeUi\UsbBridgeUi.exe
   ```
4. Then use the console app for Electron integration:
   ```
   .\UsbBridgeConsole\UsbBridgeConsole.exe
   ```

## Quick Commands

### Test UI Locally
```powershell
cd UsbBridgeUi
dotnet run
```

### Build for Deployment
```powershell
.\build-for-deployment.ps1
```

### Test Console Locally (Optional)
```powershell
cd UsbBridgeConsole
dotnet run
```

## What's Included in Deployment

Each folder contains:
- `.exe` file (standalone, self-contained)
- `USBBoxProtocol.dll`
- `AD101Device.dll`
- `AD800Device.dll`
- `lame_enc.dll` (optional)

All DLLs are automatically copied during build.

