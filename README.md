# NovaShell

Your AI Terminal Assistant for Life and Code

A powerful, extensible command-line shell built in C++ with 15 core features plus **10 revolutionary AI capabilities** in v1.5. NovaShell v1.5 introduces intelligent command understanding, AI task planning, code analysis, and natural language processing - transforming your terminal into an AI-powered assistant.

## 🌟 NovaShell v1.5 - The AI Terminal Revolution 🤖

### **What's New in v1.5: 10 AI-Powered Features**

| AI Feature | Description | Commands | Example |
|------------|-------------|----------|---------|
| 🧠 **Natural Language** | Convert plain English to shell commands | `ai-interpret` | `"remind me to commit nightly"` → `task-schedule` |
| 📋 **Task Planning** | AI generates multi-step automation plans | `ai-plan` | `"prepare deployment"` → git pull → build → test → deploy |
| 🧠 **Memory & Context** | Remembers your workflow and command history | `ai-context` | `"what was I working on?"` → recalls project context |
| 💻 **Code Generation** | Generate functions, classes, boilerplate code | `ai-generate` | `"create binary search function in C++"` |
| 🔧 **Code Editing** | AI refactoring and code improvements | `ai-edit` | `"optimize performance"` or `"add error handling"` |
| 🐛 **Debugging Assistant** | Analyze errors and provide solutions | `ai-debug` | `"segmentation fault in main.cpp"` → root cause + fix |
| 📄 **Code Analysis** | Comprehensive code review and analysis | `ai-analyze` | `"analyze main.cpp"` → bugs, improvements, explanations |
| 🔍 **Code Review** | Professional code review with detailed feedback | `ai-review` | `"review security"` → vulnerabilities, best practices |
| 🧪 **Test Generation** | Generate comprehensive test suites | `ai-test` | `"generate unit tests for calculator"` |
| 🎓 **Coding Tutor** | Interactive coding assistance and explanations | `ai-help` | `"how to implement merge sort?"` → step-by-step guidance |

**🎉 v1.5 delivers 25 total features: 15 core + 10 AI innovations!**

### **🤖 AI Coding Assistant - Advanced Features**

#### **Code Generation & Creation**
- **Generate Functions**: `ai-generate function cpp "implement binary search with recursion"`
- **Create Classes**: `ai-generate class python "database connection manager with pooling"`
- **Build Modules**: `ai-generate module javascript "authentication middleware"`
- **Write Tests**: `ai-generate test cpp "comprehensive unit tests for calculator class"`
- **Project Boilerplate**: `ai-generate project web "React dashboard with TypeScript"`

#### **Intelligent Code Editing**
- **Refactoring**: `ai-edit main.cpp refactor "extract function from long method"`
- **Optimization**: `ai-edit utils.py optimize performance --target="slow_function"`
- **Security**: `ai-edit app.js secure --add="input validation"`
- **Documentation**: `ai-edit api.cpp document --format="doxygen"`
- **Modernization**: `ai-edit legacy.c improve --modern-cpp`

#### **Professional Debugging**
- **Error Analysis**: `ai-debug "null pointer exception" main.java`
- **Crash Diagnosis**: `ai-debug "segmentation fault at line 42" program.c`
- **Build Error Help**: `ai-debug "undefined reference to 'function'" --context="linking error"`
- **Performance Issues**: `ai-debug "memory leak detected" --valgrind-output`

#### **Code Review & Quality**
- **Security Audit**: `ai-review auth.cpp security` → SQL injection, XSS vulnerabilities
- **Performance Review**: `ai-review algorithm.py performance` → bottlenecks, optimizations
- **Maintainability**: `ai-review large_file.cpp maintainability` → complexity metrics, suggestions
- **Best Practices**: `ai-review new_feature.js` → coding standards, patterns

#### **Educational Assistance**
- **Concept Explanations**: `ai-explain "what is polymorphism?"`
- **Code Comprehension**: `ai-explain main.cpp` → detailed breakdown of what code does
- **Algorithm Help**: `ai-help "implement quicksort with generics" --skill-level=3`
- **Debugging Guidance**: `ai-help "why is my recursion causing stack overflow?" factorial.py`

