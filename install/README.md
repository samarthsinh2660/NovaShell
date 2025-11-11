# NovaShell Installation Scripts

Professional one-command installation for NovaShell, just like Docker, Node.js, and Homebrew!

## 🚀 Quick Install

### Linux / macOS

```bash
curl -fsSL https://novashell.dev/install.sh | bash
```

Or with wget:
```bash
wget -qO- https://novashell.dev/install.sh | bash
```

### Windows (PowerShell)

```powershell
iwr -useb https://novashell.dev/install.ps1 | iex
```

---

## 📋 What the Installer Does

1. ✅ **Detects Platform** - Automatically detects OS and architecture
2. ✅ **Downloads Binary** - Gets the correct binary for your system
3. ✅ **Installs to PATH** - Makes `novashell` command available everywhere
4. ✅ **Sets Up Config** - Creates default configuration files
5. ✅ **Checks Dependencies** - Verifies required tools are installed
6. ✅ **Installs Completions** - Adds shell tab completion (optional)

---

## 🔧 Installation Options

### Custom Installation Directory

**Linux/macOS:**
```bash
NOVASHELL_INSTALL_DIR=/opt/novashell curl -fsSL https://novashell.dev/install.sh | bash
```

**Windows:**
```powershell
iwr -useb https://novashell.dev/install.ps1 | iex -InstallDir "C:\NovaShell"
```

### Specific Version

**Linux/macOS:**
```bash
NOVASHELL_VERSION=v1.0.0 curl -fsSL https://novashell.dev/install.sh | bash
```

**Windows:**
```powershell
iwr -useb https://novashell.dev/install.ps1 | iex -Version "v1.0.0"
```

---

## 📁 Installation Locations

### Linux

- **Binary**: `/usr/local/bin/novashell` (or `~/.local/bin/novashell`)
- **Config**: `~/.novashell/config.json`
- **Data**: `~/.novashell/system.db`
- **Logs**: `~/.novashell/nova.log`

### macOS

- **Binary**: `/usr/local/bin/novashell`
- **Config**: `~/.novashell/config.json`
- **Data**: `~/.novashell/system.db`
- **Logs**: `~/.novashell/nova.log`

### Windows

- **Binary**: `%LOCALAPPDATA%\NovaShell\novashell.exe`
- **Config**: `%USERPROFILE%\.novashell\config.json`
- **Data**: `%USERPROFILE%\.novashell\system.db`
- **Logs**: `%USERPROFILE%\.novashell\nova.log`

---

## 🌐 Hosting the Installation Scripts

To make the one-command install work, host these files:

### Option 1: GitHub Pages (Free)

1. Create a `gh-pages` branch
2. Add `install.sh` and `install.ps1`
3. Enable GitHub Pages in repository settings
4. Access at: `https://yourusername.github.io/novashell/install.sh`

### Option 2: Netlify / Vercel (Free)

1. Create a `public` folder
2. Add installation scripts
3. Deploy to Netlify/Vercel
4. Get custom domain: `https://novashell.dev/install.sh`

### Option 3: Cloudflare Pages (Free)

1. Create Pages project
2. Upload scripts
3. Access at: `https://novashell.pages.dev/install.sh`

### Option 4: Your Own Domain

1. Upload scripts to web server
2. Make them accessible via HTTPS
3. Share: `https://yourdomain.com/install.sh`

---

## 🔐 Security Best Practices

### Verify Script Before Running

**Linux/macOS:**
```bash
# Download and inspect first
curl -fsSL https://novashell.dev/install.sh -o install.sh
cat install.sh
bash install.sh
```

**Windows:**
```powershell
# Download and inspect first
Invoke-WebRequest -Uri https://novashell.dev/install.ps1 -OutFile install.ps1
Get-Content install.ps1
.\install.ps1
```

### Checksum Verification

We provide SHA256 checksums for all releases:

```bash
# Download checksum
curl -fsSL https://novashell.dev/checksums.txt -o checksums.txt

# Verify binary
sha256sum -c checksums.txt
```

---

## 📦 Manual Installation

If you prefer manual installation:

### Linux / macOS

```bash
# 1. Download binary
wget https://github.com/yourusername/novashell/releases/latest/download/novashell-linux-amd64

# 2. Make executable
chmod +x novashell-linux-amd64

# 3. Move to PATH
sudo mv novashell-linux-amd64 /usr/local/bin/novashell

# 4. Verify
novashell --version
```

### Windows

1. Download: https://github.com/yourusername/novashell/releases/latest
2. Extract `novashell.exe`
3. Add to PATH via System Properties → Environment Variables
4. Restart terminal

---

## 🔄 Updating NovaShell

### Re-run Installer

The installer automatically detects and upgrades existing installations:

```bash
curl -fsSL https://novashell.dev/install.sh | bash
```

### Using Built-in Updater (Coming Soon)

```bash
novashell update
```

---

## 🗑️ Uninstallation

### Linux / macOS

```bash
# Remove binary
sudo rm /usr/local/bin/novashell

# Remove config (optional)
rm -rf ~/.novashell
```

### Windows

```powershell
# Remove from PATH manually via Environment Variables
# Then delete:
Remove-Item -Recurse "$env:LOCALAPPDATA\NovaShell"
Remove-Item -Recurse "$env:USERPROFILE\.novashell"
```

---

## 🛠️ Building Release Binaries

For maintainers who want to create release binaries:

```bash
# Build for all platforms
./install/build-releases.sh

# This creates:
# - novashell-linux-amd64
# - novashell-linux-arm64
# - novashell-darwin-amd64
# - novashell-darwin-arm64
# - novashell-windows-amd64.exe
```

---

## 📊 Installation Analytics

Track installations with GitHub releases download count:

```bash
# View download stats
curl -s https://api.github.com/repos/yourusername/novashell/releases/latest \
  | grep download_count
```

---

## 🤝 Contributing

Help improve the installation experience:

1. Test on different platforms
2. Report installation issues
3. Suggest improvements
4. Add support for more package managers

---

## 📞 Support

Installation issues? We're here to help!

- **GitHub Issues**: https://github.com/yourusername/novashell/issues
- **Discussions**: https://github.com/yourusername/novashell/discussions
- **Email**: install@novashell.dev

---

## 🌟 Why Professional Installation Matters

Users expect professional software to install easily:

✅ **One Command** - No manual steps
✅ **Cross-Platform** - Works everywhere
✅ **Safe** - Checksums and HTTPS
✅ **Smart** - Auto-detects everything
✅ **Clean** - Easy to uninstall

**NovaShell's installer** rivals industry leaders like Docker, Node.js, and Rust!

---

**Made with ❤️ by the NovaShell Team**
