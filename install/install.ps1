# NovaShell Installation Script for Windows
# Run with: iwr -useb https://novashell.dev/install.ps1 | iex

param(
    [string]$Version = "latest",
    [string]$InstallDir = "$env:LOCALAPPDATA\NovaShell"
)

$ErrorActionPreference = "Stop"

# Colors
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

function Print-Banner {
    Write-ColorOutput Cyan @"
╔═══════════════════════════════════════════════╗
║                                               ║
║            NovaShell Installer                ║
║                                               ║
║     The Ultimate Development Shell            ║
║     21+ Features | AI-Powered | Modern        ║
║                                               ║
╚═══════════════════════════════════════════════╝
"@
}

function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Get-Architecture {
    if ([Environment]::Is64BitOperatingSystem) {
        return "amd64"
    } else {
        return "386"
    }
}

function Download-NovaShell {
    param([string]$Version, [string]$Arch)
    
    Write-ColorOutput Yellow "Downloading NovaShell $Version..."
    
    $downloadUrl = if ($Version -eq "latest") {
        "https://github.com/yourusername/novashell/releases/latest/download/novashell-windows-$Arch.exe"
    } else {
        "https://github.com/yourusername/novashell/releases/download/$Version/novashell-windows-$Arch.exe"
    }
    
    $tempFile = "$env:TEMP\novashell-$([System.Guid]::NewGuid()).exe"
    
    try {
        Invoke-WebRequest -Uri $downloadUrl -OutFile $tempFile -UseBasicParsing
        Write-ColorOutput Green "✓ Downloaded successfully"
        return $tempFile
    } catch {
        Write-ColorOutput Red "Failed to download NovaShell: $_"
        exit 1
    }
}

function Install-Binary {
    param([string]$TempFile, [string]$InstallDir)
    
    Write-ColorOutput Yellow "Installing NovaShell to $InstallDir..."
    
    # Create install directory
    if (-not (Test-Path $InstallDir)) {
        New-Item -ItemType Directory -Path $InstallDir -Force | Out-Null
    }
    
    # Move binary
    $binaryPath = Join-Path $InstallDir "novashell.exe"
    Move-Item -Path $TempFile -Destination $binaryPath -Force
    
    Write-ColorOutput Green "✓ NovaShell installed successfully"
    return $binaryPath
}

function Add-ToPath {
    param([string]$InstallDir)
    
    $currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
    
    if ($currentPath -notlike "*$InstallDir*") {
        Write-ColorOutput Yellow "Adding NovaShell to PATH..."
        
        $newPath = "$currentPath;$InstallDir"
        [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
        
        # Update current session
        $env:Path = "$env:Path;$InstallDir"
        
        Write-ColorOutput Green "✓ Added to PATH"
    } else {
        Write-ColorOutput Yellow "NovaShell already in PATH"
    }
}

function Setup-Configuration {
    $configDir = "$env:USERPROFILE\.novashell"
    
    Write-ColorOutput Yellow "Setting up configuration..."
    
    if (-not (Test-Path $configDir)) {
        New-Item -ItemType Directory -Path $configDir -Force | Out-Null
    }
    
    $configFile = Join-Path $configDir "config.json"
    
    if (-not (Test-Path $configFile)) {
        $config = @{
            shell = @{
                prompt = "nova> "
                history_size = 1000
                theme = "dark"
            }
            ai = @{
                enabled = $false
                gemini_api_key = ""
            }
            database = @{
                internal_db = ".novashell\system.db"
            }
            logging = @{
                level = "info"
                file = ".novashell\nova.log"
            }
        } | ConvertTo-Json -Depth 10
        
        $config | Out-File -FilePath $configFile -Encoding UTF8
        Write-ColorOutput Green "✓ Created default configuration"
    } else {
        Write-ColorOutput Yellow "Configuration already exists"
    }
}

function Install-Dependencies {
    Write-ColorOutput Yellow "`nWould you like to install optional dependencies? (y/N)"
    $response = Read-Host
    
    if ($response -eq 'y' -or $response -eq 'Y') {
        Write-ColorOutput Yellow "Checking for Chocolatey..."
        
        if (-not (Get-Command choco -ErrorAction SilentlyContinue)) {
            Write-ColorOutput Yellow "Chocolatey not found. Install from: https://chocolatey.org/"
        } else {
            Write-ColorOutput Yellow "Installing dependencies via Chocolatey..."
            choco install git sqlite openssl -y
            Write-ColorOutput Green "✓ Dependencies installed"
        }
    }
}

function Print-Success {
    param([string]$InstallDir)
    
    Write-Output ""
    Write-ColorOutput Green @"
╔═══════════════════════════════════════════════╗
║                                               ║
║     NovaShell installed successfully! 🎉      ║
║                                               ║
╚═══════════════════════════════════════════════╝
"@
    Write-Output ""
    Write-ColorOutput Cyan "Installation Directory: $InstallDir"
    Write-Output ""
    Write-ColorOutput Yellow "Quick Start:"
    Write-Output "  1. Close and reopen your terminal"
    Write-Output "  2. Run: novashell"
    Write-Output "  3. Type: help"
    Write-Output ""
    Write-ColorOutput Yellow "Documentation:"
    Write-Output "  https://novashell.dev/docs"
    Write-Output ""
    Write-ColorOutput Yellow "Get Gemini API Key (FREE):"
    Write-Output "  https://makersuite.google.com/app/apikey"
    Write-Output ""
}

# Main installation flow
function Main {
    Print-Banner
    
    Write-Output ""
    Write-ColorOutput Yellow "Starting installation..."
    Write-Output ""
    
    $arch = Get-Architecture
    Write-ColorOutput Green "✓ Architecture detected: $arch"
    
    if (Test-Administrator) {
        Write-ColorOutput Yellow "Running as Administrator"
    }
    
    $tempFile = Download-NovaShell -Version $Version -Arch $arch
    $binaryPath = Install-Binary -TempFile $tempFile -InstallDir $InstallDir
    Add-ToPath -InstallDir $InstallDir
    Setup-Configuration
    Install-Dependencies
    
    Print-Success -InstallDir $InstallDir
}

# Run
Main
