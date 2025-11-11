# Quick Start Guide

Get up and running with CustomOS Shell in 5 minutes!

## Installation

### Windows

```powershell
# 1. Install dependencies
# Download and install OpenSSL: https://slproweb.com/products/Win32OpenSSL.html
# Install CMake: https://cmake.org/download/

# 2. Clone and build
git clone https://github.com/yourusername/customos-shell.git
cd customos-shell
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release

# 3. Run
.\bin\Release\customos-shell.exe
```

### Linux

```bash
# 1. Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake libssl-dev libpcap-dev

# 2. Clone and build
git clone https://github.com/yourusername/customos-shell.git
cd customos-shell
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)

# 3. Run
./bin/customos-shell
```

### macOS

```bash
# 1. Install dependencies
brew install cmake openssl libpcap

# 2. Clone and build
git clone https://github.com/yourusername/customos-shell.git
cd customos-shell
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
cmake --build . -j$(sysctl -n hw.ncpu)

# 3. Run
./bin/customos-shell
```

## First Run

### Welcome Screen

```
╔════════════════════════════════════════════════════════════╗
║          CustomOS Shell - Advanced System Manager         ║
║                     Version 1.0.0                          ║
╚════════════════════════════════════════════════════════════╝

Type 'help' for available commands or 'exit' to quit.

customos> 
```

### Essential Commands

```bash
# Get help
customos> help

# Check version
customos> version

# See who you are (guest by default)
customos> whoami

# List available commands
customos> help
```

## Common Tasks

### 1. User Management

```bash
# Create an admin user (first time setup)
customos> adduser admin
Enter password: ****
Confirm password: ****
User 'admin' created successfully.

# Login
customos> login admin
Password: ****
Logged in as admin

# Check current user
customos> whoami
admin
```

### 2. Password Manager

```bash
# Initialize vault
customos> vault-init
Enter master password: ****
Confirm master password: ****
Vault initialized successfully

# Add a password
customos> vault-add github.com myusername
Enter password (leave blank to generate): 
Generated password: Xk9$mP2#qL5@nR8
Password saved

# Get a password
customos> vault-get github.com
Service: github.com
Username: myusername
Password: Xk9$mP2#qL5@nR8
(Copied to clipboard for 30 seconds)

# List all passwords
customos> vault-list
github.com - myusername
twitter.com - myusername
gmail.com - myemail@gmail.com
```

### 3. Network Analysis

```bash
# List network interfaces
customos> net-interfaces
eth0 - Ethernet Adapter
lo - Loopback Interface

# Start packet capture (requires admin/root)
customos> net-sniff eth0
Starting capture on eth0...
[TCP] 192.168.1.100:54321 -> 93.184.216.34:80
[DNS] 192.168.1.100 -> 8.8.8.8 (Query: example.com)
Press Ctrl+C to stop.

# View statistics
customos> net-stats
Packets Captured: 1,523
TCP: 892 (58.6%)
UDP: 431 (28.3%)
Other: 200 (13.1%)
```

### 4. File Operations (VFS)

```bash
# Mount a drive
customos> mount /dev/sda1 /mnt/usb
Mounted successfully

# List files
customos> vfs-ls /mnt/usb
Documents/
Pictures/
file.txt (2.5 MB)

# Copy files
customos> vfs-cp /mnt/usb/file.txt /backup/
Copied successfully
```

### 5. Scripting

```bash
# Create a backup script
customos> script edit daily-backup.cos

# Edit in your favorite editor, or type inline:
# backup.cos
set date = $(date +%Y%m%d)
vfs-mkdir /backup/$date
vfs-cp /important/* /backup/$date/
vault-export /backup/$date/vault.enc
log "Backup complete: $date"

# Run the script
customos> script run daily-backup.cos
Backup complete: 20240115
```

### 6. Logging & Audit

```bash
# View recent logs
customos> log-view --last 10
[2024-01-15 14:32:01] [INFO] Shell initialized
[2024-01-15 14:32:15] [INFO] User admin logged in
[2024-01-15 14:35:42] [INFO] Network capture started

# Search logs
customos> log-search "error"
[2024-01-15 10:23:45] [ERROR] Failed to mount /dev/sdb1

# View audit trail
customos> audit
[2024-01-15 14:32:15] admin - login - SUCCESS
[2024-01-15 14:35:42] admin - net-sniff eth0 - SUCCESS
```

## Tips & Tricks

### Command History

Use arrow keys (↑/↓) to navigate command history.

### Tab Completion

Type partial command and press Tab for auto-completion (coming soon).

### Background Jobs

Run long operations in background:
```bash
customos> net-sniff eth0 &
[1] Started in background
```

### Output Redirection

```bash
# Save output to file
customos> log-view > output.txt

# Append to file
customos> net-stats >> report.txt
```

### Command Chaining

```bash
# Sequential execution
customos> vault-get github.com ; vault-get twitter.com

# Conditional execution
customos> login admin && vault-unlock || echo "Login failed"
```

### Aliases (Planned Feature)

```bash
customos> alias ll = "vfs-ls -la"
customos> ll /home
```

## Configuration

Default config location: `~/.customos/config.json`

```json
{
  "shell": {
    "prompt": "customos> ",
    "history_size": 1000
  },
  "vault": {
    "encryption": "AES-256-GCM"
  },
  "logging": {
    "level": "info"
  }
}
```

## Getting Help

### In-Shell Help

```bash
# General help
customos> help

# Command-specific help
customos> help vault-add

# List all commands
customos> help --all
```

### Documentation

- **README.md** - Project overview
- **FEATURES.md** - Complete feature list
- **INSTALL.md** - Installation guide
- **BUILD_GUIDE.md** - Building from source
- **ARCHITECTURE.md** - System architecture

### Community

- GitHub Issues: Report bugs
- Discussions: Ask questions
- Discord: Chat with community (if available)

## Common Issues

### "Command not found"

Make sure you've logged in as a user with appropriate permissions.

### "Vault is locked"

Unlock the vault first:
```bash
customos> vault-unlock
Enter master password: ****
```

### "Permission denied" (Network)

Network packet capture requires elevated privileges:
```bash
# Linux
sudo ./customos-shell

# Or grant capabilities
sudo setcap cap_net_raw,cap_net_admin=eip ./customos-shell
```

### "OpenSSL error"

Ensure OpenSSL is properly installed and in your PATH.

## Next Steps

1. **Explore Features**: Try all the commands in `help`
2. **Create Scripts**: Automate your workflows
3. **Load Plugins**: Extend functionality
4. **Read Docs**: Dive deeper into advanced features
5. **Contribute**: Help improve the project!

## Example Workflow

Here's a typical daily workflow:

```bash
# 1. Login
customos> login admin
Password: ****

# 2. Unlock vault
customos> vault-unlock
Master password: ****

# 3. Get password for work
customos> vault-get work-email

# 4. Run backup script
customos> script run daily-backup.cos

# 5. Check system logs
customos> log-view --last 20

# 6. Monitor network (if needed)
customos> net-sniff eth0

# 7. Logout
customos> logout
customos> exit
```

---

Welcome to CustomOS Shell! Enjoy the power and flexibility! 🚀
