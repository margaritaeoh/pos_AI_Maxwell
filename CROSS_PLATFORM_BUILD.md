# Cross-Platform Build Guide

This guide explains how to build the POS Application on Windows, macOS, and Linux using the unified CMake build system.

## Overview

The POS Application now supports cross-platform compilation using:
- **CMake** - Build configuration (CMakeLists.txt)
- **wxWidgets** - Cross-platform GUI framework
- **LMDB** - Cross-platform database

## Supported Platforms

| Platform | Status | Minimum Version |
|----------|--------|-----------------|
| Windows (MinGW64/MSYS2) | ✅ Supported | Windows 7+ |
| Windows (MSVC) | ✅ Supported | Windows 7+ |
| macOS | ✅ Supported | macOS 10.13+ |
| Linux (Ubuntu/Debian) | ✅ Supported | Ubuntu 18.04+ |
| Linux (Fedora) | ✅ Supported | Fedora 30+ |
| Linux (Arch) | ✅ Supported | Latest |

## Quick Start

### Windows (MSYS2/MinGW64)

1. **Install MSYS2** from https://www.msys2.org/
2. **Open MSYS2 MinGW64 terminal** and run:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-wxWidgets mingw-w64-x86_64-lmdb
   ```
3. **Build:**
   ```bash
   cd /c/path/to/project
   bash build_multiplatform.sh
   ```

### macOS

1. **Install Xcode Command Line Tools:**
   ```bash
   xcode-select --install
   ```
2. **Install dependencies via Homebrew:**
   ```bash
   brew install cmake wxwidgets lmdb
   ```
3. **Build:**
   ```bash
   bash build_multiplatform.sh
   ```

### Linux (Ubuntu/Debian)

1. **Install dependencies:**
   ```bash
   sudo apt-get update
   sudo apt-get install -y cmake build-essential libwxgtk3.0-gtk3-dev liblmdb-dev
   ```
2. **Build:**
   ```bash
   bash build_multiplatform.sh
   ```

## Platform-Specific Guides

For detailed instructions, see:
- [macOS Build Guide](MACOS_BUILD_GUIDE.md)
- [Windows Build Guide](WINDOWS_BUILD_GUIDE.md)
- [Linux Build Guide](LINUX_BUILD_GUIDE.md)

## Build Methods

### Method 1: Automated Build Script (Recommended)

```bash
chmod +x build_multiplatform.sh
./build_multiplatform.sh
```

**Features:**
- Automatic dependency detection
- Platform-specific optimizations
- Comprehensive error checking
- Detailed build logging

### Method 2: Manual CMake Build

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release -j$(nproc)
```

### Method 3: IDE Integration

**Visual Studio Code:**
- Install CMake Tools extension
- Open project folder
- Select compiler kit
- Build with Ctrl+Shift+B

**CLion:**
- Open project folder
- CMake configuration automatic
- Build -> Build Project

**Xcode (macOS):**
```bash
mkdir build_xcode && cd build_xcode
cmake -G Xcode -DCMAKE_BUILD_TYPE=Release ..
open POS.xcodeproj
```

## Project Structure

```
pos/
├── CMakeLists.txt              # Cross-platform build configuration
├── build_multiplatform.sh      # Automated build script
├── CROSS_PLATFORM_BUILD.md     # This file
├── MACOS_BUILD_GUIDE.md        # macOS-specific guide
├── WINDOWS_BUILD_GUIDE.md      # Windows-specific guide
├── LINUX_BUILD_GUIDE.md        # Linux-specific guide
├── main.cpp                    # Application entry point
├── ui/                         # UI components (cross-platform)
├── modelos/                    # Data models
├── negocio/                    # Business logic
├── datos/                      # Data persistence
├── data/                       # Resources (copied to app bundle)
└── build/                      # Build output (created after build)
```

## Build Output

After successful build, executables are located in the `build/` directory:

### Windows
- `build/POS.exe` - Main application
- `build/migrate.exe` - Database migration utility
- `build/test_ui.exe` - UI testing tool

### macOS
- `build/POS.app/` - Application bundle
- `build/POS` - Command-line executable
- `build/migrate` - Database migration utility
- `build/test_ui` - UI testing tool

### Linux
- `build/POS` - Main application
- `build/migrate` - Database migration utility
- `build/test_ui` - UI testing tool

## Running the Application

### Windows
```bash
build\POS.exe
```

### macOS
```bash
# From app bundle (recommended)
open build/POS.app

# Or direct executable
./build/POS
```

### Linux
```bash
./build/POS
```

## Troubleshooting

