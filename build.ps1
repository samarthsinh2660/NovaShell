# NovaShell Build Script for Windows
# Automated build with dependency checking

param(
    [switch]$Clean,
    [switch]$Rebuild,
    [switch]$Release,
    [switch]$Verbose
)

$ErrorActionPreference = "Stop"

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "   NovaShell Build Script" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""

# Check for required tools
Write-Host "Checking build dependencies..." -ForegroundColor Green

$missingDeps = @()

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    $missingDeps += "CMake"
}

if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    $missingDeps += "GCC (MinGW)"
}

if ($missingDeps.Count -gt 0) {
    Write-Host ""
    Write-Host "ERROR: Missing required dependencies:" -ForegroundColor Red
    foreach ($dep in $missingDeps) {
        Write-Host "  - $dep" -ForegroundColor Red
    }
    Write-Host ""
    Write-Host "Please run: .\setup-dependencies.ps1" -ForegroundColor Yellow
    exit 1
}

# Check GCC version
$gccVersion = & g++ --version 2>&1 | Select-Object -First 1
if ($gccVersion -match "(\d+)\.(\d+)\.(\d+)") {
    $majorVersion = [int]$matches[1]
    Write-Host "  GCC Version: $majorVersion.$($matches[2]).$($matches[3])" -ForegroundColor White
    
    if ($majorVersion -lt 8) {
        Write-Host ""
        Write-Host "WARNING: GCC version is too old (< 8.0)" -ForegroundColor Yellow
        Write-Host "C++17 features may not be fully supported." -ForegroundColor Yellow
        Write-Host "Consider upgrading: .\setup-dependencies.ps1 -Force" -ForegroundColor Yellow
        Write-Host ""
    }
}

# Build directory
$buildDir = "build"
if ($Rebuild -or $Clean) {
    if (Test-Path $buildDir) {
        Write-Host "Cleaning build directory..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $buildDir
    }
}

if (-not (Test-Path $buildDir)) {
    Write-Host "Creating build directory..." -ForegroundColor Green
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Build type
$buildType = if ($Release) { "Release" } else { "Debug" }
Write-Host "Build Type: $buildType" -ForegroundColor White
Write-Host ""

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Green
Push-Location $buildDir

try {
    $cmakeArgs = @(
        "..",
        "-G", "MinGW Makefiles",
        "-DCMAKE_BUILD_TYPE=$buildType",
        "-DCMAKE_CXX_STANDARD=17"
    )
    
    if ($Verbose) {
        $cmakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
    }
    
    & cmake @cmakeArgs
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
    
    Write-Host ""
    Write-Host "Building NovaShell..." -ForegroundColor Green
    
    $makeArgs = @()
    if ($Verbose) {
        $makeArgs += "VERBOSE=1"
    }
    
    & mingw32-make @makeArgs -j4
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
    
    Write-Host ""
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host "   Build Successful!" -ForegroundColor Green
    Write-Host "================================================" -ForegroundColor Cyan
    Write-Host ""
    
    if (Test-Path "bin\customos-shell.exe") {
        $exePath = Resolve-Path "bin\customos-shell.exe"
        Write-Host "Executable: $exePath" -ForegroundColor White
        Write-Host ""
        Write-Host "Run with: .\build\bin\customos-shell.exe" -ForegroundColor Yellow
    } else {
        Write-Host "WARNING: Executable not found in expected location" -ForegroundColor Yellow
    }
    
} catch {
    Write-Host ""
    Write-Host "================================================" -ForegroundColor Red
    Write-Host "   Build Failed!" -ForegroundColor Red
    Write-Host "================================================" -ForegroundColor Red
    Write-Host ""
    Write-Host "Error: $_" -ForegroundColor Red
    Write-Host ""
    Write-Host "Try:" -ForegroundColor Yellow
    Write-Host "  1. .\build.ps1 -Rebuild -Verbose" -ForegroundColor White
    Write-Host "  2. Check if all dependencies are installed" -ForegroundColor White
    Write-Host "  3. Run: .\setup-dependencies.ps1 -Force" -ForegroundColor White
    exit 1
} finally {
    Pop-Location
}
