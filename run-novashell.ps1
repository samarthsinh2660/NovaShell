# NovaShell Launcher
# Ensures MSYS2 is in PATH for SQLite3 support

Write-Host "NovaShell Launcher" -ForegroundColor Cyan
Write-Host "==================" -ForegroundColor Cyan

# Add MSYS2 to PATH if available
$msys2Path = "C:\msys64\mingw64\bin"
if (Test-Path $msys2Path) {
    $env:Path = "$msys2Path;$env:Path"
    Write-Host "MSYS2 libraries added to PATH" -ForegroundColor Green
} else {
    Write-Host "MSYS2 not found - SQLite3 features may not work" -ForegroundColor Yellow
}

# Launch NovaShell
Write-Host "Starting NovaShell..." -ForegroundColor Green
& ".\build\bin\customos-shell.exe"
