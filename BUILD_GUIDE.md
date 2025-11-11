# Build Guide

This comprehensive guide covers building CustomOS Shell from source on all supported platforms.

## Quick Start

```bash
# Clone and build
git clone https://github.com/yourusername/customos-shell.git
cd customos-shell
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Run
./bin/customos-shell
```

## Detailed Platform Instructions

### Windows

#### Option 1: Visual Studio (Recommended)

**Prerequisites**:
1. Install Visual Studio 2019 or later with C++ development tools
2. Install CMake (via Visual Studio Installer or from cmake.org)
3. Install OpenSSL for Windows:
   - Download from: https://slproweb.com/products/Win32OpenSSL.html
   - Install Win64 OpenSSL v3.x.x (not the Light version)
   - Default location: `C:\Program Files\OpenSSL-Win64`

**Build Steps**:
```powershell
# Open Developer Command Prompt for VS
# Navigate to project directory
cd C:\path\to\customos-shell

# Create build directory
mkdir build
cd build

# Configure
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release

# Run
.\bin\Release\customos-shell.exe
```

#### Option 2: MinGW-w64

**Prerequisites**:
1. Install MinGW-w64 from https://www.mingw-w64.org/
2. Install CMake
3. Install OpenSSL (use MinGW package manager or build from source)

**Build Steps**:
```powershell
# Set up MinGW environment
set PATH=C:\mingw64\bin;%PATH%

mkdir build
cd build

# Configure
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Run
.\bin\customos-shell.exe
```

**Common Windows Issues**:

*OpenSSL Not Found*:
```powershell
cmake .. -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"
```

*DLL Missing at Runtime*:
- Copy OpenSSL DLLs to executable directory
- Or add OpenSSL bin directory to PATH

### Linux

#### Ubuntu/Debian

**Install Dependencies**:
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

**Build**:
```bash
git clone https://github.com/yourusername/customos-shell.git
cd customos-shell

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)

# Install (optional)
sudo cmake --install .
```

#### Fedora/RHEL/CentOS

**Install Dependencies**:
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    openssl-devel \
    libpcap-devel
```

**Build**:
```bash
# Same as Ubuntu
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

#### Arch Linux

**Install Dependencies**:
```bash
sudo pacman -S base-devel cmake git openssl libpcap
```

**Build**:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### macOS

**Install Dependencies**:
```bash
# Install Homebrew if needed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake openssl libpcap
```

**Build**:
```bash
mkdir build && cd build

# Point to Homebrew OpenSSL
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)

cmake --build . -j$(sysctl -n hw.ncpu)

# Run
./bin/customos-shell
```

## Build Options

### CMake Configuration Options

```bash
# Disable network features
cmake .. -DENABLE_NETWORK=OFF

# Disable tests
cmake .. -DBUILD_TESTS=OFF

# Disable sample plugins
cmake .. -DBUILD_PLUGINS=OFF

# Custom install prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/customos

# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release with debug info
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

### Build Types

- **Debug**: No optimization, full debug symbols
  ```bash
  cmake .. -DCMAKE_BUILD_TYPE=Debug
  ```

- **Release**: Full optimization, no debug symbols
  ```bash
  cmake .. -DCMAKE_BUILD_TYPE=Release
  ```

- **RelWithDebInfo**: Optimization + debug symbols
  ```bash
  cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
  ```

- **MinSizeRel**: Optimize for size
  ```bash
  cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
  ```

## Advanced Build Configurations

### Static Linking

**Linux**:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++"
```

**Note**: Fully static linking with OpenSSL can be complex.

### Cross-Compilation

**Example: Windows from Linux (MinGW)**:
```bash
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64.cmake \
    -DCMAKE_BUILD_TYPE=Release
```

### Custom Compiler

```bash
# Use Clang instead of GCC
cmake .. \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Release
```

### Sanitizers (Debug Builds)

**Address Sanitizer**:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer"
```

**Thread Sanitizer**:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=thread"
```

## Building Plugins

### Sample Plugin Build

```bash
cd plugins/example_plugin
mkdir build && cd build
cmake ..
cmake --build .
```

### Installing Plugins

