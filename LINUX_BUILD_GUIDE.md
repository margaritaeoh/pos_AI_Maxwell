# Linux Build Guide for POS Application

## System Requirements

- **Linux Distribution**: Ubuntu 18.04+, Debian 10+, Fedora 30+, Arch Linux
- **Architecture**: x86-64 (64-bit) or ARM64
- **RAM**: 2 GB minimum, 4 GB recommended
- **Disk Space**: 500 MB for build tools + dependencies

## Prerequisites

### Ubuntu / Debian-based Systems

```bash
# Update package lists
sudo apt-get update

# Install build tools and dependencies
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libwxgtk3.0-gtk3-dev \
    liblmdb-dev

# Optional: Install development tools
sudo apt-get install -y \
    gdb \
    clang \
    ninja-build
```

### Fedora / RHEL

```bash
# Install build tools and dependencies
sudo dnf install -y \
    gcc \
    gcc-c++ \
    cmake \
    git \
    pkgconfig \
    wxGTK-devel \
    lmdb-devel

# Optional: Install development tools
sudo dnf install -y \
    gdb \
    clang \
    ninja-build
```

### Arch Linux

```bash
# Install build tools and dependencies
sudo pacman -S \
    base-devel \
    cmake \
    git \
    wxgtk3 \
    lmdb

# Optional: Install development tools
sudo pacman -S \
    gdb \
    clang \
    ninja
```

### openSUSE

```bash
# Install build tools and dependencies
sudo zypper install -y \
    gcc \
    gcc-c++ \
    cmake \
    git \
    pkg-config \
    wxGTK3-devel \
    lmdb-devel
```

## Verify Installation

```bash
# Check all tools are installed
gcc --version
cmake --version
wx-config --version
pkg-config --cflags --libs lmdb
```

All commands should return version information.

## Building the Application

### Method 1: Using the Automated Build Script (Recommended)

```bash
# Clone or navigate to project directory
cd /path/to/pos_project

# Make script executable
chmod +x build_multiplatform.sh

# Run the script
./build_multiplatform.sh
```

The script automatically:
- Detects your Linux distribution
- Checks for required dependencies
- Offers to install missing packages
- Configures with optimal settings
- Builds in parallel using available cores

### Method 2: Manual CMake Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure the project
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build using make (determines CPU cores automatically)
cmake --build . -j$(nproc)

# Alternative: Use make directly
make -j$(nproc)
```

### Method 3: Using Ninja Build System

For faster builds with better error messages:

```bash
# Install ninja (if not already installed)
# Ubuntu: sudo apt-get install ninja-build
# Fedora: sudo dnf install ninja-build
# Arch: sudo pacman -S ninja

# Create build directory
mkdir -p build
cd build

# Configure with Ninja
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..

# Build
ninja
```

### Method 4: Using Clang Compiler

```bash
mkdir -p build
cd build

# Configure with Clang
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_BUILD_TYPE=Release ..

cmake --build . -j$(nproc)
```

## Running the Application

After successful build:

```bash
# From the build directory
./build/POS

# Or from project root
./build/POS
```

### Creating Desktop Shortcut

Create `~/.local/share/applications/pos.desktop`:

```ini
[Desktop Entry]
Type=Application
Name=POS Application
Exec=/path/to/pos/build/POS
Icon=application-x-executable
Categories=Office;PointOfSale;
```

Then:
```bash
chmod +x ~/.local/share/applications/pos.desktop
```

## Development Workflow

### Build with Debug Symbols

For debugging and development:

```bash
mkdir -p build_debug
cd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j$(nproc)
```

Then run under debugger:

```bash
gdb ./POS
# In gdb: run
# Use Ctrl+C to break, then debug commands
# Common: b (breakpoint), s (step), c (continue), p var (print)
```

### Incremental Build

After modifying source files:

```bash
cd build
cmake --build .  # Only rebuilds changed files
```

### IDE Integration

#### Visual Studio Code

1. **Install Extensions**
   ```bash
   # From VS Code or command line
   code --install-extension ms-vscode.cpptools
   code --install-extension ms-vscode.cmake-tools
   ```

2. **Open Project**
   ```bash
   code .
   ```

3. **Configure**
   - Press Ctrl+Shift+P
   - Select "CMake: Select a Kit"
   - Choose your compiler (GCC, Clang)

4. **Build and Run**
   - Ctrl+Shift+B to build
   - F5 to debug

#### CLion (JetBrains)

1. **Open Project**
   - File → Open → Project folder

2. **Configure Toolchain**
   - Settings → Build → Toolchains
   - Select Linux Toolchain

3. **Build**
   - Build → Build Project (Ctrl+F9)

4. **Run**
   - Run → Run (Shift+F10)

#### Qt Creator

1. **Install Qt Creator**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install qtcreator

   # Fedora
   sudo dnf install qt-creator

   # Arch
   sudo pacman -S qt-creator
   ```

2. **Open Project**
   - File → Open File or Project
   - Select CMakeLists.txt

3. **Configure and Build**
   - Qt Creator auto-detects CMake
   - Click Build Project

## Performance Optimization

### Release Build with Optimizations

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native -flto" ..
cmake --build . -j$(nproc)
```

### Link-Time Optimization (LTO)

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE ..
cmake --build . -j$(nproc)
```

### Parallel Build Jobs

```bash
# Use specific number of jobs (default: all cores)
cmake --build . -j4  # Use 4 jobs
cmake --build . -j   # Use all available cores
```

## Troubleshooting

### CMake version too old
```bash
# Check version
cmake --version

# If older than 3.16, upgrade
sudo apt-get install -y cmake  # Ubuntu/Debian
sudo dnf install -y cmake      # Fedora
sudo pacman -S cmake           # Arch
```

