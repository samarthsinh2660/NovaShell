# CustomOS Shell - Advanced System Management Shell

A comprehensive, extensible command-line shell built in C++ featuring virtual file systems, network analysis, password management, and more.

## 🌟 Features

### 🤖 **AI-Powered Command Suggestions** ⭐NEW
- Intelligent, context-aware command suggestions using Google Gemini AI
- Autocomplete based on shell history and current context
- Predictive next-command suggestions
- Learn from your workflow patterns
- Commands: `ai-suggest`, `ai-enable`, `ai-disable`

### 1. **Virtual File System (VFS)**
- Mount multiple file systems (FAT, NTFS, ext4 emulation)
- File recovery and secure deletion
- Cross-platform disk access
- Commands: `mount`, `umount`, `vfs-ls`, `vfs-cp`, `recover`

### 2. **Plugin System**
- Dynamic module loading
- Third-party extensions support
- Hot-pluggable functionality
- Commands: `load-plugin`, `unload-plugin`, `list-plugins`

### 3. **User & Access Control**
- Multi-user support with role-based permissions
- Secure authentication system
- Command-level access restrictions
- Commands: `login`, `logout`, `adduser`, `passwd`, `whoami`

### 4. **Scripting Engine**
- Built-in scripting language support
- Automation and macro creation
- Batch command execution
- Commands: `script`, `macro`, `run`

### 5. **Network Packet Analyzer**
- Real-time packet capture and analysis
- Protocol inspection (TCP, UDP, HTTP, DNS)
- Traffic statistics and reporting
- Commands: `net-sniff`, `net-analyze`, `net-capture`, `net-stats`

### 6. **Password Manager**
- AES-256 encrypted vault
- Secure password generation
- Cross-platform sync ready
- Commands: `vault-add`, `vault-get`, `vault-list`, `vault-gen`

### 7. **File System Utilities**
- Defragmentation tools
- File recovery mechanisms
- Compression utilities
- Commands: `defrag`, `recover`, `compress`, `wipe`

### 8. **Logging & Audit System**
- Comprehensive command logging
- Security event tracking
- Audit trail search
- Commands: `log-view`, `audit`, `log-search`

### 9. **Process Management**
- Process listing and control
- Simple task scheduling
- Resource monitoring
- Commands: `proc-list`, `proc-kill`, `schedule`

### 10. **Cloud Backup & Sync**
- Encrypted backup creation
- Cross-device synchronization
- Commands: `backup`, `sync-push`, `sync-pull`

### 🌐 **Remote Shell Access (SSH-like)** ⭐NEW
- Secure remote shell access over network
- SSH protocol support with encryption
- Multi-client connection management
- Key-based and password authentication
- Commands: `ssh-start`, `ssh-stop`, `ssh-connections`

### 🐳 **Container Management** ⭐NEW
- Docker and Podman integration
- Manage containers, images, networks, and volumes
- Execute commands in containers
- Build and deploy containerized applications
- Commands: `container-list`, `container-start`, `container-stop`, `image-pull`, `image-build`

### 📝 **Note & Snippet Manager** ⭐NEW
- Organize study notes and code snippets
- Tag-based categorization
- Quick search and retrieval
- Export to Markdown/JSON
- Commands: `note-add`, `note-list`, `snippet-add`, `snippet-get`, `snippet-search`

### 📊 **Real-Time System Monitor** ⭐NEW
- Live CPU, memory, disk, and network monitoring
- Process management and resource tracking
- Alert thresholds with notifications
- Historical data graphs
- Commands: `monitor-start`, `monitor-stats`, `proc-list`, `proc-kill`

### 🔧 **Project Environment Manager** ⭐NEW
- Switch between development environments
- Manage SDKs (Java, Python, Node, C++)
- Project-specific environment profiles
- PATH and environment variable management
- Commands: `env-create`, `env-switch`, `env-set`, `sdk-list`

### 🔄 **Peer-to-Peer File Sharing** ⭐NEW
- LAN file sharing without cloud services
- Automatic peer discovery
- Encrypted file transfers
- Transfer progress tracking
- Commands: `p2p-start`, `p2p-share`, `p2p-get`, `p2p-peers`

### ⏰ **Task Scheduler & Reminder System** ⭐NEW
- Schedule commands and scripts
- Set reminders for deadlines and exams
- Recurring tasks (daily, weekly, monthly)
- Priority-based task management
- Commands: `task-schedule`, `task-list`, `remind`, `remind-list`

