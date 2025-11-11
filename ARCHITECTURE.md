# NovaShell - System Architecture

## Overview

**NovaShell** is built on a modular, layered architecture that separates concerns and enables extensibility. With **21 major features** integrated seamlessly, the architecture supports everything from AI-powered suggestions to database management, Git integration, and real-time system monitoring.

This document provides a comprehensive look at the system design, components, and their interactions.

## 🎯 Architecture Highlights

- **21 Feature Modules** - Fully integrated and working together
- **Modular Design** - Each feature is self-contained
- **Cross-Platform** - Windows, Linux, macOS support
- **Production-Ready** - Professional code quality
- **Extensible** - Plugin system for custom features

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    User Interface                        │
│                   (Shell / CLI)                          │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│              Command Processor Layer                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Parser     │  │   Registry   │  │   Executor   │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                  Core Services Layer                     │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐  │
│  │   Auth   │ │ Logging  │ │ Plugins  │ │ Scripts  │  │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘  │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│              Feature Modules Layer                       │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐  │
│  │   VFS    │ │  Vault   │ │ Network  │ │  Utils   │  │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘  │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│           Operating System / Platform Layer              │
│  (File I/O, Networking, Cryptography, Threading)        │
└─────────────────────────────────────────────────────────┘
```

## Directory Structure

```
customos-shell/
├── include/                  # Public header files
│   ├── core/                # Core shell components
│   │   ├── shell.h
│   │   ├── command_processor.h
│   │   └── command_registry.h
│   ├── auth/                # Authentication
│   │   └── authentication.h
│   ├── vfs/                 # Virtual File System
│   │   └── virtual_filesystem.h
│   ├── vault/               # Password Manager
│   │   └── password_manager.h
│   ├── network/             # Network Analyzer
│   │   └── packet_analyzer.h
│   ├── plugins/             # Plugin System
│   │   └── plugin_manager.h
│   ├── scripting/           # Script Engine
│   │   └── script_engine.h
│   └── logging/             # Logging System
│       └── logger.h
│
├── src/                     # Implementation files
│   ├── main.cpp             # Entry point
│   ├── core/                # Core implementations
│   ├── auth/
│   ├── vfs/
│   ├── vault/
│   ├── network/
│   ├── plugins/
│   ├── scripting/
│   ├── logging/
│   └── utils/
│
├── plugins/                 # Sample plugins
│   └── example_plugin/
│
├── tests/                   # Unit tests
│   └── unit/
│
├── docs/                    # Documentation
├── CMakeLists.txt          # Build configuration
├── README.md
├── INSTALL.md
├── FEATURES.md
└── ARCHITECTURE.md
```

## Core Components

### 1. Shell (Entry Point)

**Location**: `src/core/shell.cpp`

**Responsibilities**:
- Main execution loop
- User input handling
- Signal management
- System initialization/shutdown
- Configuration loading

**Key Classes**:
- `Shell`: Main shell controller

### 2. Command Processor

**Location**: `src/core/command_processor.cpp`

**Responsibilities**:
- Parse command lines
- Validate syntax
- Route commands to handlers
- Manage I/O redirection
- Handle pipes and background jobs

**Key Classes**:
- `CommandProcessor`: Command execution engine
- `CommandRegistry`: Command registration and lookup
- `CommandContext`: Execution context

### 3. Authentication & Access Control

**Location**: `src/auth/`

**Responsibilities**:
- User management
- Password hashing and verification
- Session management
- Permission enforcement

**Key Classes**:
- `Authentication`: Singleton for auth operations
- `UserInfo`: User data structure
- Enums: `UserRole`, `Permission`

**Security**:
- SHA-256 password hashing
- Per-user salt generation
- Role-based access control (RBAC)

### 4. Virtual File System (VFS)

**Location**: `src/vfs/`

**Responsibilities**:
- Abstract file system operations
- Mount/unmount file systems
- File recovery
- Cross-platform file access

**Key Classes**:
- `VirtualFileSystem`: Singleton VFS manager
- `IFileSystem`: File system interface
- `FileInfo`: File metadata

**Design Pattern**: Strategy Pattern for different FS types

### 5. Password Manager (Vault)

**Location**: `src/vault/`

**Responsibilities**:
- Secure password storage
- Encryption/decryption
- Password generation
- Import/export functionality

**Key Classes**:
- `PasswordManager`: Singleton vault manager
- `PasswordEntry`: Password record
- `PasswordGenOptions`: Generation settings

**Security**:
- AES-256-GCM encryption
- PBKDF2 key derivation
- Secure memory handling

### 6. Network Packet Analyzer

**Location**: `src/network/`

**Responsibilities**:
- Packet capture
- Protocol parsing
- Traffic analysis
- Statistics generation

**Key Classes**:
- `PacketAnalyzer`: Singleton analyzer
- `PacketInfo`: Packet metadata
- `NetworkStats`: Statistics

**Dependencies**: libpcap (optional)

### 7. Plugin System

**Location**: `src/plugins/`

**Responsibilities**:
- Dynamic library loading
- Plugin lifecycle management
- Command registration from plugins
- Plugin discovery

**Key Classes**:
- `PluginManager`: Singleton plugin controller
- `IPlugin`: Plugin interface
- `PluginInfo`: Plugin metadata

**Design Pattern**: Plugin Architecture

### 8. Scripting Engine

**Location**: `src/scripting/`

**Responsibilities**:
- Script parsing
- Command execution
- Variable management
- Macro support

**Key Classes**:
- `ScriptEngine`: Singleton script executor
- `ScriptContext`: Execution context
- `Variable`: Script variable

### 9. Logging & Audit System

**Location**: `src/logging/`

**Responsibilities**:
- Event logging
- Audit trail
- Log rotation
- Search functionality

**Key Classes**:
- `Logger`: Singleton logging system
- `LogEntry`: Log record
- `AuditEntry`: Audit record

**Features**:
- Multiple log levels
- File and console output
- Searchable logs
- Tamper-evident audit trail

## Design Patterns Used

### Singleton Pattern
Used for system-wide services:
- `Authentication`
- `VirtualFileSystem`
- `PasswordManager`
- `PacketAnalyzer`
- `PluginManager`
- `ScriptEngine`
- `Logger`

### Factory Pattern
- Plugin creation
- File system instantiation

### Strategy Pattern
- Different file system implementations
- Encryption algorithms

### Observer Pattern
- Event logging
- Plugin hooks

### Command Pattern
- Command registry and execution

### Pimpl (Pointer to Implementation)
- Hide implementation details
- Reduce compilation dependencies
- ABI stability

## Threading Model

```
Main Thread
├── User Input Loop
├── Command Execution
└── Signal Handling

