$Version = "v0.1.0"
$ReleaseDir = "DualSTT_$Version"
$ZipName = "DualSTT_$Version.zip"

Write-Host "Packaging DualSTT Release $Version..." -ForegroundColor Cyan

if (Test-Path $ReleaseDir) { Remove-Item -Recurse -Force $ReleaseDir }
if (Test-Path $ZipName) { Remove-Item -Force $ZipName }


New-Item -ItemType Directory -Path $ReleaseDir | Out-Null


$ExePath = "build\Release\capture_mic.exe"
if (-Not (Test-Path $ExePath)) {
    Write-Host "Error: capture_mic.exe not found! Please run 'cmake --build build --config Release' first." -ForegroundColor Red
    Remove-Item -Recurse -Force $ReleaseDir
    exit
}
Write-Host "Copying executable..."
Copy-Item -Path $ExePath -Destination "$ReleaseDir\DualSTT.exe"

# Copy Models Directory
if (-Not (Test-Path "models\sherpa")) {
    Write-Host "Warning: models/sherpa directory not found. Creating empty structure." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "$ReleaseDir\models\sherpa" -Force | Out-Null
} else {
    Write-Host "Copying AI models..."
    Copy-Item -Path "models" -Destination "$ReleaseDir\models" -Recurse
}

# Generate config.txt dynamically
Write-Host "Generating config.txt..."
$ConfigContent = @"
encoder=models/sherpa/encoder-epoch-99-avg-1-chunk-16-left-128.int8.onnx
decoder=models/sherpa/decoder-epoch-99-avg-1-chunk-16-left-128.int8.onnx
joiner=models/sherpa/joiner-epoch-99-avg-1-chunk-16-left-128.int8.onnx
tokens=models/sherpa/tokens.txt
num_threads=2
"@
Set-Content -Path "$ReleaseDir\config.txt" -Value $ConfigContent

# Generate run.bat
Write-Host "Generating run.bat..."
$BatContent = @"
@echo off
title DualSTT Daemon
echo ===================================================
echo   DualSTT Background Daemon is starting...
echo   Do not close this window while using the AI.
echo ===================================================
echo.
DualSTT.exe
echo.
echo DualSTT has stopped.
pause
"@
Set-Content -Path "$ReleaseDir\run.bat" -Value $BatContent

# Zip it up
Write-Host "Compressing to $ZipName..." -ForegroundColor Cyan
Compress-Archive -Path "$ReleaseDir\*" -DestinationPath $ZipName

# Cleanup staging dir
Remove-Item -Recurse -Force $ReleaseDir

Write-Host "Success! Release package created at $ZipName" -ForegroundColor Green