# Build executables for deployment to target system
Write-Host "Building executables for deployment..." -ForegroundColor Green

# Create output directory
$outputDir = "deployment"
if (Test-Path $outputDir) {
    Remove-Item $outputDir -Recurse -Force
}
New-Item -ItemType Directory -Path $outputDir | Out-Null

Write-Host "`n1. Building Console Bridge..." -ForegroundColor Yellow
dotnet publish UsbBridgeConsole\UsbBridgeConsole.csproj `
    -c Release `
    -r win-x64 `
    --self-contained true `
    -p:PublishSingleFile=true `
    -p:IncludeNativeLibrariesForSelfExtract=true `
    -o "$outputDir\UsbBridgeConsole"

# Copy DLLs to console output
Write-Host "`n   Copying DLLs..." -ForegroundColor Gray
$dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")
foreach ($dll in $dlls) {
    if (Test-Path $dll) {
        Copy-Item $dll "$outputDir\UsbBridgeConsole\" -Force
    }
}

Write-Host "`n2. Building UI Application..." -ForegroundColor Yellow
dotnet publish UsbBridgeUi\UsbBridgeUi.csproj `
    -c Release `
    -r win-x64 `
    --self-contained true `
    -p:PublishSingleFile=true `
    -p:IncludeNativeLibrariesForSelfExtract=true `
    -o "$outputDir\UsbBridgeUi"

# Copy DLLs to UI output
Write-Host "`n   Copying DLLs..." -ForegroundColor Gray
foreach ($dll in $dlls) {
    if (Test-Path $dll) {
        Copy-Item $dll "$outputDir\UsbBridgeUi\" -Force
    }
}

# Create deployment README
$readme = @"
USB Device Bridge - Deployment Package
======================================

This folder contains two applications:

1. UsbBridgeConsole/
   - Console application for Electron integration
   - Run: .\UsbBridgeConsole.exe
   - Communicates via JSON stdin/stdout

2. UsbBridgeUi/
   - Windows Forms UI for testing
   - Run: .\UsbBridgeUi.exe
   - Graphical interface for device control

Installation:
-------------
1. Copy the entire 'deployment' folder to your target system
2. Ensure all DLLs are in the same folder as the .exe files
3. Run the executable on the system with USB device connected

Requirements:
-------------
- Windows 10/11 (64-bit)
- USB device drivers installed
- USB device physically connected

Testing:
--------
1. First test with UI: Run UsbBridgeUi.exe
2. Then use console for Electron: Run UsbBridgeConsole.exe
"@

$readme | Out-File -FilePath "$outputDir\README.txt" -Encoding UTF8

Write-Host "`n========================================" -ForegroundColor Green
Write-Host "Build completed!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "`nOutput folder: $outputDir" -ForegroundColor Cyan
Write-Host "`nContents:" -ForegroundColor Yellow
Write-Host "  - UsbBridgeConsole\ (Console app for Electron)" -ForegroundColor White
Write-Host "  - UsbBridgeUi\ (UI app for testing)" -ForegroundColor White
Write-Host "`nCopy the '$outputDir' folder to your target system!" -ForegroundColor Green