## 📋 Requirements

- **C++ Compiler**: C++17 or later (GCC, Clang, MSVC)
- **CMake**: Version 3.15 or higher
- **OpenSSL**: For encryption (libssl-dev)
- **Optional**: libpcap (for network packet capture)

### Platform-Specific Requirements

#### Windows
- Visual Studio 2019+ or MinGW-w64
- WinPcap or Npcap (for network features)

#### Linux
```bash
sudo apt-get install build-essential cmake libssl-dev libpcap-dev
```

#### macOS
```bash
brew install cmake openssl libpcap
```

## 🚀 Installation

### 1. Clone the Repository
```bash
git clone https://github.com/yourusername/customos-shell.git
cd customos-shell
```

### 2. Build the Project
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 3. Install (Optional)
```bash
# Linux/macOS
sudo cmake --install .

# Windows (as Administrator)
cmake --install .
```

## 🎯 Quick Start

### Running the Shell
```bash
# From build directory
./customos-shell

# Or if installed
customos-shell
```

### First Time Setup
```bash
# Create admin user
customos> adduser admin
Enter password: ****
User 'admin' created successfully.

# Login
customos> login admin
Password: ****
Logged in as admin

# Initialize password vault
customos> vault-init
Vault initialized successfully

# Configure AI suggestions (optional)
customos> ai-init
Enter Gemini API Key: YOUR_API_KEY
AI suggestions enabled successfully
```

### 🔑 Getting Your Gemini API Key

