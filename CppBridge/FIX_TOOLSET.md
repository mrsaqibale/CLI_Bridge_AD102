# Fix: Platform Toolset Error

## Error Message
```
error MSB8020: The build tools for Visual Studio 2022 (Platform Toolset = 'v143') cannot be found.
```

## Solution Options

### Option 1: Retarget in Visual Studio (Easiest)

1. **Right-click on the solution** in Solution Explorer
2. Select **"Retarget solution"**
3. Choose the available toolset (usually v142 or v143)
4. Click **OK**
5. Build again

### Option 2: Install v143 Toolset

1. Open **Visual Studio Installer**
2. Click **"Modify"** on your installation
3. Under **"Individual components"**, search for:
   - **"MSVC v143 - VS 2022 C++ x64/x86 build tools"**
4. Check it and click **"Modify"**

### Option 3: Use v142 (VS 2019) - Already Fixed

I've updated the project file to use **v142** (Visual Studio 2019 toolset) which is more widely available.

**Just rebuild the project now!**

## Check Available Toolsets

To see what toolsets you have:
1. Right-click project → **Properties**
2. Go to **Configuration Properties → General**
3. Look at **"Platform Toolset"** dropdown
4. Select the one that's available

## Common Toolsets:
- **v143** = Visual Studio 2022
- **v142** = Visual Studio 2019
- **v141** = Visual Studio 2017
- **v140** = Visual Studio 2015