#### **Project & Structure Management**
- **Project Generation**: `ai-project web blog-app javascript react express mongodb`
- **Structure Analysis**: `ai-analyze --structure large_codebase/`
- **Dependency Management**: `ai-help "how to manage circular dependencies?"`
- **Architecture Review**: `ai-review --architecture system_design/`

---

## 📋 Detailed AI Features

### 🤖 **AI Command Understanding (Natural Language → Actions)**
**You type what you mean — Nova does what you meant.**

Nova interprets plain English commands and converts them into structured CLI commands.

```bash
# Examples of natural language processing
nova> remind me to commit my project every night at 9PM
→ Translates automatically into:
task-schedule "Commit Reminder" "git add .; git commit -m 'daily commit'" --daily 21:00

nova> backup my important files to the cloud
→ Translates to:
tar -czf backup.tar.gz ~/important/ && sync-push backup.tar.gz

nova> show me all running processes sorted by memory usage
→ Translates to:
ps aux --sort=-%mem | head -20
```

### 📋 **AI Task Flow Planner (Command Chaining & Automation)**
**You describe a goal, Nova builds the command plan.**

Nova analyzes your intent and automatically generates a multi-step action plan.

```bash
nova> prepare my workspace for deployment
→ AI expands into:
1. git status           # Check current state
2. git pull            # Get latest changes
3. build-project       # Compile/build project
4. run-tests          # Execute test suite
5. deploy-app         # Deploy to production
6. report-status      # Send deployment report

nova> set up a new development environment
→ AI creates workflow:
1. install-dependencies    # Install required packages
2. setup-database        # Configure database
3. configure-environment # Set environment variables
4. initialize-project    # Create project structure
5. run-initial-tests     # Verify setup
```

### 💻 **AI Code Analyzer & Helper (Professional Development)**
**Nova reads and explains your code like a senior developer.**

Analyze and summarize source code or scripts with professional-level insights.

```bash
nova> ai-analyze main.cpp
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

⚠️  Line 123: Inefficient algorithm O(n²)
   💡 Could be optimized to O(n log n) with sorting

✅ Strengths:
✓ Good error handling patterns
✓ Clear function naming
✓ Proper resource management

💡 Recommendations:
• Add input validation for file operations
• Consider using parallel processing for large datasets
• Add comprehensive logging

nova> ai-analyze --explain main.py
📚 Code Explanation:
==================

Summary: This Python script implements a machine learning pipeline for data analysis.

Key Concepts Used:
• Object-oriented programming
• Data preprocessing with pandas
• Machine learning with scikit-learn
• Error handling and logging

Algorithms Used:
• Random Forest classification
• Cross-validation
• Feature scaling and selection

Potential Issues:
• Memory usage could be high with large datasets
• No input validation on data files
• Hardcoded paths should be configurable
```

### 🔧 **AI Code Editing & Refactoring**
**Professional code improvements with AI assistance.**

```bash
nova> ai-edit Calculator.java refactor --extract-method="calculateTax"
🔧 AI Refactoring: Calculator.java
===============================

✅ Successfully extracted method calculateTax()
📝 Changes made:
• Created new calculateTax() method
• Reduced main method complexity by 40%
• Improved code readability and maintainability

nova> ai-edit api.py optimize performance --target="database_query"
🔧 AI Optimization: api.py
========================

✅ Performance optimization completed
📈 Improvements:
• Query execution time reduced by 60%
• Database connection pooling added
• Caching implemented for frequent queries
```

### 🐛 **AI Debugging Assistant**
**Nova finds bugs and provides professional debugging solutions.**

```bash
nova> ai-debug "segmentation fault in main.cpp at line 42"
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
• Add memory monitoring

🎚️ Confidence: High (98% certainty)
```

### 📊 **AI Code Review & Quality Assurance**
**Professional code review with detailed feedback.**

```bash
nova> ai-review authentication.py security
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
• Implement rate limiting for login attempts

📋 Best Practices Violations:
• No input sanitization
• Missing CSRF protection
• Insufficient error handling

🔒 Security Score: C-
Maintainability: B+
```

### 🧪 **AI Test Generation**
**Generate comprehensive test suites automatically.**

