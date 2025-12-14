# Build Standalone EXE for Deployment

## Step 1: Build Release Version

1. **Open Visual Studio**
2. **Open:** `CppBridge.sln`
3. **Set Configuration to:** `Release`
4. **Set Platform to:** `x64`
5. **Right-click on project** → **Properties**
6. **Configuration Properties** → **C/C++** → **Code Generation**
7. **Runtime Library:** Change to **"Multi-threaded (/MT)"** (for static linking)
   - This makes the EXE standalone (no VC++ runtime needed)
8. **Build** → **Build Solution** (or `Ctrl+Shift+B`)

## Step 2: Copy DLLs

After building, copy DLLs to the Release folder:

```powershell
Copy-Item EN\64BIT\*.dll CppBridge\bin\x64\Release\ -Force
```

## Step 3: Test the EXE

```powershell
cd CppBridge\bin\x64\Release
.\CppBridgeMenu.exe
```

## Step 4: Deploy to Other System

Copy these files to the target system:
- `CppBridgeMenu.exe`
- `USBBoxProtocol.dll`
- `AD101Device.dll`
- `AD800Device.dll`
- `lame_enc.dll`

**Note:** The target system still needs:
- Visual C++ 2005 Redistributable (x64)
- Npcap (for wpcap.dll)

OR use static linking (Multi-threaded /MT) to avoid VC++ runtime.

## Alternative: Create Installer

You can create a simple installer that:
1. Copies EXE and DLLs
2. Checks/installs VC++ 2005 Redistributable
3. Checks/installs Npcap
4. Creates desktop shortcut