Background Threads
├── Network Capture Thread
├── File System Operations
├── Plugin Workers
└── Async Command Execution
```

**Synchronization**:
- `std::mutex` for critical sections
- Thread-safe singletons
- Lock-free data structures where possible

## Memory Management

- **Smart Pointers**: `std::unique_ptr`, `std::shared_ptr`
- **RAII**: Resource cleanup in destructors
- **No Raw Pointers**: Except for platform APIs
- **Secure Wiping**: Sensitive data cleared from memory

## Error Handling

```cpp
// Exception-based for initialization
try {
    shell.initialize();
} catch (const std::exception& e) {
    log_error(e.what());
}

// Return codes for command execution
int execute_command() {
    return success ? 0 : 1;
}

// Optional for nullable returns
std::optional<PasswordEntry> get_password();
```

## Inter-Module Communication

```
Command → CommandRegistry → Handler → Module API → Module Implementation
```

### Example Flow

```cpp
// User types: vault-get github.com

1. Shell::read_input() receives command
2. CommandProcessor::process() parses it
3. CommandRegistry::execute() finds handler
4. Handler calls PasswordManager::get_password()
5. PasswordManager returns encrypted data
6. Handler decrypts and displays
7. Logger::audit() records access
```

## Plugin Architecture

```cpp
// Plugin Interface
class IPlugin {
public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::vector<std::string> get_commands() = 0;
};

