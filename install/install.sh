#!/bin/bash
#
# NovaShell Installation Script
# Install NovaShell with a single command:
# curl -fsSL https://novashell.dev/install.sh | bash
#
# Or with wget:
# wget -qO- https://novashell.dev/install.sh | bash
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
NOVASHELL_VERSION="${NOVASHELL_VERSION:-latest}"
INSTALL_DIR="${NOVASHELL_INSTALL_DIR:-/usr/local/bin}"
CONFIG_DIR="$HOME/.novashell"
GITHUB_REPO="yourusername/novashell"
BINARY_NAME="novashell"

# Detect OS and Architecture
detect_platform() {
    OS="$(uname -s)"
    ARCH="$(uname -m)"
    
    case "$OS" in
        Linux*)     PLATFORM="linux";;
        Darwin*)    PLATFORM="darwin";;
        MINGW*|MSYS*|CYGWIN*) PLATFORM="windows";;
        *)          
            echo -e "${RED}Unsupported operating system: $OS${NC}"
            exit 1
            ;;
    esac
    
    case "$ARCH" in
        x86_64|amd64)   ARCH="amd64";;
        aarch64|arm64)  ARCH="arm64";;
        *)
            echo -e "${RED}Unsupported architecture: $ARCH${NC}"
            exit 1
            ;;
    esac
}

# Print banner
print_banner() {
    echo -e "${BLUE}"
    cat << "EOF"
╔═══════════════════════════════════════════════╗
║                                               ║
║            NovaShell Installer                ║
║                                               ║
║     The Ultimate Development Shell            ║
║     21+ Features | AI-Powered | Modern        ║
║                                               ║
╚═══════════════════════════════════════════════╝
EOF
    echo -e "${NC}"
}

# Check if running as root (for system-wide install)
check_permissions() {
    if [ "$EUID" -eq 0 ]; then
        echo -e "${YELLOW}Running as root - installing system-wide${NC}"
        INSTALL_DIR="/usr/local/bin"
    else
        if [ ! -w "$INSTALL_DIR" ]; then
            echo -e "${YELLOW}No write permission to $INSTALL_DIR${NC}"
            echo -e "${YELLOW}Installing to $HOME/.local/bin${NC}"
            INSTALL_DIR="$HOME/.local/bin"
            mkdir -p "$INSTALL_DIR"
            
            # Add to PATH if not already there
            if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
                echo -e "${YELLOW}Adding $HOME/.local/bin to PATH${NC}"
                echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
                echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.zshrc" 2>/dev/null || true
            fi
        fi
    fi
}

# Check dependencies
check_dependencies() {
    echo -e "${BLUE}Checking dependencies...${NC}"
    
    local missing_deps=()
    
    # Required dependencies
    if ! command -v git &> /dev/null; then
        missing_deps+=("git")
    fi
    
    # Optional but recommended
    if ! command -v sqlite3 &> /dev/null; then
        echo -e "${YELLOW}SQLite3 not found (optional but recommended)${NC}"
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        echo -e "${RED}Missing required dependencies: ${missing_deps[*]}${NC}"
        echo -e "${YELLOW}Install them with:${NC}"
        
        if [ "$PLATFORM" = "linux" ]; then
            if command -v apt-get &> /dev/null; then
                echo "  sudo apt-get install ${missing_deps[*]}"
            elif command -v yum &> /dev/null; then
                echo "  sudo yum install ${missing_deps[*]}"
            elif command -v pacman &> /dev/null; then
                echo "  sudo pacman -S ${missing_deps[*]}"
            fi
        elif [ "$PLATFORM" = "darwin" ]; then
            echo "  brew install ${missing_deps[*]}"
        fi
        
        exit 1
    fi
    
    echo -e "${GREEN}✓ All required dependencies found${NC}"
}

# Download binary
download_binary() {
    echo -e "${BLUE}Downloading NovaShell ${NOVASHELL_VERSION}...${NC}"
    
    local download_url
    if [ "$NOVASHELL_VERSION" = "latest" ]; then
        download_url="https://github.com/$GITHUB_REPO/releases/latest/download/novashell-$PLATFORM-$ARCH"
    else
        download_url="https://github.com/$GITHUB_REPO/releases/download/$NOVASHELL_VERSION/novashell-$PLATFORM-$ARCH"
    fi
    
    local tmp_file="/tmp/novashell-$$.tmp"
    
    if command -v curl &> /dev/null; then
        curl -fsSL "$download_url" -o "$tmp_file"
    elif command -v wget &> /dev/null; then
        wget -q "$download_url" -O "$tmp_file"
    else
        echo -e "${RED}Neither curl nor wget found. Please install one of them.${NC}"
        exit 1
    fi
    
    if [ ! -f "$tmp_file" ]; then
        echo -e "${RED}Failed to download NovaShell${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Downloaded successfully${NC}"
    echo "$tmp_file"
}