```bash
nova> ai-test Calculator.java junit "unit,integration,edge_cases"
🧪 AI Test Generation: Calculator.java
====================================

Framework: JUnit 5
Test Types: Unit tests, Integration tests, Edge cases

📝 Generated Test Suite:
```java
import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;

class CalculatorTest {

    private Calculator calculator;

    @BeforeEach
    void setUp() {
        calculator = new Calculator();
    }

    // Unit Tests
    @Test
    void testAddition() {
        assertEquals(5, calculator.add(2, 3));
        assertEquals(0, calculator.add(0, 0));
        assertEquals(-1, calculator.add(1, -2));
    }

    @Test
    void testDivisionByZero() {
        assertThrows(ArithmeticException.class, () -> {
            calculator.divide(5, 0);
        });
    }

    // Edge Cases
    @Test
    void testLargeNumbers() {
        assertEquals(Integer.MAX_VALUE, calculator.add(Integer.MAX_VALUE, 0));
    }

    @Test
    void testFloatingPointPrecision() {
        assertEquals(0.3, calculator.add(0.1, 0.2), 0.0001);
    }
}
```

💡 Test Coverage: 95%
   • Unit tests: 85%
   • Integration tests: 10%
   • Edge cases: 5%
```

### 🎓 **AI Interactive Coding Tutor**
**Learn coding with personalized AI assistance.**

```bash
nova> ai-help "how to implement a thread-safe singleton in C++" --skill-level=3
🤖 AI Coding Assistant
=====================

Question: How to implement a thread-safe singleton in C++?
Skill Level: Intermediate (3/5)

💬 Answer:
A thread-safe singleton ensures only one instance exists across multiple threads.
Here are the key approaches:

1. **Meyers' Singleton (C++11)**:
```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }

    // Delete copy/move operations
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;
};
```

2. **Double-Checked Locking**:
```cpp
class Singleton {
public:
    static Singleton* getInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {
                instance = new Singleton();
            }
        }
        return instance;
    }

private:
    static Singleton* instance;
    static std::mutex mutex;
};
```

📋 Next Steps:
1. Choose the appropriate implementation for your use case
2. Consider lazy vs eager initialization
3. Test thread safety with multiple threads
4. Profile performance impact

📚 Learning Resources:
• C++ Concurrency in Action (book)
• Modern C++ Singleton Patterns (article)
• std::call_once documentation
```

### 🏗️ **AI Project Structure Generator**
**Create complete project structures with AI assistance.**

```bash
nova> ai-project web e-commerce-app javascript react nodejs postgresql
🏗️ AI Project Generator
=======================

Creating web project: e-commerce-app
Tech Stack: React + Node.js + PostgreSQL

📁 Generated Project Structure:
```
e-commerce-app/
├── frontend/
│   ├── src/
│   │   ├── components/
│   │   │   ├── Header.jsx
│   │   │   ├── ProductCard.jsx
│   │   │   ├── ShoppingCart.jsx
│   │   │   └── Checkout.jsx
│   │   ├── pages/
│   │   │   ├── Home.jsx
│   │   │   ├── ProductList.jsx
│   │   │   ├── ProductDetail.jsx
│   │   │   └── UserProfile.jsx
│   │   ├── hooks/
│   │   │   ├── useAuth.js
│   │   │   └── useCart.js
│   │   └── App.jsx
│   ├── public/
│   └── package.json
├── backend/
│   ├── src/
│   │   ├── controllers/
│   │   │   ├── authController.js
│   │   │   ├── productController.js
│   │   │   └── orderController.js
│   │   ├── models/
│   │   │   ├── User.js
│   │   │   ├── Product.js
│   │   │   └── Order.js
│   │   ├── routes/
│   │   │   ├── auth.js
│   │   │   ├── products.js
│   │   │   └── orders.js
│   │   ├── middleware/
│   │   │   ├── auth.js
│   │   │   └── validation.js
│   │   └── app.js
│   ├── config/
│   │   └── database.js
│   └── package.json
├── database/
│   ├── migrations/
│   └── seeds/
├── tests/
│   ├── unit/
│   ├── integration/
│   └── e2e/
├── docs/
│   ├── API.md
│   └── DEPLOYMENT.md
├── docker/
│   ├── Dockerfile
│   ├── docker-compose.yml
│   └── nginx.conf
├── .gitignore
├── README.md
└── package.json
```

🚀 Setup Commands:
1. mkdir e-commerce-app && cd e-commerce-app
2. git init
3. npm init -y
4. cd frontend && npx create-react-app .
5. cd ../backend && npm init -y
6. npm install express pg sequelize bcryptjs jsonwebtoken
7. # ... additional setup commands

💡 Next Steps:
• Install dependencies for frontend and backend
• Set up database connection
• Implement authentication system
• Create basic CRUD operations
• Add testing framework
```

