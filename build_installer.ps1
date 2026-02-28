# build_installer.ps1 - Arcsky Control Installer Build Script
# Run this from PowerShell after building the Release exe in Qt Creator.
#
# Usage:
#   .\build_installer.ps1
#   .\build_installer.ps1 -SkipDeploy   # Skip windeployqt/GStreamer, just rebuild installer from existing C:\QGC_Deploy

param(
    [switch]$SkipDeploy
)

$ErrorActionPreference = "Stop"

# === CONFIGURATION ===
$BuildDir       = "C:\q\qgroundcontrol\build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release\Release"
$ExeName        = "ArcskyControl.exe"
$DeployDir      = "C:\QGC_Deploy"
$QtBinDir       = "C:\Qt\6.8.3\msvc2022_64\bin"
$QmlSourceDir   = "C:\q\qgroundcontrol\src"
$GStreamerDir    = "C:\gstreamer\1.0\msvc_x86_64"
$IconSource     = "C:\q\qgroundcontrol\resources\icons\qgroundcontrol.ico"
$IssFile        = "C:\q\qgroundcontrol\arcskycontrol.iss"
$InnoSetupExe   = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
# =====================

Write-Host ""
Write-Host "=== Arcsky Control Installer Build ===" -ForegroundColor Cyan
Write-Host ""

# Step 0: Verify the Release exe exists
$exePath = Join-Path $BuildDir $ExeName
if (-not (Test-Path $exePath)) {
    Write-Host "ERROR: Release exe not found at $exePath" -ForegroundColor Red
    Write-Host "Build the project in Release mode in Qt Creator first." -ForegroundColor Yellow
    exit 1
}
Write-Host "[OK] Found Release exe: $exePath" -ForegroundColor Green

if (-not $SkipDeploy) {
    # Step 1: Clean and recreate deploy folder
    Write-Host ""
    Write-Host "Step 1: Creating fresh deploy folder..." -ForegroundColor Yellow
    if (Test-Path $DeployDir) {
        Remove-Item -Recurse -Force $DeployDir
    }
    New-Item -ItemType Directory -Path $DeployDir | Out-Null
    Write-Host "[OK] Deploy folder ready: $DeployDir" -ForegroundColor Green

    # Step 2: Copy exe
    Write-Host ""
    Write-Host "Step 2: Copying exe..." -ForegroundColor Yellow
    Copy-Item $exePath $DeployDir
    Write-Host "[OK] Exe copied" -ForegroundColor Green

    # Step 3: Run windeployqt6
    Write-Host ""
    Write-Host "Step 3: Running windeployqt6 (this takes a minute)..." -ForegroundColor Yellow
    $windeployqt = Join-Path $QtBinDir "windeployqt6.exe"
    if (-not (Test-Path $windeployqt)) {
        Write-Host "ERROR: windeployqt6.exe not found at $windeployqt" -ForegroundColor Red
        exit 1
    }
    Push-Location $DeployDir
    $ErrorActionPreference = "Continue"
    & $windeployqt --release --qmldir $QmlSourceDir $ExeName 2>$null
    $ErrorActionPreference = "Stop"
    Pop-Location
    Write-Host "[OK] Qt dependencies deployed" -ForegroundColor Green

    # Step 4: Copy GStreamer
    Write-Host ""
    Write-Host "Step 4: Copying GStreamer dependencies..." -ForegroundColor Yellow
    if (-not (Test-Path $GStreamerDir)) {
        Write-Host "WARNING: GStreamer not found at $GStreamerDir - skipping" -ForegroundColor Yellow
    } else {
        Copy-Item "$GStreamerDir\bin\*.dll" $DeployDir -Force
        $gstPluginDir = Join-Path $DeployDir "lib\gstreamer-1.0"
        if (-not (Test-Path $gstPluginDir)) {
            New-Item -ItemType Directory -Path $gstPluginDir | Out-Null
        }
        Copy-Item "$GStreamerDir\lib\gstreamer-1.0\*.dll" $gstPluginDir -Force
        Write-Host "[OK] GStreamer DLLs and plugins copied" -ForegroundColor Green
    }

    # Step 5: Copy icon
    Write-Host ""
    Write-Host "Step 5: Copying icon..." -ForegroundColor Yellow
    Copy-Item $IconSource (Join-Path $DeployDir "arcskycontrol.ico") -Force
    Write-Host "[OK] Icon copied" -ForegroundColor Green

    $fileCount = (Get-ChildItem $DeployDir -File -Recurse).Count
    Write-Host ""
    Write-Host "[OK] Deploy folder ready with $fileCount files" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "Skipping deployment (using existing $DeployDir)" -ForegroundColor Yellow
    # Still update the exe even with -SkipDeploy
    Copy-Item $exePath $DeployDir -Force
    Write-Host "[OK] Exe updated in deploy folder" -ForegroundColor Green
}

# Step 6: Compile installer
Write-Host ""
Write-Host "Step 6: Compiling installer with Inno Setup..." -ForegroundColor Yellow
if (-not (Test-Path $InnoSetupExe)) {
    Write-Host "ERROR: Inno Setup not found at $InnoSetupExe" -ForegroundColor Red
    exit 1
}
& $InnoSetupExe $IssFile
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Installer compilation failed" -ForegroundColor Red
    exit 1
}

$installerPath = "C:\q\qgroundcontrol\ArcskyControl_Setup.exe"
$installerSize = [math]::Round((Get-Item $installerPath).Length / 1MB)
Write-Host ""
Write-Host "=== BUILD COMPLETE ===" -ForegroundColor Cyan
Write-Host "Installer: $installerPath ($installerSize MB)" -ForegroundColor Green
Write-Host ""