# Install binary
install_binary() {
    local tmp_file="$1"
    
    echo -e "${BLUE}Installing NovaShell to $INSTALL_DIR...${NC}"
    
    # Make executable
    chmod +x "$tmp_file"
    
    # Move to install directory
    mv "$tmp_file" "$INSTALL_DIR/$BINARY_NAME"
    
    echo -e "${GREEN}✓ NovaShell installed successfully${NC}"
}

# Setup configuration
setup_config() {
    echo -e "${BLUE}Setting up configuration...${NC}"
    
    mkdir -p "$CONFIG_DIR"
    
    # Create default config if it doesn't exist
    if [ ! -f "$CONFIG_DIR/config.json" ]; then
        cat > "$CONFIG_DIR/config.json" << 'EOC'
{
  "shell": {
    "prompt": "nova> ",
    "history_size": 1000,
    "theme": "dark"
  },
  "ai": {
    "enabled": false,
    "gemini_api_key": ""
  },
  "database": {
    "internal_db": ".novashell/system.db"
  },
  "logging": {
    "level": "info",
    "file": ".novashell/nova.log"
  }
}
EOC
        echo -e "${GREEN}✓ Created default configuration${NC}"
    else
        echo -e "${YELLOW}Configuration already exists, skipping${NC}"
    fi
}

# Install optional dependencies
install_optional_deps() {
    echo -e "${BLUE}Would you like to install optional dependencies? (y/N)${NC}"
    read -r response
    
    if [[ "$response" =~ ^[Yy]$ ]]; then
        echo -e "${BLUE}Installing optional dependencies...${NC}"
        
        if [ "$PLATFORM" = "linux" ]; then
            if command -v apt-get &> /dev/null; then
                sudo apt-get update
                sudo apt-get install -y libsqlite3-dev libssl-dev libpcap-dev
            elif command -v yum &> /dev/null; then
                sudo yum install -y sqlite-devel openssl-devel libpcap-devel
            fi
        elif [ "$PLATFORM" = "darwin" ]; then
            brew install sqlite openssl libpcap
        fi
        
        echo -e "${GREEN}✓ Optional dependencies installed${NC}"
    fi
}

# Create shell completions
install_completions() {
    echo -e "${BLUE}Installing shell completions...${NC}"
    
    # Bash completion
    if [ -d "/etc/bash_completion.d" ]; then
        sudo tee "/etc/bash_completion.d/novashell" > /dev/null << 'EOB'
_novashell_completion() {
    local cur prev
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    
    # Add your completion logic here
    local commands="help version git db vault-add monitor-stats"
    COMPREPLY=( $(compgen -W "${commands}" -- ${cur}) )
    return 0
}
complete -F _novashell_completion novashell
EOB
        echo -e "${GREEN}✓ Bash completion installed${NC}"
    fi
}

# Print success message
print_success() {
    echo ""
    echo -e "${GREEN}╔═══════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║                                               ║${NC}"
    echo -e "${GREEN}║     NovaShell installed successfully! 🎉      ║${NC}"
    echo -e "${GREEN}║                                               ║${NC}"
    echo -e "${GREEN}╚═══════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${BLUE}Installation Directory:${NC} $INSTALL_DIR"
    echo -e "${BLUE}Configuration:${NC} $CONFIG_DIR"
    echo ""
    echo -e "${YELLOW}Quick Start:${NC}"
    echo -e "  1. Run: ${GREEN}novashell${NC}"
    echo -e "  2. Type: ${GREEN}help${NC} to see all commands"
    echo -e "  3. Setup AI: ${GREEN}ai-init YOUR_GEMINI_KEY${NC}"
    echo ""
    echo -e "${YELLOW}Documentation:${NC}"
    echo -e "  https://novashell.dev/docs"
    echo ""
    echo -e "${YELLOW}Get Gemini API Key (FREE):${NC}"
    echo -e "  https://makersuite.google.com/app/apikey"
    echo ""
    
    # Check if binary is in PATH
    if ! command -v novashell &> /dev/null; then
        echo -e "${YELLOW}⚠️  NovaShell is not in your PATH${NC}"
        echo -e "Add this to your shell profile:"
        echo -e "  ${GREEN}export PATH=\"$INSTALL_DIR:\$PATH\"${NC}"
        echo ""
        echo -e "Then run: ${GREEN}source ~/.bashrc${NC} (or ~/.zshrc)"
        echo ""
    fi
}

# Main installation flow
main() {
    print_banner
    
    echo -e "${BLUE}Starting installation...${NC}"
    echo ""
    
    detect_platform
    echo -e "${GREEN}✓ Platform detected: $PLATFORM-$ARCH${NC}"
    
    check_permissions
    check_dependencies
    
    local binary_file
    binary_file=$(download_binary)
    
    install_binary "$binary_file"
    setup_config
    install_optional_deps
    install_completions
    
    print_success
}

# Run main
main "$@"
