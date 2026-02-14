# Test Call Listener Script
# Simple menu to enable device and listen for incoming calls

$ErrorActionPreference = "Continue"

# Get script directory and resolve bridge path
if ($PSScriptRoot) {
    $scriptDir = $PSScriptRoot
} else {
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
}
$projectRoot = Split-Path -Parent $scriptDir
$bridgePath = Join-Path $projectRoot "CppBridge\bin\x64\Release\CppBridge.exe"

# Convert to absolute path
if (Test-Path $bridgePath) {
    $bridgePath = (Resolve-Path $bridgePath).Path
} else {
    # Try from current directory
    $altPath = Join-Path (Get-Location) "CppBridge\bin\x64\Release\CppBridge.exe"
    if (Test-Path $altPath) {
        $bridgePath = (Resolve-Path $altPath).Path
    }
}

# Colors
function Write-Header {
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host "  Call Listener Test Tool" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
}

function Write-Menu {
    Write-Host "`nMENU:" -ForegroundColor Yellow
    Write-Host "  1. Enable & Listen for Calls" -ForegroundColor White
    Write-Host "  2. Exit" -ForegroundColor White
    Write-Host ""
}

function Show-CallInfo {
    param($event)
    
    $timestamp = Get-Date -Format "HH:mm:ss"
    
    if ($event.status -eq "Ring") {
        if ($event.callerId -and $event.callerId -ne "") {
            Write-Host "[$timestamp] 📞 CALL COME - Number: $($event.callerId)" -ForegroundColor Green
        } else {
            Write-Host "[$timestamp] 📞 CALL COME - (No Caller ID)" -ForegroundColor Green
        }
    }
    elseif ($event.status -eq "Free") {
        Write-Host "[$timestamp] ✓ Call ended (Line $($event.line))" -ForegroundColor Gray
    }
    elseif ($event.status -eq "Missed") {
        if ($event.callerId -and $event.callerId -ne "") {
            Write-Host "[$timestamp] ❌ MISSED CALL - Number: $($event.callerId)" -ForegroundColor Red
        } else {
            Write-Host "[$timestamp] ❌ MISSED CALL - (No Caller ID)" -ForegroundColor Red
        }
    }
    elseif ($event.duration) {
        $seconds = [math]::Round($event.duration / 1000, 1)
        Write-Host "[$timestamp] ⏱️  Call Duration: $seconds seconds" -ForegroundColor Cyan
    }
    elseif ($event.type -eq "event" -and $event.meaning) {
        Write-Host "[$timestamp] ℹ️  $($event.meaning)" -ForegroundColor Gray
    }
}

