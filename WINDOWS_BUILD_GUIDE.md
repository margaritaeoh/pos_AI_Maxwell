# Windows Build Guide for POS Application

## System Requirements

- **Windows Version**: Windows 7 or later
- **Architecture**: x86-64 (64-bit)
- **RAM**: 2 GB minimum, 4 GB recommended
- **Disk Space**: 500 MB for build tools + dependencies

## Installation Steps

### 1. Install Visual Studio Build Tools or MSYS2

#### Option A: MSYS2 / MinGW64 (Recommended for Open Source)

1. **Download and Install MSYS2**
   - Visit: https://www.msys2.org/
   - Download the latest installer
   - Follow the installation wizard
   - Default installation path: `C:\msys64`

2. **Update MSYS2** (Open MSYS2 terminal)
   ```bash
   pacman -Syu
   ```
   - Close and reopen terminal when prompted
   - Run again: `pacman -Syu`

3. **Install Build Tools** (in MinGW64 terminal)
   ```bash
   pacman -S git
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-cmake
   pacman -S mingw-w64-x86_64-make
   ```

4. **Install Dependencies**
   ```bash
   pacman -S mingw-w64-x86_64-wxWidgets
   pacman -S mingw-w64-x86_64-lmdb
   pacman -S mingw-w64-x86_64-pkgconf
   ```

#### Option B: Visual Studio Community (Alternative)

1. **Install Visual Studio Community**
   - Visit: https://visualstudio.microsoft.com/
   - Download Community Edition
   - During installation, select:
     - C++ development tools
     - CMake tools
     - Windows SDK

2. **Install Dependencies via vcpkg**
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\vcpkg integrate install
   .\vcpkg install wxwidgets:x64-windows lmdb:x64-windows
   ```

### 2. Verify Installation

Open **MinGW64 terminal** (or Command Prompt for MSVC) and verify:

```bash
g++ --version
cmake --version
wx-config --version
```

All should return version numbers.

## Building with MSYS2/MinGW64

### Method 1: Using the Automated Build Script

1. **Open MinGW64 Terminal**
   - Search for "MSYS2 MinGW 64-bit"
   - Or run: `C:\msys64\msys2_shell.cmd -mingw64`

2. **Navigate to project directory**
   ```bash
   cd /c/path/to/pos_project
   ```

3. **Run build script**
   ```bash
   bash build_multiplatform.sh
   ```

### Method 2: Manual CMake Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Build (use -j with number of CPU cores)
cmake --build . -j4

# Or use make directly
make -j4
```

### Method 3: Create Visual Studio Project

```bash
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
start POS.sln
```
Then build from Visual Studio IDE.

## Building with MSVC

### Method 1: Using CMake

```cmd
mkdir build
cd build

# Configure for Visual Studio
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release ..

# Build using MSBuild
msbuild POS.sln /p:Configuration=Release /p:Platform=x64
```

### Method 2: Using Visual Studio IDE

```cmd
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
# This opens a .sln solution file that can be opened in Visual Studio
```

## Running the Application

After successful build:

### From MSYS2 Terminal
```bash
cd build
./POS.exe
```

### From Windows Command Prompt
```cmd
cd build
POS.exe
```

### From File Explorer
- Navigate to `build/` folder
- Double-click `POS.exe`

## Troubleshooting

### "CMake not found"
```bash
pacman -S mingw-w64-x86_64-cmake
```

### "wxWidgets not found" or compilation fails
```bash
# Reinstall wxWidgets
pacman -R mingw-w64-x86_64-wxWidgets
pacman -S mingw-w64-x86_64-wxWidgets

# Try rebuilding with verbose output
cd build && cmake --build . --verbose
```

### "wx-config not found"
```bash
# Check installation
pacman -S mingw-w64-x86_64-wxWidgets

# Verify path
which wx-config

# If needed, add to PATH
export PATH="/mingw64/bin:$PATH"
```

### LMDB linking errors
```bash
# Reinstall LMDB
pacman -R mingw-w64-x86_64-lmdb
pacman -S mingw-w64-x86_64-lmdb

# Check pkg-config can find it
pkg-config --cflags --libs lmdb
```

### "undefined reference to" errors
This means a library isn't linked properly. Try:
```bash
# Clean and rebuild
cd build
rm -rf *
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build . --verbose
```

### Terminal encoding issues (garbled output)
The terminal should be UTF-8 compatible. In MSYS2, this is default, but if issues occur:
```bash
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8
```

### Permission denied
```bash
# Make scripts executable
chmod +x *.sh

# Or delete .gitattributes enforcement
git config core.safecrlf false
```

## Creating Installer (NSIS)

POS_Installer.nsi is included for creating Windows installer:

### Prerequisites
1. Download NSIS from: https://nsis.sourceforge.io/
2. Install in default location: `C:\Program Files (x86)\NSIS`

