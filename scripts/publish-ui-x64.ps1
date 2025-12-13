# Publish UI application for x64 Windows
Write-Host "Publishing UsbBridgeUi for win-x64..." -ForegroundColor Green

$projectPath = "..\UsbBridgeUi\UsbBridgeUi.csproj"
$outputPath = "..\publish\UsbBridgeUi-win-x64"

if (Test-Path $outputPath) {
    Remove-Item $outputPath -Recurse -Force
}

dotnet publish $projectPath `
    -c Release `
    -r win-x64 `
    --self-contained true `
    -p:PublishSingleFile=true `
    -p:IncludeNativeLibrariesForSelfExtract=true `
    -o $outputPath

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nPublish successful!" -ForegroundColor Green
    Write-Host "Output: $outputPath" -ForegroundColor Cyan
    
    # Copy DLLs
    Write-Host "`nCopying vendor DLLs..." -ForegroundColor Yellow
    $dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")
    foreach ($dll in $dlls) {
        $src = "..\$dll"
        if (Test-Path $src) {
            Copy-Item $src $outputPath -Force
            Write-Host "  Copied $dll" -ForegroundColor Gray
        } else {
            Write-Host "  Warning: $dll not found" -ForegroundColor Yellow
        }
    }
    
    Write-Host "`nDone! Executable: $outputPath\UsbBridgeUi.exe" -ForegroundColor Green
} else {
    Write-Host "`nPublish failed!" -ForegroundColor Red
    exit 1
}