---

## 💡 Why NovaShell v1.5 Changes Everything for Developers

### 🎯 **For Students & Learners**
- **AI Tutor**: Get help with assignments, understand complex concepts
- **Code Generation**: Learn by seeing professional code examples
- **Debugging Help**: Understand errors instead of just fixing them
- **Project Creation**: Start new projects with proper structure

### 👨‍💻 **For Professional Developers**
- **Productivity Boost**: 10x faster debugging and code improvements
- **Quality Assurance**: AI code review catches issues before production
- **Knowledge Base**: Build and maintain comprehensive code documentation
- **Automation**: Complex multi-step tasks become one command

### 🏢 **For Teams & Organizations**
- **Code Standards**: Consistent code quality across the team
- **Security**: AI catches vulnerabilities and suggests fixes
- **Documentation**: Auto-generated docs keep knowledge up-to-date
- **Onboarding**: New developers get instant AI assistance

**NovaShell v1.5 transforms the terminal from a text interface into an intelligent coding companion!** 🚀

---

## 📋 Detailed Features

### 🤖 **AI-Powered Command Suggestions**
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

#### **Advanced Remote Desktop** ⭐NEW
**Access other terminals and full screen control with multi-session support.**

#### Terminal Session Access:
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

novashell> remote-desktop-switch term_12345678
✅ Switched to terminal session: term_12345678

novashell> remote-desktop-capture-terminal term_87654321
✅ Captured terminal session: term_87654321
📐 Resolution: 800x600
📊 Data size: 1920000 bytes
🖼️  Full screen: No
```

#### Full Screen Desktop Access:
```bash
novashell> remote-desktop-displays
Available Displays:
===================
1. \\.\DISPLAY1 (1920x1080)
2. \\.\DISPLAY2 (2560x1440)

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

