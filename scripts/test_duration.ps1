# Test Call Duration Script
# Builds the C++ bridge and listens for calls, displaying type and duration

$ErrorActionPreference = "Continue"

# Get script directory and resolve paths
if ($PSScriptRoot) {
    $scriptDir = $PSScriptRoot
} else {
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
}
$projectRoot = Split-Path -Parent $scriptDir
$bridgePath = Join-Path $projectRoot "CppBridge\bin\x64\Release\CppBridge.exe"
$solutionPath = Join-Path $projectRoot "CppBridge\CppBridge.sln"
$projectPath = Join-Path $projectRoot "CppBridge\CppBridge.vcxproj"
$outputPath = Join-Path $projectRoot "CppBridge\bin\x64\Release"

# Find MSBuild
function Find-MSBuild {
    $msbuildPaths = @(
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles}\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    )
    
    foreach ($path in $msbuildPaths) {
        if (Test-Path $path) {
            return $path
        }
    }
    return $null
}

# Build the C++ bridge
function Build-Bridge {
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host "  Building C++ Bridge" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
    
    $msbuild = Find-MSBuild
    if (-not $msbuild) {
        Write-Host "WARNING: MSBuild not found!" -ForegroundColor Yellow
        Write-Host "Skipping build. Using existing executable if available." -ForegroundColor Yellow
        Write-Host ""
        return $false
    }
    
    Write-Host "Found MSBuild: $msbuild" -ForegroundColor Gray
    Write-Host "Building solution: $solutionPath" -ForegroundColor Gray
    Write-Host "Configuration: Release, Platform: x64" -ForegroundColor Gray
    Write-Host ""
    
    try {
        & $msbuild $solutionPath `
            /p:Configuration=Release `
            /p:Platform=x64 `
            /t:Build `
            /m `
            /v:minimal
        
        if ($LASTEXITCODE -ne 0) {
            Write-Host "`nWARNING: Build failed with exit code $LASTEXITCODE" -ForegroundColor Yellow
            Write-Host "Will try to use existing executable if available." -ForegroundColor Yellow
            Write-Host ""
            return $false
        }
        
        Write-Host ""
        Write-Host "✓ Build completed successfully!" -ForegroundColor Green
        Write-Host ""
        
        # Copy DLLs to output folder
        Write-Host "Copying DLLs..." -ForegroundColor Gray
        $dlls = @("USBBoxProtocol.dll", "AD101Device.dll", "AD800Device.dll", "lame_enc.dll")
        $dllSource = Join-Path $projectRoot "EN\64BIT"
        
        foreach ($dll in $dlls) {
            $src = Join-Path $dllSource $dll
            if (Test-Path $src) {
                Copy-Item $src $outputPath -Force -ErrorAction SilentlyContinue | Out-Null
            }
        }
        
        return $true
    } catch {
        Write-Host "`nWARNING: Build error: $_" -ForegroundColor Yellow
        Write-Host "Will try to use existing executable if available." -ForegroundColor Yellow
        Write-Host ""
        return $false
    }
}

# Display call information with type and duration
function Show-CallInfo {
    param($event)
    
    $timestamp = Get-Date -Format "HH:mm:ss"
    
    # Incoming call - RINGING
    if ($event.status -eq "Ringing") {
        $callerInfo = if ($event.callerId -and $event.callerId -ne "") { 
            "Number: $($event.callerId)" 
        } else { 
            "(No Caller ID)" 
        }
        $callType = if ($event.callType) { $event.callType } else { "Inbound" }
        Write-Host "[$timestamp] 📞 NEW CALL - Type: $callType - $callerInfo" -ForegroundColor Green
    }
    # Call active - IN CALL
    elseif ($event.status -eq "InCall") {
        $callType = if ($event.callType) { $event.callType } else { "Unknown" }
        Write-Host "[$timestamp] 📱 CALL ACTIVE - Type: $callType (Line $($event.line))" -ForegroundColor Cyan
    }
    # Call duration update
    elseif ($event.duration -ne $null) {
        $seconds = [math]::Round($event.duration / 1000.0, 1)
        $minutes = [math]::Floor($seconds / 60)
        $remainingSeconds = [math]::Round($seconds % 60, 1)
        $callType = if ($event.callType) { $event.callType } else { "Unknown" }
        
        if ($minutes -gt 0) {
            Write-Host "[$timestamp] ⏱️  DURATION - Type: $callType - $minutes min $remainingSeconds sec ($($event.duration) ms)" -ForegroundColor Yellow
        } else {
            Write-Host "[$timestamp] ⏱️  DURATION - Type: $callType - $seconds sec ($($event.duration) ms)" -ForegroundColor Yellow
        }
    }
    # Call ended - FREE
    elseif ($event.status -eq "Free") {
        Write-Host "[$timestamp] ✓ Call ended (Line $($event.line))" -ForegroundColor Gray
    }
    # Missed call
    elseif ($event.status -eq "Missed") {
        $callerInfo = if ($event.callerId -and $event.callerId -ne "") { 
            "Number: $($event.callerId)" 
        } else { 
            "(No Caller ID)" 
        }
        $callType = if ($event.callType) { $event.callType } else { "Inbound" }
        Write-Host "[$timestamp] ❌ MISSED CALL - Type: $callType - $callerInfo" -ForegroundColor Red
    }
    # Other events
    elseif ($event.type -eq "event" -and $event.meaning) {
        Write-Host "[$timestamp] ℹ️  $($event.meaning)" -ForegroundColor DarkGray
    }
}

