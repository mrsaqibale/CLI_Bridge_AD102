# Copy DLLs to output directories
Write-Host "Copying DLLs to output directories..." -ForegroundColor Green

$dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")

# Copy to Console output
$consoleOutput = "UsbBridgeConsole\bin\Debug\net10.0-windows"
if (Test-Path $consoleOutput) {
    Write-Host "Copying to $consoleOutput..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $consoleOutput -Force
            Write-Host "  Copied $dll" -ForegroundColor Gray
        }
    }
}

$consoleRelease = "UsbBridgeConsole\bin\Release\net10.0-windows"
if (Test-Path $consoleRelease) {
    Write-Host "Copying to $consoleRelease..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $consoleRelease -Force
            Write-Host "  Copied $dll" -ForegroundColor Gray
        }
    }
}

# Copy to UI output
$uiOutput = "UsbBridgeUi\bin\Debug\net10.0-windows"
if (Test-Path $uiOutput) {
    Write-Host "Copying to $uiOutput..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $uiOutput -Force
            Write-Host "  Copied $dll" -ForegroundColor Gray
        }
    }
}

$uiRelease = "UsbBridgeUi\bin\Release\net10.0-windows"
if (Test-Path $uiRelease) {
    Write-Host "Copying to $uiRelease..." -ForegroundColor Yellow
    foreach ($dll in $dlls) {
        if (Test-Path $dll) {
            Copy-Item $dll $uiRelease -Force
            Write-Host "  Copied $dll" -ForegroundColor Gray
        }
    }
}

Write-Host "`nDone!" -ForegroundColor Green