novashell> remote-desktop-set-display \\.\DISPLAY2
✅ Set active display to: \\.\DISPLAY2
📺 Remote desktop will now focus on this display.
```

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

### 💾 **Database Support** ⭐NEW
- **Internal SQLite** for system data (history, config, cache, plugins)
- **MySQL/PostgreSQL** support for user database operations
- Execute queries, manage tables, import/export data
- Connection management with multiple databases
- Commands: `db-connect`, `db-query`, `db-list-tables`, `db-export`, `db-import`

### 🔀 **Git Integration** ⭐NEW
- Full Git workflow support (status, add, commit, push, pull)
- Branch management (create, checkout, merge, rebase)
- GitHub integration with personal access tokens
- Auto-generate .gitignore files
- Stash, tags, and remote management
- Commands: `git`, `git-status`, `git-commit`, `git-push`, `git-branch`, `git-checkout`

### ⌨️ **Tab Completion** ⭐NEW
- Auto-complete commands, file paths, and arguments
- Git branch name completion
- Database table and column completion
- Plugin name completion
- History-based smart suggestions
- Fuzzy matching support
- Press TAB for suggestions!

### 🎨 **Custom Themes** ⭐NEW
- Built-in themes: Dark, Light, Monokai, Solarized, Dracula, Nord, Gruvbox
- Create custom color schemes
- Dark/Light mode toggle
- Custom prompt formats
- Import themes from VSCode and terminal apps
- Live theme preview
- Commands: `theme-list`, `theme-set`, `theme-create`, `theme-export`

### 🔮 **Advanced Tab Completion** ⭐NEW
- AI-powered smart suggestions using Gemini
- Fuzzy matching with Levenshtein distance
- Context-aware multi-word completion
- Learns from your usage patterns
- Abbreviation expansion
- Commands complete faster than ever!

### 🎤 **Voice Commands** ⭐NEW
- Speech-to-text command execution
- Natural language processing
- Text-to-speech responses
- Wake word detection ("Hey Nova")
- Multi-language support
- Offline processing option
- Commands: `voice-start`, `voice-stop`, `voice-config`

### 📊 **Advanced Analytics Dashboard** ⭐NEW
- Real-time system metrics visualization
- Command usage analytics
- Performance graphs and charts
- Custom dashboards and widgets
- Export reports (daily/weekly/monthly)
- Alert thresholds
- Commands: `dashboard`, `analytics`, `report-generate`

---

## 💡 Why NovaShell?

### For Students 🎓
- ✅ **AI Study Assistant** - Get help with coding, assignments, and learning through AI suggestions
- ✅ **All-in-One Learning Tool** - No need for 10 different applications
- ✅ **Free AI Help** - Gemini API learns your workflow patterns and helps with code
- ✅ **Easy Git** - Simplifies version control for assignments and group projects
- ✅ **Database Practice** - Direct MySQL/PostgreSQL access for database classes
- ✅ **Study Tools** - Integrated notes and code snippets for organized learning
- ✅ **Learn Faster** - Tab completion and AI suggestions accelerate your coding skills
- ✅ **Group Projects** - P2P sharing and git collaboration tools
- ✅ **Stay Organized** - Task scheduler and reminders for deadlines and exams

### For Developers 👨‍💻
- ✅ **AI Coding Assistant** - Get intelligent code suggestions and workflow help
- ✅ **Productivity Boost** - Everything in one powerful shell environment
- ✅ **DevOps Ready** - Container, database, git integration for modern development
- ✅ **No Context Switching** - All development tools in a familiar terminal interface
- ✅ **Automation Expert** - Scripting engine for complex development workflows
- ✅ **Professional Quality** - Industry-standard architecture and security
- ✅ **Extensible Platform** - Plugin system for custom development tools
- ✅ **Time Saver** - AI-powered tab completion makes you 3-5x faster
- ✅ **Full-Stack Ready** - From frontend to backend, database to deployment

### For AI Enthusiasts 🤖
- ✅ **Cutting-Edge AI Integration** - Powered by Google's Gemini AI
- ✅ **Natural Language Commands** - Voice commands and text-to-speech responses
- ✅ **Smart Learning** - AI adapts to your usage patterns and preferences
- ✅ **Future-Proof** - Ready for the AI-powered future of computing
- ✅ **Research Platform** - Experiment with AI-assisted development workflows
- ✅ **Open Architecture** - Easy to integrate new AI models and capabilities

---

## 📋 Requirements

- **C++ Compiler**: C++17 or later (GCC, Clang, MSVC)
- **CMake**: Version 3.15 or higher
- **OpenSSL**: For encryption (libssl-dev)
- **SQLite3**: For internal database (libsqlite3-dev)
- **Git**: For git integration features
- **Optional**: libpcap (for network packet capture)
- **Optional**: MySQL/PostgreSQL client libraries (for external DB support)

### Platform-Specific Requirements

#### Windows
- Visual Studio 2019+ or MinGW-w64
- SQLite3 (download from https://www.sqlite.org/)
- Git for Windows
- WinPcap or Npcap (for network features)

#### Linux
```bash
sudo apt-get install build-essential cmake libssl-dev libsqlite3-dev libpcap-dev git
```

#### macOS
```bash
brew install cmake openssl libpcap
```

## 🚀 Installation

### Quick Setup (Windows - Recommended)
```powershell
# Automated setup - installs all dependencies and builds NovaShell
.\setup-dependencies.ps1
.\build.ps1
.\run-novashell.ps1
```

### Detailed Installation Guide
For comprehensive installation instructions including:
- Manual setup steps
- Platform-specific requirements  
- Dependency installation
- Troubleshooting
- Configuration options

📖 **See [SETUP.md](SETUP.md)** for complete installation guide.

### Manual Installation
```bash
# Clone repository
git clone https://github.com/yourusername/novashell.git
cd novashell

# Build
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Run
./bin/customos-shell
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

### 🔄 **P2P File Sharing** ⭐NEW
**LAN file sharing without cloud services - files stored locally, metadata in database.**