1. Go to [Google AI Studio](https://makersuite.google.com/app/apikey)
2. Sign in with your Google account
3. Click "Create API Key"
4. Copy your API key
5. Configure in shell:
   ```bash
   customos> ai-init YOUR_GEMINI_API_KEY
   ```
   Or edit `~/.customos/config.json`:
   ```json
   {
     "ai": {
       "enabled": true,
       "gemini_api_key": "YOUR_API_KEY_HERE"
     }
   }
   ```

**Note**: Gemini API is FREE for personal use with generous rate limits!

## 📖 Usage Guide

### Virtual File System

#### Mounting a Drive
```bash
customos> mount /dev/sda1 /mnt/disk1
Mounted /dev/sda1 at /mnt/disk1

customos> vfs-ls /mnt/disk1
Documents/
Pictures/
file.txt
```

#### File Recovery
```bash
customos> recover /mnt/disk1/lost_file.doc
Scanning for deleted files...
Found: lost_file.doc (2048 KB)
Recover? (y/n): y
File recovered to /mnt/disk1/recovered/lost_file.doc
```

### Network Analysis

#### Packet Capture
```bash
customos> net-sniff eth0
Starting packet capture on eth0...
Press Ctrl+C to stop.

[TCP] 192.168.1.100:54321 -> 93.184.216.34:80
[UDP] 192.168.1.100:53 -> 8.8.8.8:53 (DNS Query)
```

#### Network Statistics
```bash
customos> net-stats
Interface: eth0
Packets Captured: 1,523
TCP: 892 (58.6%)
UDP: 431 (28.3%)
Other: 200 (13.1%)
```

### Password Manager

#### Adding Passwords
```bash
customos> vault-add
Service name: github.com
Username: myuser
Password: (leave blank to generate)
Generated strong password: Xk9$mP2#qL5@nR8

customos> vault-add github.com myuser
Password saved to vault
```

#### Retrieving Passwords
```bash
customos> vault-get github.com
Username: myuser
Password: Xk9$mP2#qL5@nR8
(Password copied to clipboard for 30 seconds)
```

### Scripting

#### Creating a Script
```bash
customos> script edit backup.cos
# backup.cos
log "Starting backup..."
vfs-cp /important/* /backup/
vault-export /backup/passwords.enc
sync-push /backup/
log "Backup complete"

customos> script run backup.cos
Starting backup...
Copying files...
Backup complete
```

### Plugin System

#### Loading Plugins
```bash
customos> load-plugin modules/advanced_crypto.dll
Plugin 'advanced_crypto' loaded successfully
New commands: encrypt, decrypt, hash-verify

customos> list-plugins
Loaded Plugins:
  - advanced_crypto v1.0 (4 commands)
  - network_tools v2.1 (7 commands)
```

### Access Control

#### Managing Users
```bash
customos> adduser developer
customos> grant developer vfs-read,script-run
customos> deny developer net-sniff,vault-*

customos> whoami
User: admin
Role: administrator
Permissions: all
```

### Logging & Audit

#### Viewing Logs
```bash
customos> log-view --last 10
[2024-01-15 14:32:01] [admin] login successful
[2024-01-15 14:32:15] [admin] vault-get github.com
[2024-01-15 14:35:42] [admin] net-sniff eth0 started
...

customos> audit --user admin --command vault-*
Audit Trail for user 'admin', commands matching 'vault-*':
[2024-01-15 14:32:15] vault-get github.com - SUCCESS
[2024-01-15 15:10:33] vault-add twitter.com - SUCCESS
```

### 🤖 AI Command Suggestions

#### Using AI Suggestions
```bash
customos> # Start typing and get suggestions
customos> git 
AI Suggestions:
  1. git status - Check repository status
  2. git pull - Update from remote
  3. git commit -m "message" - Commit changes

customos> # Get context-aware next command
customos> ai-suggest
Based on your recent commands, you might want to:
  - vault-get github.com (you just logged in)
  - vfs-ls /projects (you often work here next)
  - monitor-stats (morning routine check)
```

### 🌐 Remote Shell Access

#### Start SSH Server
```bash
customos> ssh-start 2222
SSH server started on port 2222
Server address: 192.168.1.100:2222

# On remote machine
$ ssh admin@192.168.1.100 -p 2222
Password: ****
Welcome to CustomOS Shell
customos> 
```

### 🐳 Container Management

#### Managing Containers
```bash
customos> container-list
NAME        IMAGE           STATUS    PORTS
web-app     nginx:latest    Running   80->8080
db          postgres:13     Running   5432->5432

customos> container-exec web-app ls /var/www
index.html
assets/

customos> image-pull ubuntu:22.04
Pulling ubuntu:22.04...
Download complete
```

### 📝 Notes & Snippets

#### Managing Notes
```bash
customos> note-add "Data Structures Exam" --tags exam,cs --category study
Note added: id_001

customos> snippet-add "Binary Search" cpp --tags algorithm
Enter code (Ctrl+D when done):
int binarySearch(int arr[], int l, int r, int x) {
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (arr[mid] == x) return mid;
        if (arr[mid] < x) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}
Snippet saved: id_002

customos> snippet-search "binary"
Found 1 snippet:
  [id_002] Binary Search (cpp) - algorithm
```

### 📊 System Monitor

#### Real-Time Monitoring
```bash
customos> monitor-start
System monitoring started

customos> monitor-stats
CPU Usage: 45.2% (8 cores)
Memory: 8.5 GB / 16 GB (53%)
Disk: 450 GB / 1 TB (45%)
Network: ↓ 2.5 MB/s ↑ 0.8 MB/s

customos> proc-list --top 5
PID    NAME           CPU%   MEM      USER
1234   chrome         25.3%  2.1 GB   user
5678   vscode         15.2%  1.5 GB   user
9012   node           8.5%   850 MB   user
```

### 🔧 Environment Manager

#### Managing Dev Environments
```bash
customos> env-create java-project "Java 17 Development"
Environment 'java-project' created

customos> env-set JAVA_HOME /usr/lib/jvm/java-17
customos> env-add-path /usr/lib/jvm/java-17/bin

customos> env-switch java-project
Switched to environment 'java-project'
Java: 17.0.5
Maven: 3.8.6

customos> env-switch python-ml
Switched to environment 'python-ml'
Python: 3.10.8
TensorFlow: 2.11.0
```

### 🔄 P2P File Sharing

#### Sharing Files on LAN
```bash
customos> p2p-start
P2P server started on port 8888
Discovering peers...

customos> p2p-peers
Found 3 peers:
  - Alice's Laptop (192.168.1.101)
  - Bob's Desktop (192.168.1.102)
  - Lab Computer 5 (192.168.1.105)

customos> p2p-share /projects/report.pdf
File shared: report.pdf (2.5 MB)
Share ID: file_001

# On another peer
customos> p2p-get 192.168.1.100 file_001 ~/downloads/
Downloading report.pdf...
Progress: [████████████████████] 100% (2.5 MB/s)
Transfer complete
```

### ⏰ Task Scheduler

#### Scheduling Tasks
```bash
customos> task-schedule "Daily Backup" "script run backup.cos" --daily 02:00
Task scheduled: task_001
Next run: Tomorrow at 2:00 AM

customos> remind "Submit Assignment" --in 2h
Reminder set for 2 hours from now
Reminder ID: rem_001

customos> task-list
ID         TITLE            NEXT RUN           STATUS
task_001   Daily Backup     Today, 2:00 AM     Enabled
task_002   Weekly Report    Monday, 9:00 AM    Enabled

customos> remind-list
ID        TITLE                TIME               STATUS
rem_001   Submit Assignment    In 1 hour 45 min   Active
rem_002   Exam Tomorrow        In 18 hours        Active
```

## 🏗️ Architecture

```
customos-shell/
├── src/
│   ├── core/           # Core shell engine
│   ├── vfs/            # Virtual File System
│   ├── auth/           # Authentication & Access Control
│   ├── network/        # Packet Analyzer
│   ├── vault/          # Password Manager
│   ├── scripting/      # Script Engine
│   ├── plugins/        # Plugin System
│   ├── logging/        # Logging & Audit
│   ├── ai/             # AI Command Suggestions ⭐NEW
│   ├── remote/         # SSH Server ⭐NEW
│   ├── containers/     # Container Management ⭐NEW
│   ├── notes/          # Note & Snippet Manager ⭐NEW
│   ├── monitor/        # System Monitor ⭐NEW
│   ├── env/            # Environment Manager ⭐NEW
│   ├── p2p/            # P2P File Sharing ⭐NEW
│   ├── scheduler/      # Task Scheduler ⭐NEW
│   └── utils/          # Utilities
├── include/            # Public headers
├── plugins/            # Sample plugins
├── docs/               # Documentation
├── tests/              # Unit tests
├── config.template.json # Configuration template ⭐NEW
└── CMakeLists.txt      # Build configuration
```

## 🔧 Configuration

Configuration file: `~/.customos/config.json`

```json
{
  "shell": {
    "prompt": "customos> ",
    "history_size": 1000,
    "auto_save": true
  },
  "vault": {
    "encryption": "AES-256-GCM",
    "key_derivation": "PBKDF2",
    "iterations": 100000
  },
  "network": {
    "default_interface": "eth0",
    "capture_buffer": 65536
  },
  "logging": {
    "level": "info",
    "max_size_mb": 100,
    "retention_days": 30
  }
}
```

## 🔒 Security

- **Password Storage**: AES-256-GCM encryption with PBKDF2 key derivation
- **Access Control**: Role-based with command-level granularity
- **Audit Logging**: Tamper-evident logs with checksums
- **Network Capture**: Requires elevated privileges (admin/root)
- **Plugin Verification**: Optional signature verification for plugins

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Plugin Development

Create custom plugins using our API:

```cpp
#include "customos/plugin_api.h"

class MyPlugin : public IPlugin {
public:
    bool initialize() override {
        register_command("mycommand", &MyPlugin::my_command);
        return true;
    }
    
    void my_command(const std::vector<std::string>& args) {
        // Your implementation
    }
};

EXPORT_PLUGIN(MyPlugin)
```

## 📜 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file.

## 🙏 Acknowledgments

- OpenSSL for cryptographic functions
- libpcap for packet capture
- Inspired by Unix philosophy and modern DevOps tools

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/customos-shell/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/customos-shell/discussions)
- **Email**: support@customos-shell.dev

## 🗺️ Roadmap

### ✅ Completed (v1.0.0)
- [x] AI-powered command suggestions with Gemini
- [x] Remote shell access (SSH-like)
- [x] Container management integration
- [x] Note & Snippet Manager
- [x] Real-Time System Monitor
- [x] Project Environment Manager
- [x] P2P File Sharing
- [x] Task Scheduler & Reminder System

### 🚧 In Progress / Planned
- [ ] GUI Dashboard for monitoring
- [ ] Mobile companion app
- [ ] Cloud backend for sync
- [ ] Database Support (SQLite, PostgreSQL)
- [ ] Git Integration
- [ ] Custom Themes & UI Customization
- [ ] Tab Completion Enhancement
- [ ] Voice Commands
- [ ] Advanced Analytics Dashboard
- [ ] Multi-language Support

---

**Made with ❤️ by the CustomOS Shell Team**
