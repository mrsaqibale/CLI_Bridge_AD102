# Quick script to copy DLLs to output directories for testing
Write-Host "Copying DLLs to output directories..." -ForegroundColor Green

$dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")

# Console Debug
$consoleDebug = "UsbBridgeConsole\bin\Debug\net10.0-windows"
if (Test-Path $consoleDebug) {
    Write-Host "`nCopying to $consoleDebug..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $consoleDebug -Force
            Write-Host "  ✓ $dll" -ForegroundColor Gray
        } else {
            Write-Host "  ✗ $dll not found" -ForegroundColor Red
        }
    }
}

# Console Release
$consoleRelease = "UsbBridgeConsole\bin\Release\net10.0-windows"
if (Test-Path $consoleRelease) {
    Write-Host "`nCopying to $consoleRelease..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $consoleRelease -Force
            Write-Host "  ✓ $dll" -ForegroundColor Gray
        }
    }
}

# UI Debug
$uiDebug = "UsbBridgeUi\bin\Debug\net10.0-windows"
if (Test-Path $uiDebug) {
    Write-Host "`nCopying to $uiDebug..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $uiDebug -Force
            Write-Host "  ✓ $dll" -ForegroundColor Gray
        }
    }
}

# UI Release
$uiRelease = "UsbBridgeUi\bin\Release\net10.0-windows"
if (Test-Path $uiRelease) {
    Write-Host "`nCopying to $uiRelease..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $uiRelease -Force
            Write-Host "  ✓ $dll" -ForegroundColor Gray
        }
    }
}

Write-Host "`nDone! DLLs copied to output directories." -ForegroundColor Green
Write-Host "`nNow you can run:" -ForegroundColor Cyan
Write-Host "  cd UsbBridgeConsole" -ForegroundColor White
Write-Host "  dotnet run" -ForegroundColor White

