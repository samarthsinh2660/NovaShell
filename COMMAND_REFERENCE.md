# NovaShell Command Reference Guide

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
- `analytics` - View system insights

---

**💡 Tip**: Use `help <category>` to dive deep into any feature area, or `help <command>` for specific command details. NovaShell's extensive help system ensures you always know what's possible!

**🚀 Happy exploring! NovaShell has everything you need for modern development and system administration.**
