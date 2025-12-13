# Check DLL dependencies using dumpbin if available
Write-Host "Checking DLL dependencies..." -ForegroundColor Green

$dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll")

foreach ($dll in $dlls) {
    $dllPath = "EN\64BIT\$dll"
    if (Test-Path $dllPath) {
        Write-Host "`nChecking $dll..." -ForegroundColor Yellow
        
        # Try to use dumpbin if Visual Studio is installed
        $dumpbin = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe"
        $dumpbinPath = Get-ChildItem $dumpbin -ErrorAction SilentlyContinue | Select-Object -First 1
        
        if ($dumpbinPath) {
            Write-Host "  Using dumpbin to check dependencies..." -ForegroundColor Gray
            & $dumpbinPath /dependents $dllPath | Select-String -Pattern "\.dll|\.DLL" | ForEach-Object {
                Write-Host "    Dependency: $_" -ForegroundColor Cyan
            }
        } else {
            Write-Host "  dumpbin not found. Install Visual Studio or use Dependency Walker." -ForegroundColor Yellow
        }
        
        # Check file properties
        $fileInfo = Get-Item $dllPath
        Write-Host "  Size: $($fileInfo.Length) bytes" -ForegroundColor Gray
        Write-Host "  Last Modified: $($fileInfo.LastWriteTime)" -ForegroundColor Gray
    }
}

Write-Host "`nNote: If DLLs fail to load, you may need:" -ForegroundColor Yellow
Write-Host "  1. Visual C++ Redistributable (x64)" -ForegroundColor White
Write-Host "  2. USB device drivers installed" -ForegroundColor White
Write-Host "  3. All DLLs in the same folder as the executable" -ForegroundColor White