### "wxWidgets not found" during CMake

```bash
# Check wxWidgets installation
wx-config --version

# Check libraries
pkg-config --cflags --libs wxwidgets

# Reinstall if needed
sudo apt-get install --reinstall libwxgtk3.0-gtk3-dev
```

### LMDB linking errors

```bash
# Verify LMDB installation
pkg-config --cflags --libs lmdb

# Reinstall
sudo apt-get install --reinstall liblmdb-dev
```

### Undefined reference errors

```bash
# Clean and rebuild with verbose output
cd build
rm -rf *
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build . --verbose

# Check if all libraries are present
ldd ./POS
```

### Permission denied

```bash
chmod +x build_multiplatform.sh
chmod +x build/POS
```

### Compiler not found

```bash
# Check which compilers are available
which gcc
which g++
which clang

# Install if missing
sudo apt-get install -y gcc g++
```

### Out of memory during build

```bash
# Use fewer parallel jobs
cmake --build . -j2
```

### wxWidgets version mismatch

```bash
# Check installed version
wx-config --version
wx-config --cflags --libs

# May need to install specific version
# Ubuntu: try wxPython3.0 compatible version
# Consider building wxWidgets from source if version issues persist
```

## Creating Distribution Packages

### Debian/Ubuntu (.deb)

Install fpm (Effing Package Management):

```bash
sudo apt-get install -y ruby-dev
sudo gem install fpm
```

Create package:

```bash
mkdir -p /tmp/pos_deb/usr/local/bin
cp build/POS /tmp/pos_deb/usr/local/bin/

mkdir -p /tmp/pos_deb/usr/share/pos
cp -r data /tmp/pos_deb/usr/share/pos/

fpm -s dir -t deb -n pos-app -v 1.0 -C /tmp/pos_deb -p pos-app_1.0_amd64.deb
```

Install:
```bash
sudo dpkg -i pos-app_1.0_amd64.deb
pos-app
```

### RPM Package (Fedora/RedHat)

```bash
mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

# Create SPEC file and build
# See https://rpm-guide.readthedocs.io/
```

### Portable AppImage

Install appimagetool:
```bash
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
```

Create AppImage:
```bash
mkdir pos.AppDir
cp -r build/POS pos.AppDir/
./appimagetool-x86_64.AppImage pos.AppDir POS-x86_64.AppImage
```

## Testing the Build

```bash
# Run the main application
./build/POS

# Run migration utility
./build/migrate

# Run UI tests
./build/test_ui

# Test with arguments
./build/POS --help  # If implemented
```

## Continuous Integration (GitHub Actions)

Create `.github/workflows/linux-build.yml`:

```yaml
name: Linux Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        compiler: [gcc, clang]
    steps:
      - uses: actions/checkout@v2
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y libwxgtk3.0-gtk3-dev liblmdb-dev cmake
      
      - name: Configure
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release ..
      
      - name: Build
        run: |
          cd build
          cmake --build . -j$(nproc)
      
      - name: Test
        run: |
          ./build/POS --help || true
```

## Performance Monitoring

Monitor build performance:

```bash
# Time the build
time cmake --build . -j$(nproc)

# Monitor system resources
watch -n 1 'ps aux | grep cmake'

# Check disk usage
du -sh build/
```

## Dependency Management

### Check dependencies

```bash
# Runtime dependencies
ldd ./build/POS

# Build dependencies
cmake --graph-only > dependencies.txt
```

### Update dependencies

```bash
# Ubuntu/Debian
sudo apt-get update && sudo apt-get upgrade

# Fedora
sudo dnf update

# Arch
sudo pacman -Syu
```

## Environment Variables

Custom build configuration:

```bash
# Use specific compiler
export CXX=clang++
export CC=clang

# Compiler flags
export CXXFLAGS="-O3 -march=native"
export LDFLAGS="-flto"

# Then build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Build System Alternatives

### Premake5

Install and generate:
```bash
# Requires premake5 installation
premake5 gmake2
make -j$(nproc) config=release64
```

### Meson

```bash
# Install Meson
pip install meson

# Configure
meson setup builddir -Dbuildtype=release

# Build
meson compile -C builddir
```

## Static vs Dynamic Linking

### Fully Static Build

```bash
cmake -DCMAKE_CXX_FLAGS="-static" \
      -DCMAKE_EXE_LINKER_FLAGS="-static" ..
cmake --build .
```

### Check linked libraries

```bash
ldd ./build/POS
nm -D ./build/POS
```

## Support & Resources

### Official Documentation
- [CMake Linux Guide](https://cmake.org/cmake/help/latest/)
- [wxWidgets Linux Guide](https://docs.wxwidgets.org/stable/plat_x11.html)
- [LMDB Documentation](https://github.com/LMDB/lmdb)

### Useful Linux Commands
```bash
# Find libraries
ldconfig -p | grep wx
ldconfig -p | grep lmdb

# Check package contents
dpkg -L libwxgtk3.0-gtk3-dev  # What files are in the package

# Search for packages
apt-cache search wxwidgets
dnf search wxgtk
```

## Optimization Checklist

- [ ] Using Release build type
- [ ] Optimizations enabled (-O3)
- [ ] Link-time optimization enabled
- [ ] Native architecture flags (-march=native)
- [ ] Parallel build jobs configured
- [ ] Unnecessary debug info removed
- [ ] Dependencies properly linked

---

For general cross-platform issues, see [CROSS_PLATFORM_BUILD.md](CROSS_PLATFORM_BUILD.md)

Last updated: February 2026
