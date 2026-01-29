nce Guide

A comprehensive guide to all NovaShell commands, organized by category for easy navigation.

## 📋 Table of Contents

Choose a category to learn more:

1. **[🔐 Authentication](#-authentication)** - User login and account management
2. **[🔑 Password Vault](#-password-vault)** - Secure password storage and management
3. **[🔀 Git Integration](#-git-integration)** - Version control operations
4. **[🌐 Network Tools](#-network-tools)** - Packet analysis and monitoring
5. **[🐳 Container Management](#-container-management)** - Docker/Podman operations
6. **[📊 System Monitoring](#-system-monitoring)** - Real-time system metrics
7. **[📝 Notes & Snippets](#-notes--snippets)** - Knowledge management
8. **[⏰ Task Scheduling](#-task-scheduling)** - Automated task management
9. **[🤖 AI Features](#-ai-features)** - Intelligent command suggestions
10. **[🌐 Remote Access](#-remote-access)** - SSH server management
11. **[🎨 Themes](#-themes)** - Visual customization
12. **[🎤 Voice Commands](#-voice-commands)** - Speech-to-text interface
13. **[📊 Analytics](#-analytics)** - Dashboard and metrics
14. **[🔧 Environment Manager](#-environment-manager)** - Development environment switching
15. **[🛠️ File System](#-file-system)** - File and directory operations
16. **[🏗️ Virtual File System](#-virtual-file-system)** - Cross-platform file management
17. **[💾 Database](#-database)** - SQL database operations
18. **[📦 P2P Sharing](#-p2p-sharing)** - LAN file sharing
19. **[🔌 Plugins](#-plugins)** - Extension system
20. **[⚙️ Scripting](#-scripting)** - Script execution
21. **[📋 Logging](#-logging)** - System logs and audit trails
22. **[🛠️ Utilities](#-utilities)** - General-purpose commands

---

## 🔐 Authentication

**Purpose**: User account management and secure login system.

### Commands:

#### `login <username>`
**Description**: Authenticate as a user to access NovaShell features.
**Usage**: `login admin`
**Example**:
```bash
novashell> login admin
Password for admin: ****
Login successful! Welcome, admin.
```
**Notes**: Most NovaShell features require authentication. Default admin account: username `admin`, password `admin`.

#### `logout`
**Description**: End current user session.
**Usage**: `logout`
**Example**:
```bash
novashell> logout
Logged out successfully. Goodbye, admin!
```

#### `create-user <username> <password>`
**Description**: Create a new user account.
**Usage**: `create-user john mypassword123`
**Example**:
```bash
novashell> create-user john mypassword123
User 'john' created successfully!
You can now login with: login john
```

#### `whoami`
**Description**: Display current logged-in user.
**Usage**: `whoami`
**Example**:
```bash
novashell> whoami
admin
```

---

## 🔑 Password Vault

**Purpose**: Secure encrypted password storage and management.

### Commands:

#### `vault-init`
**Description**: Initialize password vault with master password.
**Usage**: `vault-init`
**Example**:
```bash
novashell> vault-init
Choose a strong master password: ****
Confirm master password: ****
Vault initialized successfully!
```

#### `vault-unlock`
**Description**: Unlock vault for password operations.
**Usage**: `vault-unlock`
**Example**:
```bash
novashell> vault-unlock
Enter master password: ****
Vault unlocked successfully!
```

#### `vault-lock`
**Description**: Lock vault to prevent access.
**Usage**: `vault-lock`
**Example**:
```bash
novashell> vault-lock
Vault locked.
```

#### `vault-add`
**Description**: Add a new password entry.
**Usage**: `vault-add`
**Interactive Example**:
```bash
novashell> vault-add
Service name: github.com
Username: myuser
Password: ****
URL (optional): https://github.com
Notes (optional): Personal account
Password added successfully!
```

#### `vault-list`
**Description**: List all stored passwords.
**Usage**: `vault-list`
**Example**:
```bash
novashell> vault-list
Stored passwords:
================
1. github.com (myuser)
2. gmail.com (user@gmail.com)
3. bank.com (account123)
```

#### `vault-get <service>`
**Description**: Retrieve password for a specific service.
**Usage**: `vault-get github.com`
**Example**:
```bash
novashell> vault-get github.com
Service: github.com
Username: myuser
Password: mypassword123
URL: https://github.com
Notes: Personal account
```

#### `vault-delete <service>`
**Description**: Remove a password entry.
**Usage**: `vault-delete github.com`
**Example**:
```bash
novashell> vault-delete github.com
Password deleted successfully!
```

#### `vault-search <query>`
**Description**: Search passwords by service name or username.
**Usage**: `vault-search github`
**Example**:
```bash
novashell> vault-search git
Search results for 'git':
==================================
1. github.com (myuser)
2. gitlab.com (user@company.com)
```

---

## 🔀 Git Integration

**Purpose**: Complete Git version control workflow.

### Commands:

#### `git-status`
**Description**: Show current repository status.
**Usage**: `git-status`
**Example**:
```bash
novashell> git-status
Git status:
  modified: main.cpp
  untracked: temp.txt
```

#### `git-add <file> [--all]`
**Description**: Stage files for commit.
**Usage**: `git-add main.cpp` or `git-add --all`
**Examples**:
```bash
novashell> git-add main.cpp
Added main.cpp to staging area.

novashell> git-add --all
Added all files to staging area.
```

#### `git-commit <message>`
**Description**: Commit staged changes.
**Usage**: `git-commit "Fix login bug"`
**Example**:
```bash
novashell> git-commit "Fix login bug"
Changes committed successfully.
```

#### `git-log [limit]`
**Description**: Show commit history.
**Usage**: `git-log` or `git-log 20`
**Example**:
```bash
novashell> git-log 5
Commit: abc1234
Author: John Doe <john@example.com>
Date: Mon Nov 11 10:30:45 2025
Message: Fix login bug

Commit: def5678
Author: Jane Smith <jane@example.com>
Date: Mon Nov 11 09:15:22 2025
Message: Add new feature
```

#### `git-branch [name]`
**Description**: List branches or create new branch.
**Usage**: `git-branch` or `git-branch feature-x`
**Examples**:
```bash
novashell> git-branch
* main
  develop
  feature/login

novashell> git-branch hotfix
Branch 'hotfix' created.
```

#### `git-checkout <branch>`
**Description**: Switch to different branch.
**Usage**: `git-checkout develop`
**Example**:
```bash
novashell> git-checkout develop
Switched to develop.
```

#### `git-pull [remote] [branch]`
**Description**: Fetch and merge from remote repository.
**Usage**: `git-pull` or `git-pull origin main`
**Example**:
```bash
novashell> git-pull
Pull completed successfully.
```

#### `git-push [remote] [branch]`
**Description**: Push commits to remote repository.
**Usage**: `git-push` or `git-push origin main`
**Example**:
```bash
novashell> git-push
Push completed successfully.
```

---

## 🛠️ Utilities

**Purpose**: General-purpose utility commands.

### Commands:

#### `help [category|command]`
**Description**: Show help for categories or specific commands.
**Usage**: `help`, `help authentication`, or `help login`
**Examples**:
```bash
novashell> help
Available Categories:
1. 🔐 Authentication - User login and account management
2. 🔑 Password Vault - Secure password storage
3. 🔀 Git Integration - Version control operations
... (and more)

Choose a category number or type 'help <command>' for details.

novashell> help authentication
🔐 Authentication Commands:
===========================

login <username> - Authenticate as a user
logout - End current session
create-user <username> <password> - Create new account
whoami - Show current user

Type 'help <command>' for detailed usage.

novashell> help login
Command: login
Description: Login as a user
Usage: login <username>
Examples:
  login admin
  login john

Notes: Use 'admin'/'admin' for first login
```

#### `version`
**Description**: Show NovaShell version information.
**Usage**: `version`
**Example**:
```bash
novashell> version
NovaShell v1.0.0
Build date: Nov 11 2025 14:30:15
```

#### `echo <text>`
**Description**: Display text or variables.
**Usage**: `echo Hello World`
**Example**:
```bash
novashell> echo Current user session active
Current user session active
```

---

## 🤖 AI Features

**Purpose**: Revolutionary AI-powered assistance for development, debugging, and productivity.

### Commands:

#### `ai-init <gemini_api_key>`
**Description**: Initialize AI features with Google Gemini API key for all AI functionality.
**Usage**: `ai-init YOUR_API_KEY`
**Prerequisites**: Get your free API key from https://makersuite.google.com/app/apikey
**Example**:
```bash
novashell> ai-init AIzaSyXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
✅ AI initialized successfully!
API key stored securely.
You can now use all AI features:
  ai-interpret, ai-plan, ai-context, ai-analyze, ai-explain
```
**Notes**: API key is stored encrypted in your local database. Required for all AI features.

#### `ai-interpret <natural_text>`
**Description**: Convert natural language requests into shell commands using AI understanding.
**Usage**: `ai-interpret <description of what you want to do>`
**Examples**:
```bash
novashell> ai-interpret remind me to commit nightly at 9PM
🤖 AI Interpretation:
"remind me to commit nightly at 9PM"
→ Generated commands:
  1. task-schedule "Commit Reminder" "git add .; git commit -m 'daily commit'" --daily 21:00

💡 Execute with: task-schedule "Commit Reminder" "git add .; git commit -m 'daily commit'" --daily 21:00
```

#### `ai-plan <goal>`
**Description**: Create detailed multi-step automation plans from high-level goals.
**Usage**: `ai-plan <goal description>`
**Examples**:
```bash
novashell> ai-plan prepare my workspace for deployment
📋 AI Task Plan:
"prepare my workspace for deployment"

1. git status           # Check current state
2. git pull            # Get latest changes
3. build-project       # Compile/build project
4. run-tests          # Execute test suite
5. deploy-app         # Deploy to production
6. report-status      # Send deployment report

🚀 Execute plan with: ai-execute-plan "prepare my workspace for deployment"
```

#### `ai-context [query]`
**Description**: Access AI memory and context about your recent activities and workflow.
**Usage**: `ai-context` (show recent context) or `ai-context <query>` (search context)
**Examples**:
```bash
novashell> ai-context
🧠 AI Context Memory:
====================
Recent activity:
- git commit -m "Update authentication"
- vault-get github.com
- ai-analyze main.cpp
- task-schedule "backup" "backup.sh" --daily

novashell> ai-context git
🔍 Context Search Results:
"git"
1. git commit -m "Update authentication"
2. git status
3. git add .
```

#### `ai-analyze <filepath> [task]`
**Description**: Comprehensive AI code analysis with professional insights.
**Usage**: `ai-analyze <file> [analyze|explain|improve]`
**Tasks**:
- `analyze` (default): Full code review with issues and recommendations
- `explain`: Detailed code explanation and documentation
- `improve`: Suggest improvements and optimizations
**Examples**:
```bash
novashell> ai-analyze main.cpp
🔍 AI Code Analysis: main.cpp
===================
File: main.cpp
Language: C++
Lines: 245
Complexity: Medium
Overall Score: B+

📊 Issues Found:
⚠️  Line 67: Potential memory leak in process_data()
   💡 Consider using smart pointers or RAII pattern

✅ Strengths:
✓ Good error handling patterns
✓ Clear function naming
✓ Proper resource management

💡 Recommendations:
• Add input validation for file operations
• Consider using parallel processing for large datasets
```

#### `ai-edit <filepath> <operation> [target] [options]`
**Description**: AI-powered code editing and refactoring with intelligent suggestions.
**Usage**: `ai-edit <file> <operation> [target] [options]`
**Operations**:
- `refactor`: Extract functions, rename variables, improve structure
- `optimize`: Performance improvements and algorithm optimization
- `fix`: Bug fixes and error corrections
- `improve`: General code quality enhancements
- `document`: Add comments and documentation
- `secure`: Security hardening and vulnerability fixes
**Examples**:
```bash
novashell> ai-edit Calculator.java refactor --extract-method="calculateTax"
🔧 AI Refactoring: Calculator.java
===============================

✅ Successfully extracted method calculateTax()
📝 Changes made:
• Created new calculateTax() method
• Reduced main method complexity by 40%
• Improved code readability and maintainability
```

#### `ai-debug <error_message> [code_file] [language]`
**Description**: Advanced AI debugging with root cause analysis and fix suggestions.
**Usage**: `ai-debug <error description> [file] [language]`
**Examples**:
```bash
novashell> ai-debug "segmentation fault in main.cpp at line 42"
🐛 AI Debugging Analysis
========================

🎯 Root Cause: Null pointer dereference in memory allocation

📖 Detailed Explanation:
The segmentation fault occurs because the pointer returned by malloc()
is not checked for NULL before dereference. On systems with low memory,
malloc() can return NULL, causing the crash.

🔧 Fixed Code:
```cpp
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
```

💡 Alternative Solutions:
1. Use calloc() instead of malloc()
2. Implement custom memory allocator
3. Add memory pool management

🛡️ Prevention Tips:
• Always check malloc/calloc return values
• Use smart pointers in C++
• Implement proper error handling
```

#### `ai-project <type> <name> <language> [framework]`
**Description**: Generate complete project structures with AI assistance.
**Usage**: `ai-project <type> <name> <language> [framework]`
**Types**: web, api, cli, lib, desktop, mobile
**Examples**:
```bash
novashell> ai-project web e-commerce-app javascript react express mongodb
🏗️ AI Project Generator
=======================

Creating web project: e-commerce-app
Tech Stack: React + Node.js + PostgreSQL

📁 Generated Project Structure:
```
e-commerce-app/
├── frontend/
│   ├── components/
│   ├── pages/
│   ├── hooks/
│   └── App.jsx
├── backend/
│   ├── controllers/
│   ├── models/
│   ├── routes/
│   └── app.js
├── database/
│   ├── migrations/
│   └── seeds/
├── tests/
│   ├── unit/
│   ├── integration/
│   └── e2e/
```
```

#### `ai-review <filepath> [focus_area]`
**Description**: Professional AI code review with detailed feedback and scoring.
**Usage**: `ai-review <file> [security|performance|maintainability|all]`
**Focus Areas**:
- `security`: Security vulnerabilities and hardening
- `performance`: Performance bottlenecks and optimizations
- `maintainability`: Code structure and readability
- `all` (default): Complete review
**Examples**:
```bash
novashell> ai-review authentication.py security
🔍 AI Code Review: authentication.py
==================================

📊 Overall Rating: Needs Attention

🚨 Critical Security Issues:
• SQL injection vulnerability in login query (Line 45)
• Weak password hashing using MD5 (Line 23)
• Session tokens not properly invalidated (Line 78)

💡 Security Recommendations:
• Use parameterized queries or ORM
• Implement bcrypt or Argon2 for password hashing
• Add session timeout and proper cleanup
```

#### `ai-test <filepath> [test_framework] [test_types]`
**Description**: Generate comprehensive test suites for your code.
**Usage**: `ai-test <file> [framework] [types]`
**Frameworks**: gtest (cpp), pytest (python), jest (js), junit (java)
**Test Types**: unit, integration, edge_cases, performance
**Examples**:
```bash
novashell> ai-test Calculator.java junit "unit,integration,edge_cases"
🧪 AI Test Generation: Calculator.java
====================================

Framework: JUnit 5
Test Types: Unit tests, Integration tests, Edge cases

📝 Generated Test Suite:
```java
class CalculatorTest {
    @Test
    void testAddition() {
        assertEquals(5, calculator.add(2, 3));
    }

    @Test
    void testDivisionByZero() {
        assertThrows(ArithmeticException.class, () -> {
            calculator.divide(5, 0);
        });
    }
}
```
```

#### `ai-generate <type> <language> <description> [options]`
**Description**: Generate code snippets, functions, classes, and boilerplate code.
**Usage**: `ai-generate <type> <language> <description> [params]`
**Types**: function, class, module, test, boilerplate, project
**Examples**:
```bash
novashell> ai-generate function cpp "implement binary search with recursion"
🤖 Generating function in cpp...
Description: implement binary search with recursion

```cpp
int binarySearch(const std::vector<int>& arr, int target, int left, int right) {
    if (left > right) {
        return -1; // Element not found
    }

    int mid = left + (right - left) / 2;

    if (arr[mid] == target) {
        return mid; // Element found
    }
    else if (arr[mid] > target) {
        return binarySearch(arr, target, left, mid - 1);
    }
    else {
        return binarySearch(arr, target, mid + 1, right);
    }
}
```
```

#### `ai-help <question> [code_file] [skill_level]`
**Description**: Interactive AI coding assistance and tutoring with personalized responses.
**Usage**: `ai-help <question> [file] [level]`
**Skill Levels**: 1-5 (1=beginner, 5=expert)
**Examples**:
```bash
novashell> ai-help "how to implement a thread-safe singleton in C++" --skill-level=3
🤖 AI Coding Assistant
=====================

Question: How to implement a thread-safe singleton in C++?
Skill Level: Intermediate (3/5)

💬 Answer:
A thread-safe singleton ensures only one instance exists across multiple threads.

1. **Meyers' Singleton (C++11)**:
```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }
    // Delete copy/move operations
private:
    Singleton() = default;
};
```
```

#### `ai-completion-stats`
**Description**: Show AI completion learning statistics and how suggestions work.
**Usage**: `ai-completion-stats`
**Example**:
```bash
novashell> ai-completion-stats
🤖 AI Completion Learning Statistics
====================================

📊 Command Usage Patterns:
==========================
  git-status: 145 times
  vault-get: 89 times
  ai-analyze: 76 times

💡 How AI Completion Works:
===========================
• 🤖 AI Suggestions: Powered by Google Gemini for intelligent predictions
• 📊 Learned Patterns: Based on your command history and habits
• 🔄 Workflow Sequences: Learns common command chains
• ⏰ Time-Based: Suggests commands you typically use at certain times
• 📈 Adaptive: Continuously improves as you use NovaShell
```

#### `ai-explain <concept_or_command>`
**Description**: Explain technical concepts, commands, and code patterns.
**Usage**: `ai-explain <topic>`
**Examples**:
```bash
novashell> ai-explain "linker vs loader"
📚 AI Explanation:
"linker vs loader"

💬 Linker vs Loader Explanation:

**Linker**:
- Runs during compilation/linking phase
- Combines object files and libraries into executable
- Resolves external references between modules
- Creates final executable with absolute addresses

**Loader**:
- Runs when program starts executing
- Loads executable into memory
- Allocates memory segments (text, data, bss, heap, stack)
- Relocates addresses for actual memory locations
- Initializes program for execution

**Key Differences**:
- Linker: Compile-time, creates executable
- Loader: Runtime, prepares for execution
- Linker: Static linking decisions
- Loader: Dynamic memory management
```

### AI Features Setup:
1. Get Gemini API key: https://makersuite.google.com/app/apikey
2. Initialize: `ai-init YOUR_API_KEY`
3. Start using AI commands!

### AI Command Categories:
- **🧠 Understanding**: `ai-interpret`, `ai-explain`
- **📋 Planning**: `ai-plan`, `ai-project`
- **🔧 Development**: `ai-generate`, `ai-edit`, `ai-test`
- **🐛 Debugging**: `ai-debug`, `ai-analyze`
- **📊 Quality**: `ai-review`, `ai-context`
- **🎓 Learning**: `ai-help`, `ai-explain`

---

**Purpose**: Real-time dashboards and metrics for system monitoring and usage analytics.

### Commands:

#### `dashboard [name]`
**Description**: Display analytics dashboard with widgets and charts.
**Usage**: `dashboard` or `dashboard commands`
**Examples**:
```bash
novashell> dashboard
📊 system Dashboard
====================

📈 CPU Usage (%)
─────────────────

Recent data:
  Mon Nov 11 14:30:00 2025  Value: 45.2 %
  Mon Nov 11 14:25:00 2025  Value: 38.7 %
  Mon Nov 11 14:20:00 2025  Value: 52.1 %

📈 Memory Usage (%)
───────────────────

Recent data:
  Mon Nov 11 14:30:00 2025  Value: 67.8 %
  Mon Nov 11 14:25:00 2025  Value: 63.2 %
  Mon Nov 11 14:20:00 2025  Value: 71.5 %

novashell> dashboard commands
📊 commands Dashboard
======================

📈 Command Usage
────────────────

Recent data:
  Mon Nov 11 14:30:00 2025  Value: 120 count
  Mon Nov 11 14:25:00 2025  Value: 98 count
  Mon Nov 11 14:20:00 2025  Value: 145 count
```

#### `analytics [metric]`
**Description**: Show analytics metrics or overview of command usage.
**Usage**: `analytics` or `analytics cpu` or `analytics memory`
**Examples**:
```bash
novashell> analytics
📊 Command Analytics (Top 5):
=============================
  help: 145 times
  git-status: 89 times
  vault-list: 76 times
  monitor-cpu: 65 times
  note-list: 52 times

novashell> analytics cpu
📊 CPU Usage History (Last Hour):
==================================
  Mon Nov 11 14:30:00 2025  CPU: 45.2%
  Mon Nov 11 14:25:00 2025  CPU: 38.7%
  Mon Nov 11 14:20:00 2025  CPU: 52.1%

novashell> analytics memory
📊 Memory Usage History (Last Hour):
=====================================
  Mon Nov 11 14:30:00 2025  Memory: 67.8%
  Mon Nov 11 14:25:00 2025  Memory: 63.2%
  Mon Nov 11 14:20:00 2025  Memory: 71.5%
```

---

## 🌐 Remote Access

**Purpose**: Remote desktop access with terminal switching and full screen control.

### Commands:

#### `remote-desktop-terminals`
**Description**: List available terminal sessions for remote access.
**Usage**: `remote-desktop-terminals`
**Example**:
```bash
novashell> remote-desktop-terminals
Available Terminal Sessions:
===========================
🖥️  Administrator: Command Prompt
   ID: term_12345678
   Type: cmd
   Process: 8765
   Active: Yes
   Bounds: 100,100 -> 800,600

🖥️  Administrator: Windows PowerShell
   ID: term_87654321
   Type: powershell
   Process: 4321
   Active: No
   Bounds: 200,200 -> 1000,700
```

#### `remote-desktop-switch <session_id>`
**Description**: Switch focus to a specific terminal session.
**Usage**: `remote-desktop-switch term_12345678`
**Example**:
```bash
novashell> remote-desktop-switch term_12345678
✅ Switched to terminal session: term_12345678
```

#### `remote-desktop-capture-terminal <session_id>`
**Description**: Capture screen of specific terminal session.
**Usage**: `remote-desktop-capture-terminal term_87654321`
**Example**:
```bash
novashell> remote-desktop-capture-terminal term_87654321
✅ Captured terminal session: term_87654321
📐 Resolution: 800x600
📊 Data size: 1920000 bytes
🖼️  Full screen: No
```

#### `remote-desktop-displays`
**Description**: List available displays/monitors.
**Usage**: `remote-desktop-displays`
**Example**:
```bash
novashell> remote-desktop-displays
Available Displays:
===================
1. \\.\DISPLAY1 (1920x1080)
2. \\.\DISPLAY2 (2560x1440)
```

#### `remote-desktop-fullscreen`
**Description**: Capture full desktop across all monitors.
**Usage**: `remote-desktop-fullscreen`
**Example**:
```bash
novashell> remote-desktop-fullscreen
✅ Captured full desktop
📐 Resolution: 3840x2160
📊 Data size: 16588800 bytes
🖼️  Full screen: Yes
🖥️  Display: All displays

💡 This capture includes:
   • All monitors in multi-monitor setups
   • All visible windows and applications
   • Desktop wallpaper and icons
   • Taskbar and system UI elements
```

#### `remote-desktop-set-display <display_name>`
**Description**: Set active display for remote desktop capture.
**Usage**: `remote-desktop-set-display \\.\DISPLAY2`
**Example**:
```bash
novashell> remote-desktop-set-display \\.\DISPLAY2
✅ Set active display to: \\.\DISPLAY2
📺 Remote desktop will now focus on this display.
```

#### `remote-desktop-windows`
**Description**: List all visible windows for remote access.
**Usage**: `remote-desktop-windows`
**Example**:
```bash
novashell> remote-desktop-windows
Visible Windows:
================
1. NovaShell - AI Terminal
   Handle: 0x12345678

2. Visual Studio Code
   Handle: 0x87654321
```

---

## 📦 P2P Sharing

**Purpose**: LAN file sharing with local storage and database metadata.

#### Commands:

#### `p2p-start [port]`
**Description**: Start P2P file sharing server.
**Usage**: `p2p-start` or `p2p-start 8888`
**Example**:
```bash
novashell> p2p-start 8888
P2P file sharing server started on port 8888
Files will be stored locally in: .customos/p2p/shares/
Only metadata is stored in the database.
```

#### `p2p-share <filepath> [--public]`
**Description**: Share a file via P2P (stored locally, metadata in DB).
**Usage**: `p2p-share /path/to/file.pdf` or `p2p-share file.txt --public`
**Example**:
```bash
novashell> p2p-share important_document.pdf --public
✅ File shared successfully!
Share ID: share_a1b2c3d4
File: important_document.pdf
📁 Stored locally in: .customos/p2p/shares/
💾 Only metadata stored in database
🌐 Public share
```

#### `p2p-list`
**Description**: List shared P2P files (from database metadata).
**Usage**: `p2p-list`
**Example**:
```bash
novashell> p2p-list
Shared P2P Files:
=================
📄 important_document.pdf (ID: share_a1b2c3d4)
   Size: 245760 bytes
   Checksum: a1b2c3d4... (truncated)
   Local Path: .customos/p2p/shares/share_a1b2c3d4.pdf
   🌐 Public

💡 Files are stored locally on your PC, not in the database!
   Database only contains metadata for efficient sharing.
```

#### `p2p-unshare <share_id>`
**Description**: Remove a shared P2P file (deletes from local storage and DB).
**Usage**: `p2p-unshare share_a1b2c3d4`
**Example**:
```bash
novashell> p2p-unshare share_a1b2c3d4
✅ File unshared successfully!
File removed from local storage and database metadata.
```

---

## 🎯 Quick Reference

### Most Used Commands:
- `help` - Get help and command categories
- `login admin` - Login (password: admin)
- `vault-add` - Store passwords securely
- `git-status` - Check repository status
- `monitor-cpu` - Check system performance
- `docker-ps` - List containers

### Getting Started Workflow:
1. `help` - Explore available categories
2. `login admin` - Authenticate
3. `vault-init` - Setup password vault
4. `ai-init YOUR_API_KEY` - Enable AI features
5. `theme-set dark` - Customize appearance

### Power User Commands:
- `ai-suggest` - Get AI command recommendations
- `task-schedule` - Automate repetitive tasks
- `p2p-share` - Share files locally
- `ssh-start` - Enable remote access
- `dashboard` - View real-time analytics
- `analytics` - Monitor system metrics

---

**💡 Tip**: Use `help <category>` to dive deep into any feature area, or `help <command>` for specific command details. NovaShell's extensive help system ensures you always know what's possible!

**🚀 Happy exploring! NovaShell has everything you need for modern development and system administration.**
