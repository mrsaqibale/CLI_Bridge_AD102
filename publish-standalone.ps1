# Publish standalone executable for deployment to target machine
Write-Host "Publishing standalone executable..." -ForegroundColor Green

$outputPath = "publish\UsbBridgeConsole-standalone"

# Clean previous build
if (Test-Path $outputPath) {
    Remove-Item $outputPath -Recurse -Force
}

# Publish console bridge as self-contained single file
Write-Host "`nBuilding UsbBridgeConsole..." -ForegroundColor Yellow
dotnet publish UsbBridgeConsole\UsbBridgeConsole.csproj `
    -c Release `
    -r win-x64 `
    --self-contained true `
    -p:PublishSingleFile=true `
    -p:IncludeNativeLibrariesForSelfExtract=true `
    -p:IncludeAllContentForSelfExtract=true `
    -o $outputPath

if ($LASTEXITCODE -ne 0) {
    Write-Host "`nBuild failed!" -ForegroundColor Red
    exit 1
}

# Copy vendor DLLs
Write-Host "`nCopying vendor DLLs..." -ForegroundColor Yellow
$dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")
foreach ($dll in $dlls) {
    if (Test-Path $dll) {
        Copy-Item $dll $outputPath -Force
        Write-Host "  Copied $dll" -ForegroundColor Gray
    } else {
        Write-Host "  Warning: $dll not found in root directory" -ForegroundColor Yellow
    }
}

# Create a README for deployment
$readme = @"
USB Bridge Console - Standalone Deployment
==========================================

This folder contains the standalone executable for the USB Device Bridge.

Files:
- UsbBridgeConsole.exe - Main executable
- USBBoxProtocol.dll - USB Box protocol library
- AD101Device.dll - AD101 device library  
- AD800Device.dll - AD800 device library
- lame_enc.dll - Audio encoding library (optional)

Usage:
------
1. Copy this entire folder to your target machine (the one with USB device connected)
2. Open PowerShell or Command Prompt in this folder
3. Run: .\UsbBridgeConsole.exe

Testing:
--------
# Test with a command:
echo '{"cmd":"usbbox.enable","deviceType":"F2"}' | .\UsbBridgeConsole.exe

# Or run interactively and type JSON commands:
.\UsbBridgeConsole.exe

Example Commands:
-----------------
{"cmd":"usbbox.enable","deviceType":"F2"}
{"cmd":"usbbox.disable"}
{"cmd":"usbbox.pickup","line":0}
{"cmd":"usbbox.hookon","line":0}
{"cmd":"usbbox.dial","line":0,"number":"1234567890"}
{"cmd":"usbbox.getcallerid","line":0}
{"cmd":"usbbox.getlineinfo","line":0}

For Electron Integration:
--------------------------
Spawn this executable as a child process and communicate via stdin/stdout.

See main README.md for full documentation.
"@

$readme | Out-File -FilePath "$outputPath\README.txt" -Encoding UTF8

Write-Host "`n========================================" -ForegroundColor Green
Write-Host "Publish completed successfully!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "`nOutput folder: $outputPath" -ForegroundColor Cyan
Write-Host "`nNext steps:" -ForegroundColor Yellow
Write-Host "1. Copy the entire '$outputPath' folder to your target machine" -ForegroundColor White
Write-Host "2. On the target machine, run: .\UsbBridgeConsole.exe" -ForegroundColor White
Write-Host "`nAll DLLs have been included in the output folder." -ForegroundColor Gray

