# macOS Build Guide for POS Application

## Prerequisites

### 1. Install Xcode Command Line Tools
```bash
xcode-select --install
```

### 2. Install Homebrew (if not already installed)
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 3. Install Required Dependencies

#### Using Homebrew (Recommended)
```bash
# Install CMake
brew install cmake

# Install wxWidgets (this may take a few minutes)
brew install wxwidgets

# Install LMDB
brew install lmdb
```

#### Or use the automated build script
If you prefer, the build script can automatically install these for you:
```bash
bash build_multiplatform.sh
```

## Building the Application

### Option 1: Using the Multiplatform Build Script (Recommended)
```bash
# Make the script executable
chmod +x build_multiplatform.sh

# Run the build
bash build_multiplatform.sh
```

### Option 2: Manual CMake Build
```bash
# Create build directory
mkdir -p build
cd build

# Configure the project
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
cmake --build . --config Release -j$(sysctl -n hw.ncpu)

# The executable will be in:
# - build/POS.app (macOS app bundle)
# Or
# - build/POS (command-line executable)
```

### Option 3: Using Make Directly
```bash
mkdir -p build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
```

## Running the Application

### From the Build Directory
```bash
# If built as app bundle:
open build/POS.app

# If built as command-line executable:
./build/POS
```

### Creating a Standalone macOS App Bundle

To create a distributable app bundle with all dependencies:

```bash
cd build
macdeployqt POS.app -dmg  # If using Qt
# Or manually copy resources:
cp -r ../data POS.app/Contents/Resources/
```

## Troubleshooting

### CMake not found
```bash
brew install cmake
```

### wxWidgets compilation fails
```bash
# Try uninstalling and reinstalling
brew uninstall wxwidgets
brew install wxwidgets --build-from-source
```

### LMDB not found
```bash
brew install lmdb
# Then try rebuilding:
rm -rf build
mkdir build && cd build && cmake .. && make
```

### M1/M2 Mac (Apple Silicon) Specific Issues

For ARM64 architecture:
```bash
# Make sure you're using ARM64 compatible versions
brew list --versions | grep -E "wxwidgets|lmdb|cmake"

# If needed, force ARM64 compilation in CMake:
cd build
cmake -DCMAKE_OSX_ARCHITECTURES=arm64 ..
make -j$(sysctl -n hw.ncpu)
```

To build for both Intel and Apple Silicon (Universal Binary):
```bash
cd build
cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ..
make -j$(sysctl -n hw.ncpu)
```

### Permission denied when running build script
```bash
chmod +x build_multiplatform.sh
./build_multiplatform.sh
```

### "wxWidgets not found" during CMake
```bash
# Find the wxWidgets installation:
wx-config --prefix

# Or reinstall:
brew reinstall wxwidgets
```

## Development Setup

### Using an IDE

#### CLion
1. Open the project folder
2. CLion will automatically detect the CMakeLists.txt
3. Configure CMake in Settings → Build, Execution, Deployment → CMake

#### VS Code
1. Install CMake Tools extension
2. Open the project folder
3. Select the kit (should auto-detect)
4. Build with Cmd+Shift+B

#### Xcode
```bash
# Generate Xcode project
mkdir build_xcode
cd build_xcode
cmake -G Xcode -DCMAKE_BUILD_TYPE=Release ..
open POS.xcodeproj
```

## Building Additional Utilities

The build system also creates:
- `migrate` - Database migration utility
- `test_ui` - UI testing tool

These are built automatically and can be found in the `build` directory.

## Cross-Platform Build

To build for both macOS and Windows from the same CMakeLists.txt:

### macOS to Windows Cross-Compilation (Not Recommended)
It's generally better to build on the target platform. Use the Windows build guide for Windows builds.

### Building on Windows for macOS
This is not supported. Build on a Mac for the best results.

## Creating a Distributable Package

### DMG Package (macOS Installer)
```bash
# After building in build/ directory
cd build

# Create DMG
hdiutil create -volname POS -srcfolder . -ov -format UDZO -imagekey zlib-level=9 ../POS.dmg

# The DMG will be created in the parent directory
```

### App Store Preparation
1. Sign the application
2. Notarize with Apple
3. Create app bundle with all resources

For details, see: https://developer.apple.com/macos/

## Performance Optimization

For production builds with optimizations:
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-O3 -march=native" ..
make -j$(sysctl -n hw.ncpu)
```

## Environment Variables

Optional environment variables for build customization:
```bash
# Use a specific number of cores
export JOBS=4
bash build_multiplatform.sh

# Use a specific C++ compiler
export CXX=clang++
bash build_multiplatform.sh

# Enable verbose output
export VERBOSE=1
bash build_multiplatform.sh
```

## Support

For issues specific to wxWidgets on macOS:
- https://docs.wxwidgets.org/

For LMDB issues:
- https://github.com/LMDB/lmdb

## Next Steps

After building successfully:
1. Test the application: `open build/POS.app`
2. Verify all features work correctly
3. Commit the CMakeLists.txt and build scripts to version control
4. Consider setting up CI/CD for automated builds