```bash
novashell> p2p-start
P2P file sharing server started on port 8888
Files will be stored locally in: .customos/p2p/shares/
Only metadata is stored in the database.

novashell> p2p-share /projects/report.pdf --public
✅ File shared successfully!
Share ID: share_a1b2c3d4
File: report.pdf
📁 Stored locally in: .customos/p2p/shares/
💾 Only metadata stored in database
🌐 Public share

novashell> p2p-list
Shared P2P Files:
=================
📄 report.pdf (ID: share_a1b2c3d4)
   Size: 245760 bytes
   Checksum: a1b2c3d4... (truncated)
   Local Path: .customos/p2p/shares/share_a1b2c3d4.pdf
   🌐 Public

💡 Files are stored locally on your PC, not in the database!
   Database only contains metadata for efficient sharing.
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

### 💾 Database Operations

#### Internal SQLite (Automatic)
```bash
# System automatically uses SQLite for:
# - Command history
# - Plugin metadata
# - Configuration storage
# - Cache management
# - Session tracking

# Query history
customos> history
git status
vault-get github.com
monitor-stats

# Search history
customos> history-search "git"
git status
git commit -m "Update"
git push
```

#### External Database (MySQL/PostgreSQL)
```bash
# Connect to MySQL
customos> db-connect mydb --mysql localhost 3306 testdb myuser
Password: ****
Connected to mydb (MySQL)

# List tables
customos> db-list-tables
users
products
orders

# Execute query
customos> db-query "SELECT * FROM users LIMIT 5"
ID | NAME      | EMAIL
1  | John Doe  | john@example.com
2  | Jane Smith| jane@example.com

# Insert data
customos> db-insert users name="Bob" email="bob@test.com"
Row inserted successfully

# Export to CSV
customos> db-export users users_backup.csv
Exported 1,245 rows to users_backup.csv

# Switch databases
customos> db-switch proddb
Switched to proddb (PostgreSQL)
```

### 🔀 Git Workflow

#### Basic Operations
```bash
# Initialize repository
customos> git init
Initialized empty Git repository

# Check status
customos> git status
On branch main
Untracked files:
  README.md
  src/

# Add files
customos> git add .
All changes staged

# Commit
customos> git commit "Initial commit"
[main abc1234] Initial commit
 5 files changed, 250 insertions(+)

# Push to remote
customos> git push origin main
Pushing to https://github.com/user/repo.git
Done!
```

#### Branch Management
```bash
# Create branch
customos> git branch feature/new-module
Branch 'feature/new-module' created

# Checkout branch
customos> git checkout feature/new-module
Switched to branch 'feature/new-module'

# Or create and checkout
customos> git checkout -b feature/another
Switched to new branch 'feature/another'

# List branches
customos> git branch
  main
  feature/new-module
* feature/another

# Merge branch
customos> git checkout main
customos> git merge feature/new-module
Merge successful
```

#### GitHub Integration
```bash
# Set GitHub token
customos> git-set-token ghp_xxxxxxxxxxxx
GitHub token saved

# Create GitHub repo
customos> git-create-repo my-awesome-project --private
Repository created: https://github.com/user/my-awesome-project

# Generate .gitignore
customos> git-gen-ignore Node Python
Generated .gitignore for: Node, Python

# Create pull request
customos> git-pr "Add new feature" --base main --head feature/new-module
Pull request created: #42
```

### ⌨️ Tab Completion

#### Using Tab Completion
```bash
# Command completion
customos> va<TAB>
vault-add    vault-get    vault-list   vault-gen    vault-init

# File path completion
customos> vfs-ls /home/user/Doc<TAB>
/home/user/Documents/

# Git branch completion
customos> git checkout fea<TAB>
feature/new-module    feature/another    feature/bugfix

# Database table completion
customos> db-query "SELECT * FROM us<TAB>
users    user_sessions    user_preferences

# Multiple suggestions
customos> git <TAB>
status    add       commit    push      pull      branch
checkout  merge     log       diff      stash     tag

# Smart context completion
customos> git commit <TAB>
-m        --amend   --no-edit   -a

# History-based completion
customos> git pus<TAB>
git push origin main  (from history)
git push --tags       (from history)
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
│   ├── database/       # Database Support (SQLite/MySQL/PostgreSQL) ⭐NEW
│   ├── git/            # Git Integration ⭐NEW
│   └── utils/          # Utilities
├── include/            # Public headers
│   ├── core/tab_completion.h  # Tab Completion ⭐NEW
│   ├── database/       # Database headers ⭐NEW
│   └── git/            # Git headers ⭐NEW
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

