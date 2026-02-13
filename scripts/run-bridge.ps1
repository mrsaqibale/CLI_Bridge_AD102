# Build and Run C++ Bridge
# This script builds the C++ bridge and creates the executable

param(
    [switch]$BuildOnly,
    [switch]$RunOnly
)

$ErrorActionPreference = "Stop"

# Paths
$solutionPath = "..\CppBridge\CppBridge.sln"
$projectPath = "..\CppBridge\CppBridge.vcxproj"
$outputPath = "..\CppBridge\bin\x64\Release"
$exePath = "$outputPath\CppBridge.exe"

Write-Host "========================================" -ForegroundColor Green
Write-Host "  C++ Bridge Build Script" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""

# Check if Visual Studio MSBuild is available
$msbuildPaths = @(
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)

$msbuild = $null
foreach ($path in $msbuildPaths) {
    if (Test-Path $path) {
        $msbuild = $path
        break
    }
}

if (-not $msbuild) {
    Write-Host "ERROR: MSBuild not found!" -ForegroundColor Red
    Write-Host "`nPlease install Visual Studio with C++ workload:" -ForegroundColor Yellow
    Write-Host "1. Open Visual Studio Installer" -ForegroundColor White
    Write-Host "2. Modify your Visual Studio installation" -ForegroundColor White
    Write-Host "3. Check 'Desktop development with C++'" -ForegroundColor White
    Write-Host "4. Click Modify to install" -ForegroundColor White
    Write-Host "`nOr build manually in Visual Studio:" -ForegroundColor Yellow
    Write-Host "1. Open: CppBridge\CppBridge.sln" -ForegroundColor White
    Write-Host "2. Set Configuration: Release, Platform: x64" -ForegroundColor White
    Write-Host "3. Build → Build Solution (Ctrl+Shift+B)" -ForegroundColor White
    exit 1
}

Write-Host "Found MSBuild: $msbuild" -ForegroundColor Cyan
Write-Host ""

# Build the project
if (-not $RunOnly) {
    Write-Host "Building C++ Bridge..." -ForegroundColor Yellow
    Write-Host "Solution: $solutionPath" -ForegroundColor Gray
    Write-Host "Configuration: Release, Platform: x64" -ForegroundColor Gray
    Write-Host ""
    
    # Build using MSBuild
    & $msbuild $solutionPath `
        /p:Configuration=Release `
        /p:Platform=x64 `
        /t:Build `
        /m `
        /v:minimal
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "`nERROR: Build failed!" -ForegroundColor Red
        Write-Host "`nTroubleshooting:" -ForegroundColor Yellow
        Write-Host "1. Make sure Visual Studio C++ tools are installed" -ForegroundColor White
        Write-Host "2. Try building manually in Visual Studio first" -ForegroundColor White
        Write-Host "3. Check for toolset errors (retarget solution)" -ForegroundColor White
        exit 1
    }
    
    Write-Host ""
    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host ""
}

# Check if .exe exists
if (-not (Test-Path $exePath)) {
    Write-Host "ERROR: CppBridge.exe not found at: $exePath" -ForegroundColor Red
    Write-Host "`nThe build may have failed. Please check:" -ForegroundColor Yellow
    Write-Host "1. Build output for errors" -ForegroundColor White
    Write-Host "2. Try building in Visual Studio manually" -ForegroundColor White
    exit 1
}

# Copy DLLs to output folder
Write-Host "Copying DLLs to output folder..." -ForegroundColor Yellow
$dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")
$dllSource = "..\EN\64BIT"

foreach ($dll in $dlls) {
    $src = "$dllSource\$dll"
    if (Test-Path $src) {
        Copy-Item $src $outputPath -Force
        Write-Host "  ✓ Copied $dll" -ForegroundColor Gray
    } else {
        # Try root folder
        $srcRoot = "..\$dll"
        if (Test-Path $srcRoot) {
            Copy-Item $srcRoot $outputPath -Force
            Write-Host "  ✓ Copied $dll (from root)" -ForegroundColor Gray
        } else {
            Write-Host "  ⚠ Warning: $dll not found" -ForegroundColor Yellow
        }
    }
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Build Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Executable location:" -ForegroundColor Cyan
Write-Host "  $exePath" -ForegroundColor White
Write-Host ""

# Get file info
$fileInfo = Get-Item $exePath
Write-Host "File size: $([math]::Round($fileInfo.Length / 1MB, 2)) MB" -ForegroundColor Gray
Write-Host "Last modified: $($fileInfo.LastWriteTime)" -ForegroundColor Gray
Write-Host ""

# Run the executable if not BuildOnly
if (-not $BuildOnly) {
    Write-Host "Starting C++ Bridge..." -ForegroundColor Yellow
    Write-Host "Press Ctrl+C to stop`n" -ForegroundColor Gray
    Write-Host "----------------------------------------" -ForegroundColor Gray
    Write-Host ""
    
    # Change to output directory and run
    Push-Location $outputPath
    try {
        & .\CppBridge.exe
    } finally {
        Pop-Location
    }
} else {
    Write-Host "To run the bridge, execute:" -ForegroundColor Cyan
    Write-Host "  cd $outputPath" -ForegroundColor White
    Write-Host "  .\CppBridge.exe" -ForegroundColor White
    Write-Host ""
    Write-Host "Or run this script without -BuildOnly flag" -ForegroundColor Gray
}