function Start-Listening {
    Write-Host "`nStarting bridge..." -ForegroundColor Yellow
    
    # Check if bridge exists
    if (-not $bridgePath -or -not (Test-Path $bridgePath)) {
        Write-Host "ERROR: Bridge not found!" -ForegroundColor Red
        Write-Host "Tried path: $bridgePath" -ForegroundColor Yellow
        Write-Host "`nPlease build the C++ bridge first:" -ForegroundColor Yellow
        Write-Host "  1. Open CppBridge\CppBridge.sln in Visual Studio" -ForegroundColor White
        Write-Host "  2. Build for Release x64" -ForegroundColor White
        Write-Host "  3. Run this script again" -ForegroundColor White
        Write-Host "`nPress any key to return to menu..." -ForegroundColor Gray
        $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
        return
    }
    
    Write-Host "Bridge path: $bridgePath" -ForegroundColor Gray
    Write-Host "`n========================================" -ForegroundColor Cyan
    Write-Host "  Listening for Calls..." -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Device will be enabled automatically." -ForegroundColor Green
    Write-Host "Press Ctrl+C to stop listening`n" -ForegroundColor Yellow
    
    # Start the bridge process with redirected output
    # Note: We redirect stdin but don't write to it - this keeps it "open" so the bridge doesn't exit
    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = $bridgePath
    $psi.UseShellExecute = $false
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $psi.RedirectStandardInput = $true  # Keep stdin open so bridge doesn't exit
    $psi.CreateNoWindow = $true
    
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $psi
    
    # Handle Ctrl+C to gracefully stop
    $null = Register-EngineEvent PowerShell.Exiting -Action {
        if (-not $process.HasExited) {
            try {
                $process.StandardInput.Close()
                $process.Kill()
            } catch {
                # Ignore
            }
        }
    }
    
    try {
        # Start the process
        $process.Start() | Out-Null
        
        # Keep stdin open (don't close it, or bridge will exit)
        # We'll close it when user presses Ctrl+C
        
        Write-Host "Bridge process started (PID: $($process.Id))" -ForegroundColor Gray
        Write-Host "Waiting for bridge to initialize...`n" -ForegroundColor Gray
        
        # Read stderr (status messages) in a separate runspace
        $stderrOutput = New-Object System.Collections.ArrayList
        $stderrScriptBlock = {
            param($reader, $outputList)
            try {
                while (-not $reader.EndOfStream) {
                    $line = $reader.ReadLine()
                    if ($line -and $line.Trim() -ne "") {
                        [void]$outputList.Add($line)
                        if ($line -match "started|enabled|Listening") {
                            Write-Host "✓ $line" -ForegroundColor Green
                        } elseif ($line -match "Failed|Error") {
                            Write-Host "✗ $line" -ForegroundColor Red
                        } else {
                            Write-Host $line -ForegroundColor Gray
                        }
                    }
                }
            } catch {
                # End of stream or error
            }
        }
        
        # Start reading stderr in background
        $stderrJob = Start-Job -ScriptBlock $stderrScriptBlock -ArgumentList $process.StandardError, $stderrOutput
        
        # Give process time to initialize
        Start-Sleep -Milliseconds 500
        
        # Check if process is still running
        if ($process.HasExited) {
            $exitCode = $process.ExitCode
            Write-Host "`nBridge process exited immediately with code: $exitCode" -ForegroundColor Red
            
            # Check for specific DLL dependency error (0xC0000135 = -1073741515)
            if ($exitCode -eq -1073741515 -or $exitCode -eq 0xC0000135) {
                Write-Host "`n========================================" -ForegroundColor Red
                Write-Host "  DLL DEPENDENCY ERROR DETECTED!" -ForegroundColor Red
                Write-Host "========================================" -ForegroundColor Red
                Write-Host "`nThe bridge requires Visual C++ 2005 Runtime DLLs." -ForegroundColor Yellow
                Write-Host "`nSOLUTION: Install Visual C++ Redistributable" -ForegroundColor Cyan
                Write-Host ""
                Write-Host "Option 1 (Recommended):" -ForegroundColor White
                Write-Host "  Download and install:" -ForegroundColor Gray
                Write-Host "  https://aka.ms/vs/17/release/vc_redist.x64.exe" -ForegroundColor Green
                Write-Host ""
                Write-Host "Option 2 (Specific version):" -ForegroundColor White
                Write-Host "  Download Visual C++ 2005 Redistributable (x64):" -ForegroundColor Gray
                Write-Host "  https://www.microsoft.com/en-us/download/details.aspx?id=18471" -ForegroundColor Green
                Write-Host ""
                Write-Host "After installing, restart this script." -ForegroundColor Yellow
                Write-Host "`n========================================`n" -ForegroundColor Red
            }
            
            # Get any error output
            $errorOutput = $process.StandardError.ReadToEnd()
            if ($errorOutput) {
                Write-Host "Error output:" -ForegroundColor Yellow
                Write-Host $errorOutput -ForegroundColor Red
            }
            
            $stdoutOutput = $process.StandardOutput.ReadToEnd()
            if ($stdoutOutput) {
                Write-Host "Standard output:" -ForegroundColor Yellow
                Write-Host $stdoutOutput -ForegroundColor Gray
            }
            
            Remove-Job $stderrJob -Force -ErrorAction SilentlyContinue | Out-Null
            return
        }
        
        Write-Host "✓ Bridge is running and listening for calls...`n" -ForegroundColor Green
        
        # Use event-based async reading for stdout (non-blocking)
        $stdoutLines = New-Object System.Collections.ArrayList
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
        
        # Process exited, get remaining output
        if ($buffer.Trim() -ne "") {
            try {
                $json = $buffer | ConvertFrom-Json -ErrorAction Stop
                if ($json) {
                    Show-CallInfo -event $json
                }
            } catch {
                # Not JSON
            }
        }
        
        # Wait for stderr job to finish
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
                Write-Host "Stopping bridge..." -ForegroundColor Yellow
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

# Main loop
Clear-Host
Write-Header

while ($true) {
    Write-Menu
    $choice = Read-Host "Enter your choice (1 or 2)"
    
    switch ($choice) {
        "1" {
            Clear-Host
            Write-Header
            Start-Listening
            Write-Host "`nStopped listening. Returning to menu..." -ForegroundColor Yellow
            Start-Sleep -Seconds 2
            Clear-Host
            Write-Header
        }
        "2" {
            Write-Host "`nExiting... Goodbye!" -ForegroundColor Yellow
            exit 0
        }
        default {
            Write-Host "`nInvalid choice. Please enter 1 or 2." -ForegroundColor Red
            Start-Sleep -Seconds 1
        }
    }
}
