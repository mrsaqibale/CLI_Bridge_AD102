# Building C++ Bridge - Step by Step Guide

## Step 1: Install C++ Workload in Visual Studio

1. **Open Visual Studio Installer**
   - Search for "Visual Studio Installer" in Windows Start menu
   - Or open it from: `C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe`

2. **Click "Modify" on your Visual Studio installation**

3. **Select "Desktop development with C++" workload**
   - Check the box for "Desktop development with C++"
   - This includes:
     - MSVC v143 compiler
     - Windows SDK
     - CMake tools
     - All necessary C++ libraries

4. **Click "Modify" to install**
   - Wait for installation to complete (may take 10-30 minutes)

## Step 2: Open the Project in Visual Studio

1. **Open Visual Studio 2022**

2. **Open the Solution File:**
   - File → Open → Project/Solution
   - Navigate to: `C:\Users\DOCTOR\Documents\gpos\bridge\CppBridge\`
   - Select: `CppBridge.sln`
   - Click "Open"

   **OR** simply double-click `CppBridge.sln` in Windows Explorer

## Step 3: Configure Build Settings

1. **Set Platform to x64:**
   - At the top toolbar, find the dropdown that says "Debug" or "Release"
   - Next to it, change platform from "x86" to **"x64"**

2. **Set Configuration to Release:**
   - Change from "Debug" to **"Release"** (for production)

## Step 4: Build the Project

1. **Build Menu → Build Solution**
   - Or press `Ctrl+Shift+B`
   - Or right-click "CppBridge" in Solution Explorer → Build

2. **Check Output Window:**
   - Should see: `Build succeeded`
   - Executable will be at: `CppBridge\bin\x64\Release\CppBridge.exe`

## Step 5: Copy DLLs to Output Folder

After building, copy the DLLs:

```powershell
# Run this in PowerShell from the bridge folder
Copy-Item EN\64BIT\USBBoxProtocol.dll CppBridge\bin\x64\Release\ -Force
Copy-Item EN\64BIT\AD101Device.dll CppBridge\bin\x64\Release\ -Force
Copy-Item EN\64BIT\AD800Device.dll CppBridge\bin\x64\Release\ -Force
Copy-Item EN\64BIT\lame_enc.dll CppBridge\bin\x64\Release\ -Force
```

## Step 6: Test the Bridge

```powershell
cd CppBridge\bin\x64\Release
echo '{"cmd":"enable","deviceType":"F2"}' | .\CppBridge.exe
```

## Troubleshooting

### If you get "Cannot open include file" error:
- Make sure the include path is correct in project settings
- Right-click project → Properties → C/C++ → General → Additional Include Directories
- Should include: `..\EN\64BIT`

### If you get "Cannot open library file" error:
- Right-click project → Properties → Linker → General → Additional Library Directories
- Should include: `..\EN\64BIT`
- Linker → Input → Additional Dependencies should have: `USBBoxProtocol.lib`

### If DLLs are not found at runtime:
- Make sure DLLs are in the same folder as CppBridge.exe
- Check: `CppBridge\bin\x64\Release\` folder

## Quick Reference

**Project Location:**
```
C:\Users\DOCTOR\Documents\gpos\bridge\CppBridge\CppBridge.sln
```

**Output Location:**
```
CppBridge\bin\x64\Release\CppBridge.exe
```

**Required DLLs Location:**
```
EN\64BIT\*.dll
```

