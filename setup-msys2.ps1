# MSYS2 Terminal Setup for SQLite3
Write-Host "Setting up MSYS2 environment..." -ForegroundColor Green

# MSYS2 should be installed at:
$msys2Path = "C:\msys64"

if (Test-Path $msys2Path) {
    Write-Host "MSYS2 found at: $msys2Path" -ForegroundColor Green

    # Instructions for the user
    Write-Host ""
    Write-Host "To install SQLite3 development libraries:" -ForegroundColor Yellow
    Write-Host "1. Open MSYS2 MSYS terminal (not MinGW)" -ForegroundColor White
    Write-Host "2. Run: pacman -Syu" -ForegroundColor White
    Write-Host "3. Run: pacman -S mingw-w64-x86_64-sqlite3" -ForegroundColor White
    Write-Host "4. Close MSYS2 terminal" -ForegroundColor White
    Write-Host "5. Run: .\build.ps1" -ForegroundColor White
    Write-Host ""

    Write-Host "MSYS2 Terminal Location:" -ForegroundColor Cyan
    Write-Host "$msys2Path\msys2.exe" -ForegroundColor White
    Write-Host ""

    # Check if we can add MSYS2 to PATH
    $addToPath = Read-Host "Add MSYS2 to PATH for current session? (Y/N)"
    if ($addToPath -eq "Y" -or $addToPath -eq "y") {
        $env:Path = "$msys2Path\usr\bin;$env:Path"
        Write-Host "MSYS2 added to PATH for this session" -ForegroundColor Green
    }

} else {
    Write-Host "MSYS2 not found at expected location: $msys2Path" -ForegroundColor Red
    Write-Host "Please check your MSYS2 installation" -ForegroundColor Red
}
