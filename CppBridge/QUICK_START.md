# Quick Start Guide - Visual Studio 2022

## Step 1: Install C++ Workload

1. **Open Visual Studio Installer:**
   - Press `Windows Key` and search: **"Visual Studio Installer"**
   - Click to open it

2. **Click "Modify"** on your Visual Studio 2022 installation

3. **Check this box:**
   - ✅ **"Desktop development with C++"**
   - This installs all C++ tools you need

4. **Click "Modify"** button at bottom right
   - Wait for installation (10-30 minutes)

## Step 2: Open the Project

**Option A: Double-click the file:**
- Go to: `C:\Users\DOCTOR\Documents\gpos\bridge\CppBridge\`
- Double-click: **`CppBridge.sln`**
- Visual Studio will open automatically

**Option B: From Visual Studio:**
1. Open Visual Studio 2022
2. File → Open → Project/Solution
3. Navigate to: `C:\Users\DOCTOR\Documents\gpos\bridge\CppBridge\`
4. Select: **`CppBridge.sln`**
5. Click "Open"

## Step 3: Build the Project

1. **At the top toolbar, select:**
   - Configuration: **"Release"** (dropdown)
   - Platform: **"x64"** (dropdown next to it)

2. **Build the solution:**
   - Menu: **Build → Build Solution**
   - Or press: **`Ctrl + Shift + B`**

3. **Wait for build to complete:**
   - Check bottom "Output" window
   - Should say: **"Build succeeded"**

## Step 4: Copy DLLs

After building, run this in PowerShell:

```powershell
cd C:\Users\DOCTOR\Documents\gpos\bridge
Copy-Item EN\64BIT\*.dll CppBridge\bin\x64\Release\ -Force
```

## Step 5: Test

```powershell
cd CppBridge\bin\x64\Release
echo '{"cmd":"enable","deviceType":"F2"}' | .\CppBridge.exe
```

## That's it! 🎉

Your executable is at:
```
C:\Users\DOCTOR\Documents\gpos\bridge\CppBridge\bin\x64\Release\CppBridge.exe
```

