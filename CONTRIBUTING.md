# Contributing to CustomOS Shell

Thank you for your interest in contributing to CustomOS Shell! This document provides guidelines for contributing to the project.

## Code of Conduct

- Be respectful and inclusive
- Welcome newcomers and help them learn
- Focus on constructive criticism
- Respect differing viewpoints and experiences

## How to Contribute

### Reporting Bugs

1. Check if the bug has already been reported in [Issues](https://github.com/yourusername/customos-shell/issues)
2. If not, create a new issue with:
   - Clear, descriptive title
   - Steps to reproduce
   - Expected vs actual behavior
   - System information (OS, compiler, version)
   - Relevant logs or screenshots

### Suggesting Features

1. Check existing feature requests
2. Open a new issue with:
   - Clear use case
   - Proposed solution
   - Alternative approaches considered
   - Willingness to implement it

### Submitting Code

#### Before You Start

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Keep changes focused and atomic

#### Code Standards

**C++ Style**:
- Follow C++17 standards
- Use modern C++ features (smart pointers, auto, range-for)
- Avoid raw pointers and manual memory management
- Use const-correctness
- Prefer RAII for resource management

**Naming Conventions**:
```cpp
// Classes: PascalCase
class CommandProcessor { };

// Functions/Methods: snake_case
void execute_command();

// Variables: snake_case
int user_count;

// Constants: UPPER_SNAKE_CASE
const int MAX_USERS = 100;

// Members: trailing underscore
class Shell {
    std::string prompt_;
};
```

**File Organization**:
- Header files in `include/`
- Implementation files in `src/`
- One class per file (generally)
- Use header guards or `#pragma once`

**Comments**:
```cpp
// Brief function description
// @param name Parameter description
// @return Return value description
int my_function(const std::string& name);

// Use // for single-line comments
/* Use this for multi-line
   block comments */
```

#### Building and Testing

```bash
# Build the project
mkdir build && cd build
cmake ..
cmake --build .

# Run tests (if available)
ctest
```

#### Commit Guidelines

**Commit Messages**:
```
<type>: <subject>

<body>

<footer>
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `test`: Adding tests
- `chore`: Build/tooling changes

**Example**:
```
feat: Add password strength indicator to vault

- Implemented zxcvbn algorithm for strength calculation
- Added visual indicator in vault-add command
- Updated documentation

Closes #123
```

#### Pull Request Process

1. **Update Documentation**: If you change functionality, update relevant docs
2. **Add Tests**: Include unit tests for new features
3. **Follow Code Style**: Ensure code matches project style
4. **Update CHANGELOG**: Add entry for your change
5. **Rebase on Main**: Keep your branch up to date
6. **Small PRs**: Keep changes focused and reviewable

**PR Template**:
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
How was this tested?

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings
- [ ] Tests added/updated
- [ ] All tests pass
```

## Development Setup

### Prerequisites
- C++17 compatible compiler
- CMake 3.15+
- OpenSSL development libraries
- Git

### Setup Steps

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/customos-shell.git
cd customos-shell

# Add upstream remote
git remote add upstream https://github.com/ORIGINAL_OWNER/customos-shell.git

# Create feature branch
git checkout -b feature/my-feature

# Make changes, commit, push
git add .
git commit -m "feat: my new feature"
git push origin feature/my-feature
```

## Plugin Development

To create a plugin:

```cpp
#include "customos/plugin_api.h"

class MyPlugin : public IPlugin {
public:
    std::string get_name() const override {
        return "MyPlugin";
    }

    std::string get_version() const override {
        return "1.0.0";
    }

    bool initialize() override {
        // Register commands
        register_command("mycommand", [](const auto& ctx) {
            std::cout << "Hello from plugin!\n";
            return 0;
        });
        return true;
    }

    void shutdown() override {
        // Cleanup
    }

    std::vector<std::string> get_commands() const override {
        return {"mycommand"};
    }
};

EXPORT_PLUGIN(MyPlugin)
```

## Code Review Process

1. **Automated Checks**: CI runs tests and linters
2. **Peer Review**: At least one maintainer reviews
3. **Feedback**: Address review comments
4. **Approval**: Maintainer approves when ready
5. **Merge**: Squash and merge into main

## Architecture Guidelines

- **Modularity**: Keep modules independent
- **Single Responsibility**: One class, one job
- **SOLID Principles**: Follow OOP best practices
- **Error Handling**: Use exceptions for initialization, return codes for operations
- **Thread Safety**: Use mutexes for shared state
- **Memory Safety**: Use smart pointers, avoid leaks

## Testing Guidelines

```cpp
// Unit test example
TEST(CommandRegistry, RegisterCommand) {
    CommandRegistry registry;
    CommandInfo cmd;
    cmd.name = "test";
    cmd.handler = [](const auto&) { return 0; };
    
    ASSERT_TRUE(registry.register_command(cmd));
    ASSERT_TRUE(registry.has_command("test"));
}
```

## Documentation

- **Code Comments**: Explain *why*, not *what*
- **API Documentation**: Document all public interfaces
- **README**: Keep updated with new features
- **Examples**: Provide usage examples
- **Architecture**: Update ARCHITECTURE.md for major changes

## Release Process

1. Update version numbers
2. Update CHANGELOG.md
3. Tag release: `git tag v1.0.0`
4. Push tag: `git push --tags`
5. Create GitHub release
6. Build and upload binaries

## Getting Help

- **Issues**: For bugs and features
- **Discussions**: For questions and ideas
- **Email**: support@customos-shell.dev
- **Discord**: (link if available)

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Credited in release notes
- Mentioned in project README

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to CustomOS Shell! 🎉