### CMake not found
Install CMake for your platform:
```bash
# macOS
brew install cmake

# Ubuntu/Debian
sudo apt-get install cmake

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-cmake

# Fedora
sudo dnf install cmake
```

### wxWidgets not found
```bash
# macOS
brew install wxwidgets
brew doctor

# Ubuntu/Debian
sudo apt-get install libwxgtk3.0-gtk3-dev

# Fedora
sudo dnf install wxGTK-devel

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-wxWidgets
```

### LMDB configuration issues
```bash
# Force rebuild
rm -rf build
mkdir build && cd build

# Configure with verbose output
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build . --verbose
```

### Permission denied
```bash
chmod +x build_multiplatform.sh
./build_multiplatform.sh
```

### Build fails with "undefined reference"
This usually means a library wasn't found or linked correctly:
```bash
# Clean and rebuild
rm -rf build
bash build_multiplatform.sh

# Or check library installation
pkg-config --cflags --libs wxwidgets lmdb
```

## Development Workflow

### 1. Clone/Setup Repository
```bash
git clone https://github.com/margaritaeoh/pos_AI_Maxwell.git
cd pos_AI_Maxwell
```

### 2. Build for Development
```bash
# With debug symbols
mkdir -p build_debug
cd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### 3. Modify Code
- Edit source files in `ui/`, `negocio/`, `datos/`, etc.
- No changes needed to CMakeLists.txt unless adding new files

### 4. Rebuild
```bash
cd build
cmake --build . -j$(nproc)
```

### 5. Test
```bash
./POS  # macOS/Linux
./POS.exe  # Windows
```

### 6. Commit
```bash
git add .
git commit -m "Your changes"
git push
```

## Adding New Source Files

When adding new `.cpp` or `.h` files:

1. Update `CMakeLists.txt` to include the new files in the appropriate source list
2. Rebuild the project

Example:
```cmake
set(MAIN_SOURCES
    # ... existing files
    ui/NewComponent.cpp
    ui/NewComponent.h
    # ... rest
)
```

## GitHub Actions / CI/CD

To set up continuous integration, add `.github/workflows/build.yml`:

```yaml
name: Multi-Platform Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies (Ubuntu)
        if: runner.os == 'Linux'
        run: |
          sudo apt-get update
          sudo apt-get install -y libwxgtk3.0-gtk3-dev liblmdb-dev
      - name: Install dependencies (macOS)
        if: runner.os == 'macOS'
        run: brew install wxwidgets lmdb
      - name: Install dependencies (Windows)
        if: runner.os == 'Windows'
        run: choco install wxwidgets lmdb
      - name: Build
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release ..
          cmake --build . -j$(nproc || echo 4)
```

## Release/Distribution

### Creating Release Packages

#### Windows (.exe installer)
See [WINDOWS_BUILD_GUIDE.md](WINDOWS_BUILD_GUIDE.md) for NSIS packaging

#### macOS (.dmg package)
See [MACOS_BUILD_GUIDE.md](MACOS_BUILD_GUIDE.md) for DMG creation

#### Linux (.deb, .rpm packages)
See [LINUX_BUILD_GUIDE.md](LINUX_BUILD_GUIDE.md) for packaging details

## Performance Tips

### Release Build Optimizations
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_CXX_FLAGS="-O3 -march=native" ..
cmake --build . -j$(nproc)
```

### Parallel Build
```bash
# Use all available cores
cmake --build . -j$(nproc)  # macOS/Linux
cmake --build . -j%NUMBER_OF_PROCESSORS%  # Windows
```

### Incremental Build
```bash
cd build
cmake --build .  # Only rebuilds changed files
```

## Compiler Support

- **GCC**: 7.0+ (Linux default)
- **Clang**: 5.0+ (macOS default)
- **MSVC**: 2019+ (Windows)
- **MinGW**: 7.0+ (Windows via MSYS2)

## CMake Versions

Minimum CMake version: **3.16**

Current recommended: **3.20+**

## License & Contributing

See [README.md](README.md) for project information.

For contributions:
1. Fork the repository
2. Create a feature branch
3. Build and test on all platforms
4. Submit a pull request

## Support

For issues:
1. Check platform-specific guides
2. Review build log: `build_output.log`
3. Open an issue on GitHub with:
   - Platform and version
   - Build command used
   - Full error output from `build_output.log`

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [wxWidgets Documentation](https://docs.wxwidgets.org/)
- [LMDB Documentation](https://github.com/LMDB/lmdb)
- [Git Workflow](https://git-scm.com/book/en/v2)

---

Last updated: February 2026