### Creating .exe Installer

```bash
# After successful build
cd ..
"C:\Program Files (x86)\NSIS\makensis.exe" /V4 POS_Installer.nsi
```

This creates `POS_Setup.exe` ready for distribution.

### Customizing Installer

Edit `POS_Installer.nsi` to customize:
- Application name and version
- Installation directory
- Start menu items
- Uninstall configuration

## Release Build Optimization

For optimized releases:

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS="-O3 -march=native" ..
cmake --build . -j4
```

## Performance Tips

### Speed Up Incremental Builds
```bash
# Use multiple cores (4, 8, 12, etc.)
cmake --build . -j8
```

### Faster Compilation with ccache
```bash
# Install
pacman -S mingw-w64-x86_64-ccache

# Use in CMake
cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ..
```

## Environment Variables

Optional customization:

```bash
# Use specific compiler
export CXX=g++
export CC=gcc

# Force static linking
export LDFLAGS="-static-libgcc -static-libstdc++"

# Enable optimizations
export CXXFLAGS="-O3 -march=native"

# Then build
bash build_multiplatform.sh
```

## Development with IDE

### Visual Studio Code (Recommended)

1. **Install Extensions**
   - C/C++ Extension Pack
   - CMake Tools
   - MSYS2 Integration (optional)

2. **Configure**
   - Open Command Palette (Ctrl+Shift+P)
   - Select "Select CMake Kit"
   - Choose MinGW 64-bit compiler

3. **Build and Debug**
   - Ctrl+Shift+B to build
   - F5 to debug

### CLion

1. **Open Project**
   - File → Open → Project folder

2. **Configure Toolchain**
   - Settings → Build → Toolchains
   - Select MinGW from MSYS2

3. **Build**
   - Build → Build Project

## 32-bit vs 64-bit

This project builds for **64-bit** (x86-64) by default. To build 32-bit:

```bash
# Use mingw-w64-i686 version
pacman -S mingw-w64-i686-gcc mingw-w64-i686-wxWidgets mingw-w64-i686-lmdb

# Open MinGW32 terminal instead of MinGW64
C:\msys64\msys2_shell.cmd -mingw32

# Then build
bash build_multiplatform.sh
```

## Static vs Dynamic Linking

### Static Linking (Recommended for Distribution)
```bash
cmake -DCMAKE_CXX_FLAGS="-static-libgcc -static-libstdc++" ..
cmake --build .
```

### Dynamic Linking (Faster, requires runtime libraries)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Testing the Build

After build, verify functionality:

1. **Run the application**
   ```bash
   ./POS.exe
   ```

2. **Test migration utility**
   ```bash
   ./migrate.exe
   ```

3. **Test UI components**
   ```bash
   ./test_ui.exe
   ```

## Packaging for Distribution

```bash
# Create NSIS installer
"C:\Program Files (x86)\NSIS\makensis.exe" POS_Installer.nsi

# Or create ZIP archive
# Use Windows Explorer or:
powershell Compress-Archive -Path build\*.exe -DestinationPath POS_release.zip
```

## Continuous Integration

To set up GitHub Actions for Windows builds, create `.github/workflows/windows-build.yml`:

```yaml
name: Windows Build
on: [push, pull_request]
jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v2
      - uses: msys2/setup-msys2@v2
        with:
          msystem: MINGW64
      - name: Install dependencies
        shell: msys2 {0}
        run: |
          pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-wxWidgets mingw-w64-x86_64-lmdb
      - name: Build
        shell: msys2 {0}
        run: |
          mkdir build && cd build
          cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
          cmake --build . -j4
```

## Support & Resources

### Official Documentation
- [MSYS2 Installation](https://www.msys2.org/)
- [CMake Documentation](https://cmake.org/documentation/)
- [wxWidgets Windows Guide](https://docs.wxwidgets.org/stable/plat_msw.html)
- [LMDB](https://github.com/LMDB/lmdb)

### Useful Commands
```bash
# Check MSYS2 package list
pacman -Sl
pacman -Ss wxwidgets

# Update everything
pacman -Syu

# List installed packages
pacman -Q | grep -E "wx|lmdb|cmake"
```

## Troubleshooting Checklist

- [ ] MSYS2 fully updated (`pacman -Syu`)
- [ ] All dependencies installed (`pacman -S ...`)
- [ ] CMake version >= 3.16 (`cmake --version`)
- [ ] C++ compiler works (`g++ --version`)
- [ ] wx-config accessible (`which wx-config`)
- [ ] LMDB library found (`pkg-config --cflags lmdb`)
- [ ] Build directory clean (deleted old build/)
- [ ] Rebuild from scratchcd (`rm -rf build`, create new one)

---

For general cross-platform issues, see [CROSS_PLATFORM_BUILD.md](CROSS_PLATFORM_BUILD.md)
