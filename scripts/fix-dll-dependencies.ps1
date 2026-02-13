# Fix DLL Dependencies for CppBridge
# This script helps identify and fix missing DLL dependencies

Write-Host "========================================" -ForegroundColor Green
Write-Host "  DLL Dependency Checker" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

$missing = @()
$found = @()

# Check for Visual C++ 2005 Runtime
Write-Host "Checking Visual C++ 2005 Runtime..." -ForegroundColor Yellow
$msvcp80 = Test-Path "C:\Windows\System32\MSVCP80.dll"
$msvcr80 = Test-Path "C:\Windows\System32\MSVCR80.dll"

if ($msvcp80) {
    Write-Host "  ✓ MSVCP80.dll found" -ForegroundColor Green
    $found += "MSVCP80.dll"
} else {
    Write-Host "  ✗ MSVCP80.dll MISSING" -ForegroundColor Red
    $missing += "MSVCP80.dll"
}

if ($msvcr80) {
    Write-Host "  ✓ MSVCR80.dll found" -ForegroundColor Green
    $found += "MSVCR80.dll"
} else {
    Write-Host "  ✗ MSVCR80.dll MISSING" -ForegroundColor Red
    $missing += "MSVCR80.dll"
}

Write-Host ""

# Check for wpcap.dll (optional, for network features)
Write-Host "Checking wpcap.dll (optional)..." -ForegroundColor Yellow
$wpcap = Test-Path "C:\Windows\System32\wpcap.dll"
if ($wpcap) {
    Write-Host "  ✓ wpcap.dll found" -ForegroundColor Green
} else {
    Write-Host "  ⚠ wpcap.dll not found (may be optional)" -ForegroundColor Yellow
}

Write-Host ""

# Check DLLs in Release folder
Write-Host "Checking DLLs in Release folder..." -ForegroundColor Yellow
$releasePath = "..\CppBridge\bin\x64\Release"
$requiredDlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")

foreach ($dll in $requiredDlls) {
    $dllPath = Join-Path $releasePath $dll
    if (Test-Path $dllPath) {
        Write-Host "  ✓ $dll found" -ForegroundColor Green
    } else {
        Write-Host "  ✗ $dll MISSING" -ForegroundColor Red
        $missing += $dll
    }
}

Write-Host ""

# Summary
if ($missing.Count -eq 0) {
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "  All dependencies found!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
} else {
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "  Missing Dependencies Found!" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    Write-Host ""
    
    if ($missing -contains "MSVCP80.dll" -or $missing -contains "MSVCR80.dll") {
        Write-Host "SOLUTION: Install Visual C++ 2005 Redistributable" -ForegroundColor Yellow
        Write-Host ""
        Write-Host "Download and install:" -ForegroundColor White
        Write-Host "  https://www.microsoft.com/en-us/download/details.aspx?id=18471" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "Or install the latest (recommended):" -ForegroundColor White
        Write-Host "  https://aka.ms/vs/17/release/vc_redist.x64.exe" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "After installing, restart your application." -ForegroundColor Gray
    }
    
    if ($missing -contains "wpcap.dll") {
        Write-Host ""
        Write-Host "OPTIONAL: Install Npcap (for network features)" -ForegroundColor Yellow
        Write-Host "  https://npcap.com/dist/" -ForegroundColor Cyan
        Write-Host "  (Only needed if using network dialing features)" -ForegroundColor Gray
    }
    
    # Check if DLLs need to be copied
    $sourceDlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")
    $needsCopy = $false
    foreach ($dll in $sourceDlls) {
        $src = "..\EN\64BIT\$dll"
        $dst = "$releasePath\$dll"
        if ((Test-Path $src) -and -not (Test-Path $dst)) {
            $needsCopy = $true
            break
        }
    }
    
    if ($needsCopy) {
        Write-Host ""
        Write-Host "Copying DLLs from EN\64BIT..." -ForegroundColor Yellow
        foreach ($dll in $sourceDlls) {
            $src = "..\EN\64BIT\$dll"
            $dst = "$releasePath\$dll"
            if (Test-Path $src) {
                Copy-Item $src $dst -Force
                Write-Host "  ✓ Copied $dll" -ForegroundColor Green
            }
        }
    }
}

Write-Host ""

