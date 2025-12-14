# Install Required Runtime Libraries

## Problem
`USBBoxProtocol.dll` requires **Visual C++ 2005** runtime libraries that are not installed.

## Required Files
- `MSVCP80.dll` (VC++ 2005 C++ runtime)
- `MSVCR80.dll` (VC++ 2005 C runtime)
- `wpcap.dll` (WinPcap/Npcap - for network features, may be optional)

## Solution

### Step 1: Install Visual C++ 2005 Redistributable (x64)

**Download:**
- Direct link: https://www.microsoft.com/en-us/download/details.aspx?id=18471
- Or search: "Visual C++ 2005 Redistributable x64"

**Install:**
1. Download `vcredist_x64.exe`
2. Run the installer
3. Restart your application

### Step 2: Install Npcap (if wpcap.dll is missing)

**Download:**
- https://npcap.com/dist/
- Download the latest Npcap installer

**Install:**
1. Run the installer
2. Choose "Install Npcap in WinPcap API-compatible Mode" (if asked)
3. Restart your application

**Note:** Npcap is only needed if the DLL uses network features. If you don't need network dialing, you might be able to skip this.

## Verify Installation

After installing, check if files exist:
```powershell
Test-Path "C:\Windows\System32\MSVCP80.dll"
Test-Path "C:\Windows\System32\MSVCR80.dll"
```

Both should return `True`.

## Alternative: Copy DLLs Locally

If you can't install system-wide, you can copy the DLLs to your app folder:
1. Copy `MSVCP80.dll` and `MSVCR80.dll` from a system that has them
2. Place them in the same folder as `CppBridge.exe`
3. This is not recommended but may work for testing

## Quick Test

After installing, try running your app again. The error should be gone.

