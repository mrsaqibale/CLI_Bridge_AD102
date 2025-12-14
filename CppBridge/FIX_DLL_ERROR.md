# Fix: Error 0xc0000135 (Dependent DLL Not Found)

## Problem
The error `0xc0000135` means a dependency of `USBBoxProtocol.dll` is missing.

## Solution 1: Install Visual C++ Redistributables

The DLL was compiled with an older Visual C++ runtime. Install these:

### For 64-bit (x64):
1. **Visual C++ 2005 Redistributable (x64)**
   - Download: https://www.microsoft.com/en-us/download/details.aspx?id=18471
   - Install: `vcredist_x64.exe`

2. **Visual C++ 2008 Redistributable (x64)**
   - Download: https://www.microsoft.com/en-us/download/details.aspx?id=15336
   - Install: `vcredist_x64.exe`

3. **Visual C++ 2010 Redistributable (x64)**
   - Download: https://www.microsoft.com/en-us/download/details.aspx?id=14632
   - Install: `vcredist_x64.exe`

4. **Visual C++ 2015-2022 Redistributable (x64)** (Latest)
   - Download: https://aka.ms/vs/17/release/vc_redist.x64.exe
   - Install this one first - it includes 2015-2022 runtimes

### Quick Install (All-in-One):
Download and install the **latest VC++ Redistributable** which includes all versions:
- **Visual C++ 2015-2022 Redistributable (x64)**: https://aka.ms/vs/17/release/vc_redist.x64.exe

## Solution 2: Check DLL Dependencies

Use Dependency Walker or dumpbin to see what's missing:

```powershell
# If Visual Studio is installed:
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe" /dependents EN\64BIT\USBBoxProtocol.dll
```

## Solution 3: Copy All DLLs Together

Make sure ALL DLLs are in the same folder:
- `USBBoxProtocol.dll`
- `AD101Device.dll`
- `AD800Device.dll`
- `lame_enc.dll`

They might depend on each other.

## Solution 4: Test with Working Demo

If the working demo (`EN\64BIT\SDK DEMO.exe`) runs, then:
1. The DLLs are fine
2. You just need the VC++ runtime
3. Install the redistributables above

## Most Likely Fix

**Install Visual C++ 2015-2022 Redistributable (x64):**
https://aka.ms/vs/17/release/vc_redist.x64.exe

This usually fixes the issue.

