# Install wpcap.dll (Npcap/WinPcap)

## Problem
`USBBoxProtocol.dll` requires `wpcap.dll` which is missing.

## Solution: Install Npcap

**Npcap** is the modern replacement for WinPcap and provides `wpcap.dll`.

### Step 1: Download Npcap
- **Website:** https://npcap.com/dist/
- **Download:** Latest Npcap installer (e.g., `npcap-1.xx.exe`)

### Step 2: Install Npcap
1. Run the installer
2. **Important:** During installation, check:
   - ✅ "Install Npcap in WinPcap API-compatible Mode"
   - This ensures `wpcap.dll` is available for compatibility
3. Complete the installation
4. **Restart your computer** (recommended) or at least restart Visual Studio

### Step 3: Verify Installation
After installing, check:
```powershell
Test-Path "C:\Windows\System32\wpcap.dll"
```
Should return `True`.

### Step 4: Test Your App
Run `CppBridge.exe` again - the error should be gone.

## Alternative: WinPcap (Older)
If Npcap doesn't work, try WinPcap:
- **Download:** https://www.winpcap.org/install/default.htm
- **Note:** WinPcap is no longer maintained, Npcap is recommended

## Why is wpcap.dll needed?
The USBBox DLL has network dialing features (`EnableNetDial_TCP`, `EnableNetDial_WS`) that use packet capture libraries. Even if you don't use network features, the DLL is statically linked to wpcap.dll, so it's required.

## After Installation
Once Npcap is installed, your app should run successfully!