// Plugin Implementation
class MyPlugin : public IPlugin {
    bool initialize() override {
        register_command("mycommand", &MyPlugin::handler);
        return true;
    }
};

// Export
EXPORT_PLUGIN(MyPlugin)
```

**Loading Process**:
1. `dlopen()` / `LoadLibrary()` loads `.so` / `.dll`
2. Locate `create_plugin()` symbol
3. Call to instantiate plugin
4. Plugin registers its commands
5. Commands become available in shell

## Security Architecture

### Layers of Security

1. **Authentication Layer**: User verification
2. **Authorization Layer**: Permission checking
3. **Encryption Layer**: Data protection
4. **Audit Layer**: Activity tracking
5. **Isolation Layer**: Process separation (planned)

### Threat Model

**Protected Against**:
- Unauthorized access
- Password theft
- Command injection
- Memory leaks
- Log tampering

**Not Protected Against** (out of scope):
- Physical access
- OS-level compromise
- Side-channel attacks

## Performance Optimizations

1. **Lazy Loading**: Modules loaded on-demand
2. **Caching**: Command registry, file metadata
3. **String Pooling**: Reduce allocations
4. **Move Semantics**: Efficient resource transfer
5. **Lock-Free Structures**: Where appropriate

## Platform Abstraction

```cpp
#ifdef _WIN32
    // Windows-specific code
#else
    // POSIX-specific code
#endif
```

**Platform-Specific**:
- File I/O
- Dynamic library loading
- Network interfaces
- Console handling

## Build System

**CMake** orchestrates the build:
- Finds dependencies (OpenSSL, libpcap)
- Configures platform-specific settings
- Generates platform-native build files
- Handles optional components

## Testing Strategy

1. **Unit Tests**: Individual component testing
2. **Integration Tests**: Module interaction testing
3. **System Tests**: End-to-end scenarios
4. **Security Tests**: Penetration testing (planned)

## Extensibility Points

1. **Plugins**: Add new commands and features
2. **File Systems**: Implement `IFileSystem`
3. **Encryption**: Swappable algorithms
4. **Scripting**: Custom functions
5. **Logging**: Custom log handlers

## Future Architecture Enhancements

- **Microkernel Design**: Core + isolated services
- **IPC Mechanism**: Inter-process communication
- **Container Support**: Run isolated processes
- **Network Services**: Remote shell access
- **GUI Layer**: Graphical interface
- **REST API**: HTTP interface

## Dependencies

### Required
- **OpenSSL**: Cryptography (AES, SHA, PBKDF2)
- **C++17 STL**: Containers, algorithms, threading

### Optional
- **libpcap**: Packet capture (network features)

### Build Only
- **CMake**: Build system
- **Compiler**: GCC, Clang, or MSVC

## Development Guidelines

1. **Follow SOLID Principles**
2. **Use Modern C++ Features**
3. **Document Public APIs**
4. **Write Unit Tests**
5. **Handle Errors Gracefully**
6. **Profile Before Optimizing**
7. **Keep It Simple**

## Code Organization Principles

- **Single Responsibility**: One class, one purpose
- **Open/Closed**: Open for extension, closed for modification
- **Dependency Inversion**: Depend on abstractions
- **Interface Segregation**: Small, focused interfaces
- **Don't Repeat Yourself** (DRY)

---

## 🚀 Summary

This architecture supports **NovaShell's 21 major features** while allowing for future expansion through well-defined interfaces and extension points. The modular design ensures:

✅ **Maintainability** - Easy to understand and modify
✅ **Extensibility** - Add new features via plugins
✅ **Performance** - Optimized for speed and efficiency
✅ **Security** - Multiple layers of protection
✅ **Reliability** - Robust error handling

**NovaShell** - Professional architecture for professional developers! 🎉
