# Troubleshooting DLL Loading Issues

## Error: "Unable to load DLL 'USBBoxProtocol.dll' or one of its dependencies"

This error means the DLL file exists but cannot be loaded because:
1. **Missing Visual C++ Redistributable** (most common)
2. Missing other DLL dependencies
3. DLL architecture mismatch

## Solution 1: Install Visual C++ Redistributable

The DLLs were compiled with Visual C++ and require the VC++ Redistributable:

**For 64-bit (x64) applications:**
- Download and install: **Microsoft Visual C++ Redistributable for Visual Studio** (x64)
- Latest version: https://aka.ms/vs/17/release/vc_redist.x64.exe
- Or search for "Visual C++ Redistributable x64" on Microsoft website

**Verify installation:**
- Check in Control Panel > Programs > Programs and Features
- Look for "Microsoft Visual C++ 2015-2022 Redistributable (x64)"

## Solution 2: Check DLL Architecture

Ensure all DLLs match your application architecture:

```powershell
# Check if DLLs are 64-bit
Get-Item EN\64BIT\*.dll | Select-Object Name, Length
```

Your application must be built for **x64** to use 64-bit DLLs.

## Solution 3: Copy DLLs to Output Folder

Ensure DLLs are in the same folder as the executable:

```powershell
# Copy 64-bit DLLs to output
Copy-Item EN\64BIT\*.dll UsbBridgeUi\bin\Debug\net10.0-windows\
```

## Solution 4: Check Dependencies

Use Dependency Walker or dumpbin to check DLL dependencies:

```powershell
# If Visual Studio is installed
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe" /dependents EN\64BIT\USBBoxProtocol.dll
```

## Solution 5: Test with Working Demo

Run the working demo from EN folder to verify DLLs work:

```powershell
cd EN\64BIT
.\SDK DEMO.exe
```

If the working demo runs, the DLLs are fine - you just need VC++ Redistributable.

## Quick Fix Checklist

- [ ] Install Visual C++ Redistributable (x64)
- [ ] Ensure application is built for x64
- [ ] Copy all DLLs to output folder
- [ ] Restart application after installing redistributable
- [ ] Check Device Manager for USB device

## Common Error Codes

- **-1**: Device not initialized or not found
- **-2**: DLL not found (file missing)
- **-3**: DLL bitness mismatch (x64/x86)
- **-4**: DLL access error (dependencies missing)

