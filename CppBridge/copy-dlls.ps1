# Copy DLLs to output folders
# Run this after building

$dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")
$sourceDir = "..\EN\64BIT"
$outputDirs = @(
    "bin\x64\Debug",
    "bin\x64\Release"
)

foreach ($outputDir in $outputDirs) {
    $fullPath = Join-Path $PSScriptRoot $outputDir
    if (Test-Path $fullPath) {
        Write-Host "Copying DLLs to $outputDir..." -ForegroundColor Yellow
        foreach ($dll in $dlls) {
            $source = Join-Path $sourceDir $dll
            $dest = Join-Path $fullPath $dll
            if (Test-Path $source) {
                Copy-Item $source $dest -Force
                Write-Host "  ✓ $dll" -ForegroundColor Green
            } else {
                Write-Host "  ✗ $dll not found in $sourceDir" -ForegroundColor Red
            }
        }
    }
}

Write-Host "`nDone! DLLs are now in output folders." -ForegroundColor Green