# Start listening for calls
function Start-Listening {
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host "  Listening for Calls..." -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Device will be enabled automatically." -ForegroundColor Green
    Write-Host "Press Ctrl+C to stop listening`n" -ForegroundColor Yellow
    
    # Check if bridge exists
    if (-not $bridgePath -or -not (Test-Path $bridgePath)) {
        Write-Host "ERROR: Bridge not found!" -ForegroundColor Red
        Write-Host "Tried path: $bridgePath" -ForegroundColor Yellow
        Write-Host "`nPlease build the C++ bridge first." -ForegroundColor Yellow
        return
    }
    
    Write-Host "Bridge path: $bridgePath" -ForegroundColor Gray
    Write-Host ""
    
    # Start the bridge process
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = $bridgePath
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $psi.RedirectStandardInput = $true
    $psi.CreateNoWindow = $true
    
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $psi
    
    try {
        # Start the process
        $process.Start() | Out-Null
        
        Write-Host "Bridge process started (PID: $($process.Id))" -ForegroundColor Gray
        Write-Host "Waiting for bridge to initialize...`n" -ForegroundColor Gray
        
        # Read stderr (status messages) in background
        $stderrScriptBlock = {
            param($reader)
            try {
                while (-not $reader.EndOfStream) {
                    $line = $reader.ReadLine()
                    if ($line -and $line.Trim() -ne "") {
                        if ($line -match "started|enabled|Listening") {
                            Write-Host "✓ $line" -ForegroundColor Green
                        } elseif ($line -match "Failed|Error") {
                            Write-Host "✗ $line" -ForegroundColor Red
                        } else {
                            Write-Host $line -ForegroundColor DarkGray
                        }
                    }
                }
            } catch {
                # End of stream
            }
        }
        
        $stderrJob = Start-Job -ScriptBlock $stderrScriptBlock -ArgumentList $process.StandardError
        
        # Give process time to initialize
        Start-Sleep -Milliseconds 500
        
        # Check if process exited immediately
        if ($process.HasExited) {
            $exitCode = $process.ExitCode
            Write-Host "`nBridge process exited immediately with code: $exitCode" -ForegroundColor Red
            
            # Check for DLL dependency error
            if ($exitCode -eq -1073741515 -or $exitCode -eq 0xC0000135) {
                Write-Host "`n========================================" -ForegroundColor Red
                Write-Host "  DLL DEPENDENCY ERROR DETECTED!" -ForegroundColor Red
                Write-Host "========================================" -ForegroundColor Red
                Write-Host "`nThe bridge requires Visual C++ 2005 Runtime DLLs." -ForegroundColor Yellow
                Write-Host "`nSOLUTION: Install Visual C++ Redistributable" -ForegroundColor Cyan
                Write-Host ""
                Write-Host "Download and install:" -ForegroundColor White
                Write-Host "  https://aka.ms/vs/17/release/vc_redist.x64.exe" -ForegroundColor Green
                Write-Host ""
            }
            
            Remove-Job $stderrJob -Force -ErrorAction SilentlyContinue | Out-Null
            return
        }
        
        Write-Host "✓ Bridge is running and listening for calls...`n" -ForegroundColor Green
        
        # Use event-based async reading for stdout (non-blocking)
        $stdoutHandler = {
            param($sender, $e)
            if ($e.Data -and $e.Data.Trim() -ne "") {
                try {
                    $json = $e.Data | ConvertFrom-Json -ErrorAction Stop
                    if ($json -and $json.type -eq "event") {
                        # Display call information immediately
                        Show-CallInfo -event $json
                    }
                } catch {
                    # Not JSON, ignore
                }
            }
        }
        
        # Register event handler
        Register-ObjectEvent -InputObject $process -EventName OutputDataReceived -Action $stdoutHandler | Out-Null
        
        # Start async reading
        $process.BeginOutputReadLine()
        
        # Wait for process (it will keep running until stdin is closed or killed)
        while (-not $process.HasExited) {
            Start-Sleep -Milliseconds 500
        }
        
        # Wait for stderr job
        Wait-Job $stderrJob -Timeout 2 | Out-Null
        Remove-Job $stderrJob -Force -ErrorAction SilentlyContinue | Out-Null
        
        if ($process.HasExited) {
            Write-Host "`nBridge process exited (Code: $($process.ExitCode))" -ForegroundColor Yellow
        }
        
    } catch [System.Management.Automation.PipelineStoppedException] {
        # User pressed Ctrl+C
        Write-Host "`n`nStopping bridge..." -ForegroundColor Yellow
        try {
            $process.StandardInput.Close()
        } catch {
            # Ignore
        }
        if (-not $process.HasExited) {
            $process.Kill()
            $process.WaitForExit(2000)
        }
    } catch {
        Write-Host "`nError: $_" -ForegroundColor Red
        Write-Host $_.Exception.Message -ForegroundColor Red
    } finally {
        # Cleanup
        if (-not $process.HasExited) {
            try {
                Write-Host "`nStopping bridge..." -ForegroundColor Yellow
                try {
                    $process.StandardInput.Close()
                } catch {
                    # Ignore
                }
                $process.Kill()
                $process.WaitForExit(2000)
            } catch {
                # Ignore
            }
        }
        try {
            $process.Dispose()
        } catch {
            # Ignore
        }
    }
}

# Main execution
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Call Duration Test Tool" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "This script will:" -ForegroundColor White
Write-Host "  1. Build the C++ bridge" -ForegroundColor Gray
Write-Host "  2. Listen for incoming calls" -ForegroundColor Gray
Write-Host "  3. Display call type and duration" -ForegroundColor Gray
Write-Host ""

# Build the bridge
$buildSuccess = Build-Bridge

# Start listening
Start-Listening

Write-Host "`nTest completed. Press any key to exit..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

