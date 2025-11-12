# NovaShell Setup & Installation Guide

## Quick Start (Windows)

### Automated Setup (Recommended)

Run the automated dependency installer:

`powershell
# Run as Administrator for best results
.\setup-dependencies.ps1
`

This will automatically install:
- **Chocolatey** - Package manager for Windows
- **MinGW-w64** - GCC 11+ compiler with C++17 support
- **OpenSSL** - Cryptography library
- **SQLite3** - Database engine
- **CMake** - Build system

### Build NovaShell

After dependencies are installed:

`powershell
# Build the project
.\build.ps1

# Or for release build
.\build.ps1 -Release

# Clean and rebuild
.\build.ps1 -Rebuild
`

### Run NovaShell

`powershell
.\build\bin\customos-shell.exe
`

---

## Prerequisites

### All Platforms
- **C++ Compiler**: Support for C++17 or later
- **CMake**: Version 3.15 or higher
- **OpenSSL**: For cryptographic functions
- **Git**: For cloning the repository

### Platform-Specific

#### Windows
- **Visual Studio 2019+** (recommended) or **MinGW-w64**
- **OpenSSL for Windows**: Download from https://slproweb.com/products/Win32OpenSSL.html
- **Optional**: WinPcap or Npcap for network features

#### Linux (Ubuntu/Debian)
`ash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libpcap-dev \
    pkg-config
`

#### Linux (Fedora/RHEL)
`ash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    openssl-devel \
    libpcap-devel
`

#### macOS
`ash
# Install Homebrew if not already installed
/bin/bash -c ""

# Install dependencies
brew install cmake openssl sqlite3
`

---

## Manual Setup

If you prefer to install dependencies manually:

### 1. Install Chocolatey

`powershell
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
`

### 2. Install Build Tools

`powershell
choco install mingw cmake -y
`

### 3. Install Libraries

`powershell
choco install openssl sqlite -y
`

### 4. Build Manually

`powershell
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make -j4
`

---

## Building from Source

### Step 1: Clone the Repository
`ash
git clone https://github.com/yourusername/novashell.git
cd novashell
`

### Step 2: Create Build Directory
`ash
mkdir build
cd build
`

### Step 3: Configure with CMake

#### Linux/macOS
`ash
cmake .. -DCMAKE_BUILD_TYPE=Release
`

#### Windows (Visual Studio)
`powershell
cmake .. -G "Visual Studio 16 2019" -A x64
`

#### Windows (MinGW)
`powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
`

### Step 4: Build
`ash
# Linux/macOS
cmake --build . --config Release

# Windows
cmake --build . --config Release
`

### Step 5: Install (Optional)

#### Linux/macOS
`ash
sudo cmake --install .
`

#### Windows (Run as Administrator)
`powershell
cmake --install .
`

---

## Build Options

You can customize the build with these CMake options:

`ash
# Disable network packet analyzer features
cmake .. -DENABLE_NETWORK=OFF

# Disable building tests
cmake .. -DBUILD_TESTS=OFF

# Disable building sample plugins
cmake .. -DBUILD_PLUGINS=OFF

# Specify custom OpenSSL location
cmake .. -DOPENSSL_ROOT_DIR=/path/to/openssl
`

---

## Compiler Requirements

### Minimum Requirements
- **GCC 8.0+** for full C++17 support
- **CMake 3.10+**
- **OpenSSL 1.1.0+**
- **SQLite3 3.0+**

### Current Compatibility

The project includes compatibility layers for:
- **GCC 6.3+** - Limited C++17 support with fallback implementations
- **std::optional** - Custom implementation for older compilers
- **std::mutex** - Thread safety features

### Recommended Setup
- **GCC 11+** - Full C++17 support, best performance
- **CMake 3.20+** - Latest features
- **OpenSSL 3.0+** - Modern cryptography

---

## Troubleshooting

### "CMake not found"
`powershell
choco install cmake -y
refreshenv
`

### "g++ not found"
`powershell
choco install mingw -y
refreshenv
`

### "OpenSSL not found"
`powershell
choco install openssl -y
# Or specify path manually:
cmake .. -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"
`

### "OpenSSL Not Found (macOS)"
`ash
cmake .. -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
`

### "Permission Denied (Linux/macOS)"
Some features (like network packet capture) require root privileges:
`ash
sudo setcap cap_net_raw,cap_net_admin=eip ./bin/customos-shell
`

### "std::optional not found"
Your compiler is too old. Options:
1. **Upgrade GCC**: .\setup-dependencies.ps1 -Force
2. **Use compatibility layer**: Already included in the project

### Build fails with mutex errors
GCC 6.3.0 has limited threading support. Upgrade to GCC 11+:
`powershell
.\setup-dependencies.ps1 -SkipOpenSSL -SkipSQLite
`

### Permission denied errors
Run PowerShell as Administrator:
`powershell
Start-Process powershell -Verb RunAs
`

### MinGW Linking Issues (Windows)
Make sure you're using MinGW-w64 and not the older MinGW:
`powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND"
`

---

## Verification

After installation, verify your setup:

`powershell
# Check GCC version
g++ --version

# Check CMake version
cmake --version

# Check OpenSSL
openssl version

# Check SQLite
sqlite3 --version
`

Expected output:
`
g++ (MinGW-w64) 11.x.x or higher
cmake version 3.20.x or higher
OpenSSL 1.1.x or 3.x.x
SQLite 3.x.x
`

After building, verify the installation:

`ash
# From build directory
./bin/customos-shell --version

# If installed
customos-shell --version
`

You should see:
`
NovaShell v1.0.0
Build date: [date] [time]
`

---

## Running NovaShell

### Interactive Mode
`ash
./bin/customos-shell
`

### Execute Single Command
`ash
./bin/customos-shell help
`

### First Run Setup
On first run, the shell will initialize automatically.

---

## Advanced Configuration

### Custom Installation Prefix
`ash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/novashell
cmake --build . --config Release
sudo cmake --install .
`

### Static Linking
For a fully static binary:
`ash
cmake .. -DCMAKE_EXE_LINKER_FLAGS="-static"
`

---

## Next Steps

1. ✅ Install dependencies: .\setup-dependencies.ps1
2. ✅ Build project: .\build.ps1
3. ✅ Run NovaShell: .\build\bin\customos-shell.exe
4. 📖 Read documentation: README.md
5. 🚀 Explore features: Type help in the shell

- Read [README.md](README.md) for usage instructions
- Check [FEATURES.md](FEATURES.md) for feature documentation
- See [CONTRIBUTING.md](CONTRIBUTING.md) to contribute

---

## Support

For issues or questions:
- Check the [Troubleshooting](#troubleshooting) section above
- Review build logs in uild/ directory
- Run with verbose output: .\build.ps1 -Verbose
- Search [GitHub Issues](https://github.com/yourusername/novashell/issues)
- Create a new issue with build logs and system information

---

**Note**: First-time setup may take 5-10 minutes depending on your internet connection.