```bash
# Copy to plugin directory
cp libmyplugin.so ~/.customos/plugins/

# Load in shell
customos> load-plugin ~/.customos/plugins/libmyplugin.so
```

## Testing

### Running Tests

```bash
cd build
ctest --output-on-failure

# Verbose output
ctest -V

# Run specific test
ctest -R CommandRegistry
```

### Test Coverage (Linux)

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="--coverage"

cmake --build .
ctest

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## Packaging

### Creating Distribution Package

**Linux (DEB)**:
```bash
cd build
cpack -G DEB
```

**Linux (RPM)**:
```bash
cd build
cpack -G RPM
```

**Windows (NSIS)**:
```bash
cd build
cpack -G NSIS
```

**macOS (Bundle)**:
```bash
cd build
cpack -G Bundle
```

### Manual Package

```bash
cd build
cmake --build . --config Release
cmake --install . --prefix package

# Create archive
tar czf customos-shell-1.0.0-linux-x64.tar.gz package/
```

## Development Build

For active development:

```bash
mkdir build-dev && cd build-dev

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Incremental build
cmake --build . -j$(nproc)

# Run tests
ctest
```

## IDE Integration

### Visual Studio Code

```json
// .vscode/settings.json
{
    "cmake.configureSettings": {
        "CMAKE_BUILD_TYPE": "Debug"
    },
    "cmake.buildDirectory": "${workspaceFolder}/build"
}
```

### CLion

Open project root directory. CLion auto-detects CMake projects.

### Visual Studio

Use "Open Folder" with CMakeLists.txt project.

## Continuous Integration

### GitHub Actions Example

```yaml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]

    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        # OS-specific dependency installation
    
    - name: Build
      run: |
        mkdir build && cd build
        cmake ..
        cmake --build . --config Release
    
    - name: Test
      run: |
        cd build
        ctest --output-on-failure
```

## Troubleshooting

### CMake Can't Find Dependencies

```bash
# Specify manually
cmake .. \
    -DOPENSSL_ROOT_DIR=/path/to/openssl \
    -DPCAP_LIBRARY=/path/to/libpcap
```

### Compilation Errors

1. **Check compiler version**: Requires C++17 support
   ```bash
   g++ --version  # Should be 7.0+
   clang++ --version  # Should be 5.0+
   ```

2. **Clear CMake cache**:
   ```bash
   rm -rf build
   mkdir build && cd build
   cmake ..
   ```

### Linking Errors

1. **Undefined references to OpenSSL**:
   - Ensure OpenSSL development libraries are installed
   - Check library paths

2. **Undefined references to pthread**:
   - CMake should handle this, but you can manually add:
   ```bash
   cmake .. -DCMAKE_CXX_FLAGS="-pthread"
   ```

### Runtime Issues

1. **Shared library not found**:
   ```bash
   # Linux
   export LD_LIBRARY_PATH=/path/to/libs:$LD_LIBRARY_PATH
   
   # macOS
   export DYLD_LIBRARY_PATH=/path/to/libs:$DYLD_LIBRARY_PATH
   ```

2. **Permission denied for network features**:
   ```bash
   # Linux: Grant capabilities
   sudo setcap cap_net_raw,cap_net_admin=eip ./bin/customos-shell
   
   # Or run as root (not recommended)
   sudo ./bin/customos-shell
   ```

## Performance Optimization

### Profile-Guided Optimization (PGO)

1. Build instrumented binary:
   ```bash
   cmake .. -DCMAKE_CXX_FLAGS="-fprofile-generate"
   cmake --build .
   ```

2. Run typical workload to generate profile data

3. Rebuild with profile:
   ```bash
   cmake .. -DCMAKE_CXX_FLAGS="-fprofile-use"
   cmake --build .
   ```

### Link-Time Optimization (LTO)

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

## Clean Build

```bash
# Remove build artifacts
rm -rf build

# Or clean within build directory
cd build
cmake --build . --target clean
```

## Getting Help

- **Build Issues**: Check [GitHub Issues](https://github.com/yourusername/customos-shell/issues)
- **Documentation**: See [README.md](README.md) and [INSTALL.md](INSTALL.md)
- **Community**: Join our Discord/discussions

---

Happy Building! 🛠️