- **Issues**: [GitHub Issues](https://github.com/yourusername/novashell/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/novashell/discussions)
- **Email**: support@novashell.dev

## 🗺️ Roadmap

### ✅ Completed Features (v1.0 - Core System)
**Core Features (15)**
- [x] Virtual File System (VFS)
- [x] Plugin System
- [x] User Authentication & Access Control
- [x] Scripting Engine
- [x] Network Packet Analyzer
- [x] Password Manager (Vault)
- [x] File System Utilities
- [x] Logging & Audit System
- [x] Process Management
- [x] Cloud Backup & Sync
- [x] Git Integration
- [x] Database Support (SQLite + MySQL/PostgreSQL)
- [x] Tab Completion (Commands, Files, Git, DB)
- [x] Custom Themes
- [x] Task Scheduler & Reminder System

**Total: 15 Major Features** 🎉

### ✅ Completed Features (v1.5 - AI Terminal Assistant)
**🤖 AI Features (10 Revolutionary Additions)**
- [x] **🧠 AI Command Understanding** - Natural language to CLI commands
- [x] **📋 AI Task Flow Planner** - Goal to multi-step automation
- [x] **🧠 AI Memory & Context Engine** - Workflow remembrance & recall
- [x] **💻 AI Code Analyzer & Helper** - Code review, explanation, debugging
- [x] **🔍 AI Error & Log Analyzer** - Log diagnosis & fix suggestions
- [x] **📄 AI Data & File Summarizer** - Document summarization & insights
- [x] **🕸️ AI Knowledge Graph** - Project relationship mapping
- [x] **⚙️ AI Routine & Automation Manager** - Personalized workflow automation
- [x] **🔎 AI Search & Knowledge Recall** - Semantic search & memory
- [x] **📚 AI Explanation Engine** - Technical concept education

**Total: 25 Major Features** 🎉

### 🚀 **Future: NovaShell v2.0 - Voice & Cloud Revolution**

**Coming in 2025: Voice commands, real-time collaboration, and cloud synchronization!**

#### 🔥 **Voice & Cloud Features (8 Next-Gen Additions)**
- [x] **🎤 Advanced Voice Commands** - "Hey Nova" wake word + speech-to-text
- [x] **☁️ Cloud Sync** - Seamless device synchronization
- [x] **👥 Real-time Collaboration** - Work together on projects live
- [x] Remote Desktop Integration ⭐ENHANCED
- [x] Mobile Companion App ⭐ENHANCED
- [x] Advanced Remote Desktop (Terminal Access & Full Screen) ⭐NEW
- [x] Enhanced P2P File Storage (Local Files, DB Metadata) ⭐NEW
- [x] AIPromptManager (Modular AI Prompts) ⭐NEW
- [x] **🌍 Global Plugin Marketplace** - Community plugin ecosystem
- [x] **🔐 Advanced Security** - Biometric authentication
- [x] **📈 Performance Analytics** - AI-powered productivity insights

**Expected: Q2 2025** - **The complete AI terminal experience!** 🚀

### 💫 **Why NovaShell v2.0 Will Change Everything**

NovaShell v2.0 isn't just an update—it's a **complete reimagining** of what a terminal can be. By integrating cutting-edge AI with traditional shell capabilities, NovaShell becomes your **intelligent companion** for life and code.

**Imagine:**
- Talking to your terminal naturally: *"Hey Nova, show me my git status and suggest what to do next"*
- Getting AI help with complex commands: *"How do I deploy this to Kubernetes?"*
- Having your terminal learn your habits and anticipate your needs
- Real-time collaboration on coding projects
- Voice-controlled development workflows

**NovaShell v2.0 will be the first terminal that truly understands you!** 🤖💫

### 🔔 **Stay Tuned for Updates**
- Follow our [GitHub](https://github.com/yourusername/novashell) for development updates
- Join our [Discord community](https://discord.gg/novashell) for early access
- Subscribe to our newsletter for exclusive v2.0 previews

---

**Made with ❤️ by the NovaShell Team**
