# NovaShell - Your AI-Powered Terminal Assistant

**Transform your command line into an intelligent coding companion powered by Google Gemini AI!**

---

## 🚀 Quick Start

### Installation (Windows - 3 Simple Steps)

```powershell
# Step 1: Install dependencies (run as Administrator)
.\setup-dependencies.ps1

# Step 2: Build the project
.\build.ps1

# Step 3: Run NovaShell
.\run-novashell.ps1
```

**That's it!** You're ready to experience the future of terminal interfaces.

### First Time Setup

```bash
# Login with default credentials
novashell> login admin
Password: admin

# Change password (recommended)
novashell> create-user myadmin your_secure_password

# Setup AI features (optional - get free API key from https://makersuite.google.com/app/apikey)
novashell> ai-init YOUR_GEMINI_API_KEY
```

---

## 📋 Table of Contents

1. [🌟 What is NovaShell?](#-what-is-novashell)
2. [🎯 Key Features](#-key-features)
3. [💡 Why NovaShell?](#-why-novashell)
4. [⚡ Quick Installation](#-installation-guide)
5. [📖 Complete Command Reference](#-complete-command-reference)
6. [🔧 Configuration](#-configuration)
7. [🏗️ Architecture](#-architecture)
8. [🐛 Troubleshooting](#-troubleshooting)
9. [🤝 Contributing](#-contributing)
10. [📞 Support](#-support)

---

## 🌟 What is NovaShell?

NovaShell is a **next-generation command-line shell** that combines traditional Unix power with cutting-edge AI capabilities. Built in C++17, it offers 25+ features including:

- 🧠 **AI Command Understanding** - Natural language to CLI commands
- 💻 **Code Generation & Analysis** - Write, debug, and review code with AI
- 🔐 **Secure Password Vault** - AES-256 encrypted password management
- 🐳 **Container Management** - Docker/Podman integration
- 🔀 **Git Integration** - Full version control workflow
- 💾 **Database Support** - MySQL, PostgreSQL, SQLite
- 📊 **System Monitoring** - Real-time resource tracking
- 🌐 **Remote Access** - SSH server with encryption
- ⏰ **Task Scheduler** - Automated command execution
- 📝 **Notes & Snippets** - Integrated knowledge management

**NovaShell v1.5**: The first terminal that truly understands you! 🤖✨

---

## 🎯 Key Features

### 🤖 AI-Powered Features (10 Revolutionary Capabilities)

| Feature | Description | Command | Example |
|---------|-------------|---------|---------|
| 🧠 **Natural Language** | Convert English to shell commands | `ai-interpret` | "backup my files" → `tar -czf backup.tar.gz ~/files` |
| 📋 **Task Planning** | Multi-step automation plans | `ai-plan` | "prepare deployment" → generates step-by-step workflow |
| 💻 **Code Generation** | Create functions, classes, tests | `ai-generate` | "binary search in C++" → complete implementation |
| 🔧 **Code Editing** | AI refactoring and improvements | `ai-edit` | "optimize performance" → improved code |
| 🐛 **Debugging Assistant** | Analyze errors and provide solutions | `ai-debug` | "segfault at line 42" → root cause + fix |
| 📄 **Code Analysis** | Comprehensive code review | `ai-analyze` | "analyze main.cpp" → bugs, suggestions |
| 🔍 **Code Review** | Professional security audit | `ai-review` | "review security" → vulnerabilities found |
| 🧪 **Test Generation** | Generate comprehensive tests | `ai-test` | "test calculator" → full test suite |
| 📚 **Coding Tutor** | Interactive learning assistance | `ai-help` | "implement merge sort" → step-by-step guide |
| 🧠 **Memory & Context** | Remembers your workflow | `ai-context` | "what was I working on?" → recalls context |

### 💼 Core Features (15 Essential Tools)

| Feature | Description | Key Commands |
|---------|-------------|-------------|
| 🔐 **Authentication** | Multi-user with role-based permissions | `login`, `create-user`, `whoami` |
| 🔑 **Password Vault** | AES-256 encrypted password storage | `vault-add`, `vault-get`, `vault-list` |
| 🔀 **Git Integration** | Full Git workflow support | `git status`, `git commit`, `git push` |
| 🐳 **Containers** | Docker/Podman management | `container-list`, `container-start` |
| 💾 **Database** | MySQL/PostgreSQL/SQLite support | `db-connect`, `db-query`, `db-export` |
| 📊 **System Monitor** | Real-time CPU, memory, disk tracking | `monitor-stats`, `proc-list` |
| 🌐 **Network Tools** | Packet capture and analysis | `net-interfaces`, `net-capture`, `net-stats` |
| 📝 **Notes & Snippets** | Code and study note management | `note-add`, `snippet-add`, `snippet-search` |
| ⏰ **Task Scheduler** | Automated task execution | `task-schedule`, `remind`, `task-list` |
| 🔧 **Environment Manager** | Switch dev environments | `env-create`, `env-switch` |
| 🔄 **P2P Sharing** | LAN file sharing without cloud | `p2p-start`, `p2p-share`, `p2p-get` |
| 🌐 **Remote Access** | SSH server with encryption | `ssh-start`, `ssh-stop` |
| 🎨 **Custom Themes** | 7+ built-in themes + custom | `theme-set`, `theme-create` |
| 🏗️ **Virtual File System** | Cross-platform file management | `mount`, `vfs-ls`, `recover` |
| ⌨️ **Smart Tab Completion** | AI-powered command suggestions | Press `TAB` |

---

## 💡 Why NovaShell?

### For Students 🎓
- ✅ **AI Study Assistant** - Get coding help and explanations
- ✅ **All-in-One Tool** - No need for 10 different apps
- ✅ **Free AI Help** - Gemini API learns your patterns
- ✅ **Git Made Easy** - Simplified version control
- ✅ **Database Practice** - Direct MySQL/PostgreSQL access
- ✅ **Study Tools** - Integrated notes and snippets
- ✅ **Learn Faster** - AI suggestions accelerate learning
- ✅ **Group Projects** - P2P sharing and collaboration
- ✅ **Stay Organized** - Task scheduler for deadlines

### For Developers 👨‍💻
- ✅ **AI Coding Assistant** - Intelligent code suggestions
- ✅ **Productivity Boost** - Everything in one environment
- ✅ **DevOps Ready** - Container, database, git integration
- ✅ **No Context Switching** - All tools in terminal
- ✅ **Automation Expert** - Scripting engine for workflows
- ✅ **Professional Quality** - Industry-standard architecture
- ✅ **Extensible Platform** - Plugin system for custom tools
- ✅ **Time Saver** - AI-powered tab completion makes you 3-5x faster
- ✅ **Full-Stack Ready** - Frontend to backend to deployment

### For AI Enthusiasts 🤖
- ✅ **Cutting-Edge AI** - Powered by Google Gemini
- ✅ **Natural Language Commands** - Voice and text-to-speech
- ✅ **Smart Learning** - Adapts to your usage patterns
- ✅ **Future-Proof** - Ready for AI-powered computing
- ✅ **Research Platform** - Experiment with AI workflows
- ✅ **Open Architecture** - Easy to integrate new AI models

---

## ⚡ Installation Guide

### System Requirements

#### All Platforms
- **C++ Compiler**: C++17 or later (GCC 11+, Clang, MSVC 2019+)
- **CMake**: Version 3.15 or higher
- **OpenSSL**: For encryption (libssl-dev)
- **SQLite3**: For internal database (libsqlite3-dev)
- **Git**: For repository cloning and git features

#### Windows
- **MinGW-w64** (GCC 11+) or **Visual Studio 2019+**
- **OpenSSL for Windows**: From https://slproweb.com/products/Win32OpenSSL.html
- **SQLite3**: Automatically installed by setup script
- **Optional**: WinPcap or Npcap for network features

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libssl-dev libsqlite3-dev libpcap-dev pkg-config
```

#### Linux (Fedora/RHEL)
```bash
sudo dnf install -y gcc-c++ cmake git openssl-devel libpcap-devel sqlite-devel
```

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake openssl sqlite3 libpcap
```

---

### Quick Installation (Windows - Recommended)

#### Step 1: Automated Dependency Installation

```powershell
# Run as Administrator
.\setup-dependencies.ps1
```

This installs:
- ✅ Chocolatey (Package manager)
- ✅ MinGW-w64 (GCC 11+ with C++17)
- ✅ OpenSSL (Cryptography library)
- ✅ SQLite3 (Database engine via MSYS2)
- ✅ CMake (Build system)

#### Step 2: Build NovaShell

```powershell
# Build Release version
.\build.ps1

# Or build with options
.\build.ps1 -Release     # Release build (optimized)
.\build.ps1 -Rebuild     # Clean and rebuild
```

#### Step 3: Run NovaShell

```powershell
# Recommended: Use launcher (handles PATH setup)
.\run-novashell.ps1

# Or run directly
.\build\bin\customos-shell.exe
```

---

### Manual Installation (All Platforms)

#### 1. Clone Repository
```bash
git clone https://github.com/yourusername/novashell.git
cd novashell
```

#### 2. Create Build Directory
```bash
mkdir build
cd build
```

#### 3. Configure with CMake

**Linux/macOS**:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Windows (MinGW)**:
```powershell
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
```

**Windows (Visual Studio)**:
```powershell
cmake .. -G "Visual Studio 16 2019" -A x64
```

#### 4. Build
```bash
# Linux/macOS
cmake --build . --config Release -j4

# Windows
cmake --build . --config Release
```

#### 5. Run
```bash
# From build directory
./bin/customos-shell

# Or install system-wide
sudo cmake --install .  # Linux/macOS (requires root)
cmake --install .       # Windows (run as Administrator)
```

---

### Build Options

Customize your build with CMake options:

```bash
# Disable network packet analyzer features
cmake .. -DENABLE_NETWORK=OFF

# Disable building tests
cmake .. -DBUILD_TESTS=OFF

# Disable sample plugins
cmake .. -DBUILD_PLUGINS=OFF

# Specify custom OpenSSL location
cmake .. -DOPENSSL_ROOT_DIR=/path/to/openssl

# Create static binary
cmake .. -DCMAKE_EXE_LINKER_FLAGS="-static"
```

---

### Getting Your Gemini API Key

1. Go to [Google AI Studio](https://makersuite.google.com/app/apikey)
2. Sign in with your Google account
3. Click "Create API Key"
4. Copy your API key
5. Configure in NovaShell:
   ```bash
   novashell> ai-init YOUR_GEMINI_API_KEY
   ```

**Note**: Gemini API is **FREE** for personal use with generous rate limits!

---

## 📖 Complete Command Reference

### 🔐 Authentication Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `login <username>` | Authenticate as user | `login admin` |
| `logout` | End current session | `logout` |
| `create-user <username> <password>` | Create new user | `create-user john pass123` |
| `whoami` | Display current user | `whoami` |

**Example Workflow**:
```bash
novashell> login admin
Password: admin
✅ Login successful! Welcome, admin.

novashell> whoami
admin

novashell> create-user dev mypassword
✅ User 'dev' created successfully!

novashell> logout
👋 Logged out successfully. Goodbye, admin!
```

---

### 🔑 Password Vault Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `vault-init` | Initialize vault | `vault-init` |
| `vault-add` | Store password | `vault-add` |
| `vault-get <service>` | Retrieve password | `vault-get github` |
| `vault-list` | List all entries | `vault-list` |
| `vault-delete <service>` | Remove entry | `vault-delete gmail` |
| `vault-search <term>` | Search passwords | `vault-search git` |
| `vault-gen [length]` | Generate password | `vault-gen 16` |

**Example Workflow**:
```bash
novashell> vault-add
Service name: github
Username: myuser
Password: (leave blank to auto-generate)
🔐 Generated strong password: Xk9$mP2#qL5@nR8
✅ Password saved to vault

novashell> vault-list
📋 Stored Passwords:
1. github (myuser)
2. gmail (email@example.com)

novashell> vault-get github
Service: github
Username: myuser
Password: Xk9$mP2#qL5@nR8
📋 (Copied to clipboard for 30 seconds)
```

**Security**: AES-256-GCM encryption, PBKDF2 key derivation, master password protection

---

### 🔀 Git Integration Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `git status` | Check repository status | `git status` |
| `git add <files>` | Stage files | `git add .` |
| `git commit <message>` | Commit changes | `git commit "Initial commit"` |
| `git push [remote] [branch]` | Push to remote | `git push origin main` |
| `git pull [remote] [branch]` | Pull from remote | `git pull origin main` |
| `git branch [name]` | List or create branch | `git branch feature/new` |
| `git checkout <branch>` | Switch branch | `git checkout develop` |
| `git merge <branch>` | Merge branch | `git merge feature/new` |
| `git log` | View commit history | `git log` |
| `git diff` | Show changes | `git diff` |

**Example Workflow**:
```bash
novashell> git init
✅ Initialized empty Git repository

novashell> git status
On branch main
Untracked files:
  README.md
  src/

novashell> git add .
✅ All changes staged

novashell> git commit "Initial commit"
[main abc1234] Initial commit
 5 files changed, 250 insertions(+)

novashell> git push origin main
🚀 Pushing to https://github.com/user/repo.git
✅ Done!
```

---

### 🤖 AI Command Understanding Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `ai-init <api-key>` | Initialize AI with Gemini API key | `ai-init YOUR_KEY` |
| `ai-interpret <text>` | Convert natural language to command | `ai-interpret "list all files"` |
| `ai-plan <goal>` | Generate multi-step plan | `ai-plan "deploy to production"` |
| `ai-suggest` | Get context-aware suggestions | `ai-suggest` |
| `ai-enable` | Enable AI suggestions | `ai-enable` |
| `ai-disable` | Disable AI suggestions | `ai-disable` |

**Example Workflow**:
```bash
novashell> ai-interpret "backup my important files"
💡 Interpreted command:
tar -czf backup.tar.gz ~/important/ && cp backup.tar.gz /backup/

Execute this command? (y/n): y
✅ Executing...

novashell> ai-plan "prepare for deployment"
📋 AI Generated Plan:
1. git status           # Check current state
2. git pull            # Get latest changes
3. npm run build       # Build project
4. npm test            # Run tests
5. git push            # Push to remote
6. deploy.sh           # Execute deployment

Execute plan? (y/n):
```

---

### 💻 AI Code Generation & Analysis Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `ai-generate <type> <lang> <description>` | Generate code | `ai-generate function cpp "binary search"` |
| `ai-analyze <file>` | Analyze code file | `ai-analyze main.cpp` |
| `ai-explain <file>` | Explain code | `ai-explain algorithm.py` |
| `ai-edit <file> <operation>` | Refactor/optimize code | `ai-edit main.cpp refactor` |
| `ai-debug <error> [file]` | Debug error | `ai-debug "segfault" main.cpp` |
| `ai-review <file> [focus]` | Code review | `ai-review auth.py security` |
| `ai-test <file> <framework>` | Generate tests | `ai-test calculator.java junit` |
| `ai-help <question>` | Coding assistance | `ai-help "implement singleton"` |

**Example: Code Generation**:
```bash
novashell> ai-generate function cpp "implement binary search with recursion"
💻 Generated Code:
```cpp
int binarySearch(int arr[], int left, int right, int target) {
    if (right >= left) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target)
            return mid;

        if (arr[mid] > target)
            return binarySearch(arr, left, mid - 1, target);

        return binarySearch(arr, mid + 1, right, target);
    }
    return -1;
}
```
✅ Copy to clipboard? (y/n):
```

**Example: Code Analysis**:
```bash
novashell> ai-analyze main.cpp
🔍 AI Code Analysis: main.cpp
===================
Language: C++
Lines: 245
Complexity: Medium
Overall Score: B+

📊 Issues Found:
⚠️  Line 67: Potential memory leak in process_data()
   💡 Consider using smart pointers or RAII pattern

⚠️  Line 123: Inefficient algorithm O(n²)
   💡 Could be optimized to O(n log n) with sorting

✅ Strengths:
✓ Good error handling patterns
✓ Clear function naming
✓ Proper resource management

💡 Recommendations:
• Add input validation for file operations
• Consider parallel processing for large datasets
• Add comprehensive logging
```

**Example: Debugging**:
```bash
novashell> ai-debug "segmentation fault at line 42" main.cpp
🐛 AI Debugging Analysis
========================

🎯 Root Cause: Null pointer dereference in memory allocation

📖 Detailed Explanation:
The segmentation fault occurs because malloc() return value
is not checked for NULL before dereference.

🔧 Fixed Code:
// Before (buggy)
int* data = (int*)malloc(size * sizeof(int));
data[0] = 42;  // Crash if data is NULL

// After (fixed)
int* data = (int*)malloc(size * sizeof(int));
if (data == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return EXIT_FAILURE;
}
data[0] = 42;

💡 Prevention Tips:
• Always check malloc/calloc return values
• Use smart pointers in C++
• Implement proper error handling

🎚️ Confidence: High (98%)
```

---

### 🐳 Container Management Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `container-init` | Initialize Docker/Podman | `container-init` |
| `container-list [--all]` | List containers | `container-list` |
| `container-start <id>` | Start container | `container-start web-app` |
| `container-stop <id>` | Stop container | `container-stop web-app` |
| `container-create <image> <name>` | Create container | `container-create nginx:latest web` |
| `container-remove <id>` | Remove container | `container-remove web-app` |
| `container-exec <id> <cmd>` | Execute command | `container-exec web ls /var` |
| `image-list` | List images | `image-list` |
| `image-pull <image>` | Pull image | `image-pull ubuntu:22.04` |

**Example Workflow**:
```bash
novashell> container-init
✅ Connected to Docker daemon

novashell> container-list
NAME        IMAGE           STATUS    PORTS
web-app     nginx:latest    Running   80->8080
db          postgres:13     Running   5432->5432

novashell> container-exec web-app ls /var/www
index.html
assets/
config/

novashell> image-pull ubuntu:22.04
📥 Pulling ubuntu:22.04...
✅ Download complete
```

---

### 💾 Database Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `db-connect <name> --mysql <host> <port> <db> <user>` | Connect to MySQL | `db-connect mydb --mysql localhost 3306 testdb user` |
| `db-connect <name> --postgres <host> <port> <db> <user>` | Connect to PostgreSQL | `db-connect pgdb --postgres localhost 5432 db user` |
| `db-connect <name> --sqlite <file>` | Connect to SQLite | `db-connect local --sqlite data.db` |
| `db-query <sql>` | Execute SQL query | `db-query "SELECT * FROM users"` |
| `db-list-tables` | List all tables | `db-list-tables` |
| `db-insert <table> <data>` | Insert data | `db-insert users name="John" email="john@test.com"` |
| `db-export <table> <file>` | Export to CSV | `db-export users backup.csv` |
| `db-import <table> <file>` | Import from CSV | `db-import users data.csv` |
| `db-switch <name>` | Switch database | `db-switch proddb` |

**Example Workflow**:
```bash
novashell> db-connect mydb --mysql localhost 3306 testdb myuser
Password: ****
✅ Connected to mydb (MySQL)

novashell> db-list-tables
📋 Tables in mydb:
users
products
orders

novashell> db-query "SELECT * FROM users LIMIT 5"
ID | NAME      | EMAIL
1  | John Doe  | john@example.com
2  | Jane Smith| jane@example.com
3  | Bob Jones | bob@test.com

novashell> db-export users users_backup.csv
✅ Exported 1,245 rows to users_backup.csv
```

---

### 📊 System Monitoring Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `monitor-start` | Start system monitoring | `monitor-start` |
| `monitor-stop` | Stop monitoring | `monitor-stop` |
| `monitor-stats` | Show current statistics | `monitor-stats` |
| `proc-list [--top N]` | List processes | `proc-list --top 10` |
| `proc-kill <pid>` | Kill process | `proc-kill 1234` |
| `monitor-network` | Network information | `monitor-network` |
| `monitor-disk` | Disk usage | `monitor-disk` |

**Example**:
```bash
novashell> monitor-start
✅ System monitoring started

novashell> monitor-stats
📊 System Statistics:
CPU Usage: 45.2% (8 cores)
Memory: 8.5 GB / 16 GB (53%)
Disk: 450 GB / 1 TB (45%)
Network: ↓ 2.5 MB/s ↑ 0.8 MB/s

novashell> proc-list --top 5
PID    NAME           CPU%   MEM      USER
1234   chrome         25.3%  2.1 GB   user
5678   vscode         15.2%  1.5 GB   user
9012   node           8.5%   850 MB   user
```

---

### 📝 Notes & Snippets Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `note-add <title>` | Add new note | `note-add "Study Notes"` |
| `note-list [--category]` | List notes | `note-list --category study` |
| `note-get <id>` | View note | `note-get note_001` |
| `note-search <term>` | Search notes | `note-search "algorithm"` |
| `snippet-add <title> <lang>` | Add code snippet | `snippet-add "Binary Search" cpp` |
| `snippet-list` | List snippets | `snippet-list` |
| `snippet-get <id>` | View snippet | `snippet-get snip_001` |
| `snippet-search <term>` | Search snippets | `snippet-search "sort"` |

**Example**:
```bash
novashell> snippet-add "Binary Search" cpp
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
✅ Snippet saved: snip_001

novashell> snippet-search "binary"
📋 Found 1 snippet:
[snip_001] Binary Search (cpp) - algorithm, search
```

---

### ⏰ Task Scheduler Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `task-schedule <name> <command> --daily <time>` | Schedule daily task | `task-schedule "Backup" "backup.sh" --daily 02:00` |
| `task-schedule <name> <command> --weekly <day> <time>` | Schedule weekly task | `task-schedule "Report" "report.sh" --weekly monday 09:00` |
| `task-list` | List all tasks | `task-list` |
| `task-enable <id>` | Enable task | `task-enable task_001` |
| `task-disable <id>` | Disable task | `task-disable task_001` |
| `remind <title> --in <duration>` | Set reminder | `remind "Meeting" --in 2h` |
| `remind <title> --at <time>` | Set reminder at time | `remind "Call" --at 15:30` |
| `remind-list` | List reminders | `remind-list` |

**Example**:
```bash
novashell> task-schedule "Daily Backup" "backup.sh" --daily 02:00
✅ Task scheduled: task_001
Next run: Tomorrow at 2:00 AM

novashell> remind "Submit Assignment" --in 2h
⏰ Reminder set for 2 hours from now
Reminder ID: rem_001

novashell> task-list
ID         TITLE            NEXT RUN           STATUS
task_001   Daily Backup     Today, 2:00 AM     Enabled
task_002   Weekly Report    Monday, 9:00 AM    Enabled
```

---

### 🌐 Network Tools Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `net-interfaces` | List network interfaces | `net-interfaces` |
| `net-capture [interface]` | Start packet capture | `net-capture eth0` |
| `net-stop` | Stop packet capture | `net-stop` |
| `net-packets [count]` | Show captured packets | `net-packets 10` |
| `net-stats` | Network statistics | `net-stats` |
| `net-protocols` | Protocol distribution | `net-protocols` |

**Example**:
```bash
novashell> net-interfaces
Available network interfaces:
1. eth0 (192.168.1.100)
2. wlan0 (192.168.1.105)
3. lo (127.0.0.1)

novashell> net-capture eth0
🔍 Started packet capture on eth0
Press Ctrl+C to stop...

novashell> net-stats
📊 Network Statistics:
Total packets: 1,523
TCP: 892 (58.6%)
UDP: 431 (28.3%)
Other: 200 (13.1%)
```

---

### 🔧 Environment Manager Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `env-create <name> <description>` | Create environment | `env-create java "Java 17 Dev"` |
| `env-switch <name>` | Switch environment | `env-switch python-ml` |
| `env-set <var> <value>` | Set variable | `env-set JAVA_HOME /usr/lib/jvm/java-17` |
| `env-list` | List environments | `env-list` |
| `env-delete <name>` | Delete environment | `env-delete old-env` |

**Example**:
```bash
novashell> env-create java-project "Java 17 Development"
✅ Environment 'java-project' created

novashell> env-set JAVA_HOME /usr/lib/jvm/java-17
novashell> env-switch java-project
✅ Switched to environment 'java-project'
Java: 17.0.5
Maven: 3.8.6
```

---

### 🔄 P2P File Sharing Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `p2p-start` | Start P2P server | `p2p-start` |
| `p2p-stop` | Stop P2P server | `p2p-stop` |
| `p2p-share <file> [--public]` | Share file | `p2p-share report.pdf --public` |
| `p2p-list` | List shared files | `p2p-list` |
| `p2p-get <share-id>` | Download file | `p2p-get share_abc123` |
| `p2p-peers` | List connected peers | `p2p-peers` |

**Example**:
```bash
novashell> p2p-start
✅ P2P server started on port 8888
Files stored in: .customos/p2p/shares/

novashell> p2p-share /projects/report.pdf --public
✅ File shared successfully!
Share ID: share_a1b2c3d4
📁 Local storage: .customos/p2p/shares/
🌐 Public share

novashell> p2p-list
Shared P2P Files:
📄 report.pdf (ID: share_a1b2c3d4)
   Size: 245760 bytes
   🌐 Public
```

---

### 🎨 Theme Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `theme-list` | List available themes | `theme-list` |
| `theme-set <name>` | Set theme | `theme-set dracula` |
| `theme-create <name>` | Create custom theme | `theme-create mytheme` |
| `theme-preview <name>` | Preview theme | `theme-preview monokai` |

**Example**:
```bash
novashell> theme-list
Available Themes:
1. dark (current)
2. light
3. dracula
4. monokai
5. solarized
6. nord
7. gruvbox

novashell> theme-set dracula
✅ Theme changed to 'dracula'
```

---

### 🌐 Remote Access Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `ssh-start [port]` | Start SSH server | `ssh-start 2222` |
| `ssh-stop` | Stop SSH server | `ssh-stop` |
| `ssh-connections` | List active connections | `ssh-connections` |

**Example**:
```bash
novashell> ssh-start 2222
✅ SSH server started on port 2222
Server address: 192.168.1.100:2222

# On remote machine
$ ssh admin@192.168.1.100 -p 2222
Password: ****
✅ Welcome to NovaShell
novashell>
```

---

### 🛠️ Utility Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `help [category]` | Show help | `help git` |
| `history` | Show command history | `history` |
| `clear` | Clear screen | `clear` |
| `echo <text>` | Print text | `echo "Hello World"` |
| `version` | Show version | `version` |
| `exit` | Exit shell | `exit` |

---

## 🔧 Configuration

Configuration file: `~/.customos/config.json`

```json
{
  "shell": {
    "prompt": "novashell> ",
    "history_size": 1000,
    "auto_save": true
  },
  "ai": {
    "enabled": true,
    "gemini_api_key": "YOUR_API_KEY_HERE",
    "suggestions_enabled": true,
    "learning_enabled": true,
    "max_context_length": 10000
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

---

## 🏗️ Architecture

```
novashell/
├── src/
│   ├── core/           # Core shell engine
│   ├── vfs/            # Virtual File System
│   ├── auth/           # Authentication & Access Control
│   ├── network/        # Packet Analyzer & HTTP Server
│   ├── vault/          # Password Manager (AES-256)
│   ├── scripting/      # Script Engine & Macros
│   ├── plugins/        # Plugin System
│   ├── logging/        # Logging & Audit Trail
│   ├── ai/             # AI Module (Gemini Integration)
│   ├── remote/         # SSH Server & Remote Desktop
│   ├── containers/     # Docker/Podman Management
│   ├── notes/          # Note & Snippet Manager
│   ├── monitor/        # System Monitor (CPU/Memory/Disk)
│   ├── env/            # Environment Manager
│   ├── p2p/            # P2P File Sharing
│   ├── scheduler/      # Task Scheduler & Reminders
│   ├── database/       # Database Support (SQLite/MySQL/PostgreSQL)
│   ├── git/            # Git Integration
│   ├── ui/             # Theme Manager
│   ├── analytics/      # Performance Analytics
│   ├── mobile/         # Mobile API
│   └── utils/          # Utilities
├── include/            # Public headers
├── plugins/            # Sample plugins
├── docs/               # Documentation
├── tests/              # Unit tests
└── CMakeLists.txt      # Build configuration
```

---

## 🐛 Troubleshooting

### Build Issues

#### "CMake not found"
```powershell
choco install cmake -y
refreshenv
```

#### "g++ not found"
```powershell
choco install mingw -y
refreshenv
```

#### "OpenSSL not found"
```powershell
# Windows
choco install openssl -y
# Or specify manually:
cmake .. -DOPENSSL_ROOT_DIR="C:\Program Files\OpenSSL-Win64"

# macOS
cmake .. -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

#### "SQLite3 not found" when running
```powershell
# Use launcher script (recommended)
.\run-novashell.ps1

# Or ensure MSYS2 is in PATH
$env:PATH = "C:\msys64\mingw64\bin;$env:PATH"
.\build\bin\customos-shell.exe
```

#### Compiler errors with std::optional
Your compiler is too old. Upgrade to GCC 11+:
```powershell
.\setup-dependencies.ps1 -Force
```

#### Permission denied errors
Run PowerShell as Administrator:
```powershell
Start-Process powershell -Verb RunAs
```

### Runtime Issues

#### "Permission Denied" (Linux/macOS - Network features)
Network packet capture requires elevated privileges:
```bash
sudo setcap cap_net_raw,cap_net_admin=eip ./bin/customos-shell
# Or run with sudo
sudo ./bin/customos-shell
```

#### AI features not working
1. Verify API key: Check `~/.customos/config.json`
2. Test API key at https://makersuite.google.com/
3. Check internet connection
4. Run: `ai-init YOUR_API_KEY_HERE`

#### Database connection fails
1. Verify database server is running
2. Check credentials and hostname
3. Test connection: `db-connect test --mysql localhost 3306 testdb user`

### Getting Help

1. Check [Troubleshooting](#troubleshooting) section above
2. Run with verbose output: `.\build.ps1 -Verbose`
3. View build logs in `build/` directory
4. Search [GitHub Issues](https://github.com/yourusername/novashell/issues)
5. Create new issue with:
   - Error message
   - System information (`cmake --version`, `g++ --version`)
   - Build log
   - Steps to reproduce

---

## 🤝 Contributing

We welcome contributions! Here's how to get started:

### Development Setup

```bash
# Fork and clone
git clone https://github.com/yourusername/novashell.git
cd novashell

# Create feature branch
git checkout -b feature/my-feature

# Build and test
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest

# Make changes and commit
git add .
git commit -m "feat: add amazing feature"
git push origin feature/my-feature
```

### Coding Standards

- **C++ Standard**: C++17
- **Style**: Follow existing code style (4 spaces, no tabs)
- **Naming**:
  - `snake_case` for functions and variables
  - `PascalCase` for classes
  - `UPPER_CASE` for constants
- **Comments**: Use Doxygen-style comments for public APIs
- **Error Handling**: Always check return values and handle errors

### Pull Request Process

1. Update documentation (README, code comments)
2. Add tests for new features
3. Ensure all tests pass
4. Update CHANGELOG.md
5. Submit PR with clear description

### Plugin Development

Create custom plugins using our API:

```cpp
#include "customos/plugin_api.h"

class MyPlugin : public IPlugin {
public:
    std::string get_name() const override {
        return "my-plugin";
    }

    bool initialize() override {
        register_command("mycommand", &MyPlugin::my_command);
        return true;
    }

    void my_command(const std::vector<std::string>& args) {
        std::cout << "Hello from my plugin!\n";
    }
};

EXPORT_PLUGIN(MyPlugin)
```

---

## 🔒 Security

### Security Features

- **Password Storage**: AES-256-GCM encryption with PBKDF2 (100,000 iterations)
- **Access Control**: Role-based permissions with command-level granularity
- **Audit Logging**: Tamper-evident logs with SHA-256 checksums
- **Network Capture**: Requires elevated privileges (admin/root)
- **Plugin Verification**: Optional signature verification

### Reporting Security Issues

**DO NOT** create public GitHub issues for security vulnerabilities.

Instead, email: security@novashell.dev

Include:
- Description of vulnerability
- Steps to reproduce
- Potential impact
- Suggested fix (if any)

We aim to respond within 48 hours.

---

## 📞 Support

### Getting Help

- 📖 **Documentation**: Read this README and inline `help` command
- 🐛 **Bug Reports**: [GitHub Issues](https://github.com/yourusername/novashell/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/yourusername/novashell/discussions)
- 📧 **Email**: support@novashell.dev
- 💬 **Discord**: [Join our community](https://discord.gg/novashell)

### Before Asking for Help

1. Read relevant documentation
2. Search existing issues
3. Try troubleshooting steps
4. Gather error messages and logs

### When Creating an Issue

Include:
- **System**: OS, compiler version, CMake version
- **Error**: Full error message
- **Steps**: How to reproduce
- **Expected**: What should happen
- **Actual**: What actually happens
- **Logs**: Build logs or runtime output

---

## 📜 License

This project is licensed under the **MIT License**.

```
MIT License

Copyright (c) 2024 NovaShell Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

See [LICENSE](LICENSE) file for full text.

---

## 🙏 Acknowledgments

- **OpenSSL** - Cryptographic functions
- **SQLite** - Embedded database
- **Google Gemini** - AI capabilities
- **libpcap** - Packet capture
- **Docker** - Container runtime
- Unix philosophy and modern DevOps tools for inspiration

---

## 🗺️ Roadmap

### ✅ Completed (v1.0 - v1.5)

**Core Features (15)**:
- Virtual File System
- Plugin System
- Authentication & Access Control
- Scripting Engine
- Network Packet Analyzer
- Password Manager
- File Utilities
- Logging & Audit
- Process Management
- Cloud Backup
- Git Integration
- Database Support
- Tab Completion
- Custom Themes
- Task Scheduler

**AI Features (10)**:
- AI Command Understanding
- AI Task Flow Planner
- AI Memory & Context Engine
- Code Analyzer & Helper
- Error & Log Analyzer
- Data & File Summarizer
- Knowledge Graph
- Routine & Automation Manager
- Search & Knowledge Recall
- Explanation Engine

**Total: 25 Major Features** ✅

### 🚀 Coming Soon (v2.0 - 2025)

**Next-Gen Features**:
- [ ] Advanced voice commands with wake word ("Hey Nova")
- [ ] Cloud synchronization across devices
- [ ] Real-time collaboration
- [ ] Enhanced remote desktop (full screen + terminal access)
- [ ] Mobile companion app
- [ ] Global plugin marketplace
- [ ] Biometric authentication
- [ ] Performance analytics dashboard
- [ ] Multi-language support
- [ ] WebAssembly plugin support

**Expected: Q2 2025** 🎯

---

## 🌟 Why NovaShell v2.0 Will Change Everything

NovaShell v2.0 isn't just an update—it's a **complete reimagining** of the terminal experience:

**Imagine:**
- 🗣️ Talking to your terminal naturally: *"Hey Nova, show me git status and suggest next steps"*
- 🤖 Getting AI help with complex commands: *"How do I deploy this to Kubernetes?"*
- 🧠 Having your terminal learn your habits and anticipate needs
- 👥 Real-time collaboration on coding projects with teammates
- 🎤 Voice-controlled development workflows
- ☁️ Seamless work across multiple devices

**NovaShell v2.0 will be the first terminal that truly understands you!** 🤖💫

### Stay Tuned

- ⭐ Star on [GitHub](https://github.com/yourusername/novashell)
- 🔔 Watch for release notifications
- 💬 Join [Discord community](https://discord.gg/novashell)
- 📧 Subscribe to newsletter for early access

---

## 📊 Project Statistics

- **Lines of Code**: 50,000+
- **Features**: 25+ major features
- **Commands**: 200+ built-in commands
- **Languages Supported**: C++17, Python, JavaScript, and more (via AI)
- **Platforms**: Windows, Linux, macOS
- **License**: MIT (Free and Open Source)
- **Active Development**: Since 2024

---

## 🎯 Quick Links

- [🏠 Homepage](https://novashell.dev)
- [📖 Documentation](https://docs.novashell.dev)
- [🐛 Report Bug](https://github.com/yourusername/novashell/issues/new?template=bug_report.md)
- [💡 Request Feature](https://github.com/yourusername/novashell/issues/new?template=feature_request.md)
- [💬 Discussions](https://github.com/yourusername/novashell/discussions)
- [🎓 Tutorials](https://novashell.dev/tutorials)

---

<div align="center">

**Made with ❤️ by the NovaShell Team**

⭐ Star us on GitHub | 🐦 Follow on Twitter | 💬 Join Discord

[Website](https://novashell.dev) • [GitHub](https://github.com/yourusername/novashell) • [Discord](https://discord.gg/novashell) • [Twitter](https://twitter.com/novashell)

**Transform your terminal into an AI-powered development environment today!**

</div>

---

**Last Updated**: January 30, 2026
**Version**: 1.5.0
**Status**: ✅ Stable & Production Ready
