# NovaShell Dependency Setup Script for Windows
# This script automatically installs all required dependencies

param(
    [switch]$SkipOpenSSL,
    [switch]$SkipSQLite,
    [switch]$SkipGCC,
    [switch]$Force
)

$ErrorActionPreference = "Stop"

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "   NovaShell Dependency Setup for Windows" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "WARNING: Not running as administrator. Some installations may fail." -ForegroundColor Yellow
    Write-Host "Consider running: Start-Process powershell -Verb RunAs -ArgumentList '-File setup-dependencies.ps1'" -ForegroundColor Yellow
    Write-Host ""
}

# Check for Chocolatey
Write-Host "[1/5] Checking for Chocolatey package manager..." -ForegroundColor Green
if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
    Write-Host "  Chocolatey not found. Installing..." -ForegroundColor Yellow
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    Write-Host "  Chocolatey installed successfully!" -ForegroundColor Green
} else {
    Write-Host "  Chocolatey already installed." -ForegroundColor Green
}

# Install/Update MinGW-w64 with GCC 11+
if (-not $SkipGCC) {
    Write-Host ""
    Write-Host "[2/5] Checking for MinGW-w64 (GCC 11+)..." -ForegroundColor Green
    
    $gccVersion = $null
    try {
        $gccOutput = & g++ --version 2>&1 | Select-Object -First 1
        if ($gccOutput -match "(\d+)\.(\d+)\.(\d+)") {
            $gccVersion = [int]$matches[1]
        }
    } catch {}
    
    if ($gccVersion -lt 11 -or $Force) {
        Write-Host "  Installing MinGW-w64 with GCC 11+..." -ForegroundColor Yellow
        choco install mingw -y --force
        Write-Host "  MinGW-w64 installed successfully!" -ForegroundColor Green
        Write-Host "  Please restart your terminal and re-run this script." -ForegroundColor Yellow
    } else {
        Write-Host "  GCC $gccVersion already installed." -ForegroundColor Green
    }
} else {
    Write-Host ""
    Write-Host "[2/5] Skipping GCC installation (--SkipGCC specified)" -ForegroundColor Yellow
}

# Install OpenSSL
if (-not $SkipOpenSSL) {
    Write-Host ""
    Write-Host "[3/5] Checking for OpenSSL..." -ForegroundColor Green
    
    $opensslPath = "C:\Program Files\OpenSSL-Win64"
    if (-not (Test-Path "$opensslPath\bin\openssl.exe") -or $Force) {
        Write-Host "  Installing OpenSSL..." -ForegroundColor Yellow
        choco install openssl -y --force
        Write-Host "  OpenSSL installed successfully!" -ForegroundColor Green
    } else {
        Write-Host "  OpenSSL already installed at $opensslPath" -ForegroundColor Green
    }
} else {
    Write-Host ""
    Write-Host "[3/5] Skipping OpenSSL installation (--SkipOpenSSL specified)" -ForegroundColor Yellow
}

# Install SQLite (prefer MSYS2 for complete dev libraries, fallback to Chocolatey)
if (-not $SkipSQLite) {
    Write-Host ""
    Write-Host "[4/5] Checking for SQLite..." -ForegroundColor Green

    # Check for MSYS2
    $msys2Path = "C:\msys64"

    if (Test-Path $msys2Path) {
        Write-Host "  MSYS2 found. Installing SQLite3 development libraries..." -ForegroundColor Yellow
        Write-Host "  This provides complete headers and libraries for development." -ForegroundColor Cyan

        # Create a temporary batch file for MSYS2 commands
        $tempBatch = "$env:TEMP\msys2_sqlite_install.bat"
        @"
@echo off
"$msys2Path\msys2.exe" -c "pacman -Syu --noconfirm && pacman -S mingw-w64-x86_64-sqlite3 --noconfirm"
"@ | Out-File -FilePath $tempBatch -Encoding ASCII

        try {
            & $tempBatch
            Write-Host "  SQLite3 development libraries installed via MSYS2!" -ForegroundColor Green
            Write-Host "  This provides complete CMake support and development headers." -ForegroundColor Cyan
        }
        catch {
            Write-Host "  MSYS2 installation failed. Falling back to Chocolatey..." -ForegroundColor Yellow
            choco install sqlite -y --force
            Write-Host "  SQLite installed via Chocolatey." -ForegroundColor Green
        }
        finally {
            Remove-Item $tempBatch -ErrorAction SilentlyContinue
        }
    }
    else {
        Write-Host "  MSYS2 not found. Installing SQLite via Chocolatey..." -ForegroundColor Yellow
        choco install sqlite -y --force
        Write-Host "  SQLite installed successfully!" -ForegroundColor Green
        Write-Host "  Note: For complete development support, consider installing MSYS2." -ForegroundColor Cyan
    }
} else {
    Write-Host ""
    Write-Host "[4/5] Skipping SQLite installation (--SkipSQLite specified)" -ForegroundColor Yellow
}

# Install CMake
Write-Host ""
Write-Host "[5/5] Checking for CMake..." -ForegroundColor Green

$cmakeVersion = $null
try {
    $cmakeOutput = & cmake --version 2>&1 | Select-Object -First 1
    if ($cmakeOutput -match "(\d+)\.(\d+)\.(\d+)") {
        $cmakeVersion = "$($matches[1]).$($matches[2]).$($matches[3])"
    }
} catch {}

if (-not $cmakeVersion -or $Force) {
    Write-Host "  Installing CMake..." -ForegroundColor Yellow
    choco install cmake -y --force --installargs 'ADD_CMAKE_TO_PATH=System'
    Write-Host "  CMake installed successfully!" -ForegroundColor Green
} else {
    Write-Host "  CMake $cmakeVersion already installed." -ForegroundColor Green
}

# Refresh environment variables
Write-Host ""
Write-Host "Refreshing environment variables..." -ForegroundColor Green
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")

# Summary
Write-Host ""
Write-Host "================================================" -ForegroundColor Cyan
Write-Host "   Installation Summary" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan

Write-Host ""
Write-Host "Installed Dependencies:" -ForegroundColor Green
Write-Host "  - Chocolatey Package Manager" -ForegroundColor White
if (-not $SkipGCC) { Write-Host "  - MinGW-w64 (GCC 11+)" -ForegroundColor White }
if (-not $SkipOpenSSL) { Write-Host "  - OpenSSL" -ForegroundColor White }
if (-not $SkipSQLite) {
    Write-Host "  - SQLite3 (Development Libraries)" -ForegroundColor White
    Write-Host "    * MSYS2 preferred for complete dev support" -ForegroundColor Gray
    Write-Host "    * Chocolatey fallback for basic functionality" -ForegroundColor Gray
}
Write-Host "  - CMake" -ForegroundColor White

Write-Host ""
Write-Host "Next Steps:" -ForegroundColor Yellow
Write-Host "  1. Close and reopen your terminal" -ForegroundColor White
Write-Host "  2. Run: .\build.ps1" -ForegroundColor White
Write-Host "  3. Run: .\bin\customos-shell.exe" -ForegroundColor White

Write-Host ""
Write-Host "Setup completed successfully!" -ForegroundColor Green
Write-Host ""
