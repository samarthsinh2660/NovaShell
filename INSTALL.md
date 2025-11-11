# Installation Guide - CustomOS Shell

This guide will help you compile and install CustomOS Shell on various platforms.

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
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libpcap-dev \
    pkg-config
```

#### Linux (Fedora/RHEL)
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    openssl-devel \
    libpcap-devel
```

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake openssl libpcap
```

## Building from Source

### Step 1: Clone the Repository
```bash
git clone https://github.com/yourusername/customos-shell.git
cd customos-shell
```

### Step 2: Create Build Directory
```bash
mkdir build
cd build
```

### Step 3: Configure with CMake

#### Linux/macOS
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### Windows (Visual Studio)
```powershell
cmake .. -G "Visual Studio 16 2019" -A x64
```

#### Windows (MinGW)
```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
```

### Step 4: Build
```bash
# Linux/macOS
cmake --build . --config Release

# Windows
cmake --build . --config Release
```

### Step 5: Install (Optional)

#### Linux/macOS
```bash
sudo cmake --install .
```

#### Windows (Run as Administrator)
```powershell
cmake --install .
```

## Build Options

You can customize the build with these CMake options:

```bash
# Disable network packet analyzer features
cmake .. -DENABLE_NETWORK=OFF

# Disable building tests
cmake .. -DBUILD_TESTS=OFF

# Disable building sample plugins
cmake .. -DBUILD_PLUGINS=OFF

# Specify custom OpenSSL location
cmake .. -DOPENSSL_ROOT_DIR=/path/to/openssl
```

## Troubleshooting

### OpenSSL Not Found (Windows)

If CMake cannot find OpenSSL:
```powershell
cmake .. -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"
```

### OpenSSL Not Found (macOS)

```bash
cmake .. -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

### Permission Denied (Linux/macOS)

Some features (like network packet capture) require root privileges:
```bash
sudo setcap cap_net_raw,cap_net_admin=eip ./bin/customos-shell
```

### MinGW Linking Issues (Windows)

Make sure you're using MinGW-w64 and not the older MinGW:
```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND"
```

## Verification

After building, verify the installation:

```bash
# From build directory
./bin/customos-shell --version

# If installed
customos-shell --version
```

You should see:
```
CustomOS Shell v1.0.0
Build date: [date] [time]
```

## Running the Shell

### Interactive Mode
```bash
./bin/customos-shell
```

### Execute Single Command
```bash
./bin/customos-shell help
```

## First Run Setup

On first run, create an admin user:

```bash
customos> adduser admin
Enter password: ****
User 'admin' created successfully.

customos> login admin
Password: ****
Logged in as admin
```

## Uninstallation

### If Installed via CMake
```bash
# Linux/macOS
sudo rm /usr/local/bin/customos-shell
sudo rm -rf /usr/local/include/customos

# Windows
del "C:\Program Files\customos-shell\bin\customos-shell.exe"
```

### Manual Cleanup
Simply delete the build directory and cloned repository.

## Advanced Configuration

### Custom Installation Prefix

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/customos
cmake --build . --config Release
sudo cmake --install .
```

### Static Linking

For a fully static binary:
```bash
cmake .. -DCMAKE_EXE_LINKER_FLAGS="-static"
```

## Next Steps

- Read [README.md](README.md) for usage instructions
- Check [FEATURES.md](FEATURES.md) for feature documentation
- See [CONTRIBUTING.md](CONTRIBUTING.md) to contribute

## Support

If you encounter issues:
1. Check the [Troubleshooting](#troubleshooting) section
2. Search [GitHub Issues](https://github.com/yourusername/customos-shell/issues)
3. Create a new issue with build logs and system information
