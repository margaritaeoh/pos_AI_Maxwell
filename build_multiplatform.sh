#!/bin/bash
# Cross-Platform Build Script for POS Application
# Supports: Windows (MSYS2), macOS, and Linux

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_ROOT/build"
LOG_FILE="$PROJECT_ROOT/build_output.log"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}POS Application Cross-Platform Build${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Detect operating system
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macOS"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "Windows"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "Linux"
    else
        echo "Unknown"
    fi
}

# Detect architecture
detect_arch() {
    if [[ $(uname -m) == "arm64" ]] || [[ $(uname -m) == "aarch64" ]]; then
        echo "arm64"
    elif [[ $(uname -m) == "x86_64" ]] || [[ $(uname -m) == "AMD64" ]]; then
        echo "x86_64"
    else
        echo "unknown"
    fi
}

OS=$(detect_os)
ARCH=$(detect_arch)

echo -e "Detected OS: ${BLUE}$OS${NC} (${BLUE}$ARCH${NC})"
echo -e "Build directory: ${BLUE}$BUILD_DIR${NC}"
echo ""

# Check for required tools
check_requirements() {
    echo -e "${YELLOW}Checking requirements...${NC}"
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}❌ CMake not found. Please install CMake.${NC}"
        return 1
    fi
    echo -e "${GREEN}✓ CMake found${NC}"
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        echo -e "${RED}❌ C++ compiler not found. Please install g++ or clang.${NC}"
        return 1
    fi
    if command -v g++ &> /dev/null; then
        echo -e "${GREEN}✓ g++ found${NC}"
    else
        echo -e "${GREEN}✓ clang++ found${NC}"
    fi
    
    # Check for wxWidgets
    if ! pkg-config --exists wxwidgets 2>/dev/null; then
        echo -e "${RED}❌ wxWidgets not found. Please install wxWidgets development files.${NC}"
        echo ""
        case "$OS" in
            "macOS")
                echo -e "  ${YELLOW}Install with Homebrew:${NC}"
                echo "    brew install wxwidgets"
                ;;
            "Linux")
                echo -e "  ${YELLOW}On Ubuntu/Debian:${NC}"
                echo "    sudo apt-get install libwxgtk3.0-gtk3-dev"
                echo ""
                echo -e "  ${YELLOW}On Fedora:${NC}"
                echo "    sudo dnf install wxGTK-devel"
                ;;
            "Windows")
                echo -e "  ${YELLOW}Using MSYS2:${NC}"
                echo "    pacman -S mingw-w64-x86_64-wxWidgets"
                ;;
        esac
        return 1
    fi
    echo -e "${GREEN}✓ wxWidgets found${NC}"
    
    # Check for LMDB
    if ! pkg-config --exists lmdb 2>/dev/null; then
        echo -e "${RED}❌ LMDB not found. Please install LMDB development files.${NC}"
        echo ""
        case "$OS" in
            "macOS")
                echo -e "  ${YELLOW}Install with Homebrew:${NC}"
                echo "    brew install lmdb"
                ;;
            "Linux")
                echo -e "  ${YELLOW}On Ubuntu/Debian:${NC}"
                echo "    sudo apt-get install liblmdb-dev"
                echo ""
                echo -e "  ${YELLOW}On Fedora:${NC}"
                echo "    sudo dnf install lmdb-devel"
                ;;
            "Windows")
                echo -e "  ${YELLOW}Using MSYS2:${NC}"
                echo "    pacman -S mingw-w64-x86_64-lmdb"
                ;;
        esac
        return 1
    fi
    echo -e "${GREEN}✓ LMDB found${NC}"
    
    echo ""
    return 0
}

# Install dependencies for macOS
install_macos_dependencies() {
    echo -e "${YELLOW}Installing macOS dependencies via Homebrew...${NC}"
    
    if ! command -v brew &> /dev/null; then
        echo -e "${RED}Homebrew not found. Please install Homebrew first:${NC}"
        echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        return 1
    fi
    
    echo "Installing wxWidgets..."
    brew install wxwidgets || true
    
    echo "Installing CMake..."
    brew install cmake || true
    
    echo "Installing LMDB..."
    brew install lmdb || true
    
    echo -e "${GREEN}Dependencies installed${NC}"
    return 0
}

# Install dependencies for Linux
install_linux_dependencies() {
    echo -e "${YELLOW}Installing Linux dependencies...${NC}"
    
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y cmake build-essential libwxgtk3.0-gtk3-dev liblmdb-dev
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y cmake gcc-c++ wxGTK-devel lmdb-devel
    elif command -v pacman &> /dev/null; then
        sudo pacman -S cmake base-devel wxWidgets lmdb
    else
        echo -e "${RED}Unsupported Linux package manager${NC}"
        return 1
    fi
    
    echo -e "${GREEN}Dependencies installed${NC}"
    return 0
}

# Clean build directory
clean_build() {
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    mkdir -p "$BUILD_DIR"
    echo -e "${GREEN}Build directory cleaned${NC}"
}

# Configure with CMake
configure_build() {
    echo ""
    echo -e "${YELLOW}Configuring build with CMake...${NC}"
    
    cd "$BUILD_DIR"
    
    # Platform-specific CMake options
    CMAKE_OPTS="-DCMAKE_BUILD_TYPE=Release"
    
    case "$OS" in
        "macOS")
            CMAKE_OPTS="$CMAKE_OPTS -DCMAKE_OSX_ARCHITECTURES=x86_64"
            if [[ "$ARCH" == "arm64" ]]; then
                CMAKE_OPTS="$CMAKE_OPTS -DCMAKE_OSX_ARCHITECTURES=arm64"
            fi
            ;;
        "Windows")
            # MinGW specific options
            if [[ -d "/mingw64" ]]; then
                CMAKE_OPTS="$CMAKE_OPTS -G 'Unix Makefiles' -DCMAKE_C_COMPILER=/mingw64/bin/gcc -DCMAKE_CXX_COMPILER=/mingw64/bin/g++"
            fi
            ;;
    esac
    
    cmake $CMAKE_OPTS "$PROJECT_ROOT"
    
    echo -e "${GREEN}Configuration successful${NC}"
}

# Build the project
build_project() {
    echo ""
    echo -e "${YELLOW}Building project...${NC}"
    
    cd "$BUILD_DIR"
    
    # Use parallel builds with -j flag
    if [[ "$OS" == "macOS" ]] || [[ "$OS" == "Linux" ]]; then
        JOBS=$(sysctl -n hw.ncpu 2>/dev/null || nproc)
    else
        JOBS=4
    fi
    
    cmake --build . --config Release -j $JOBS
    
    echo -e "${GREEN}Build successful${NC}"
}

# Verify build output
verify_build() {
    echo ""
    echo -e "${YELLOW}Verifying build output...${NC}"
    
    cd "$PROJECT_ROOT"
    
    case "$OS" in
        "macOS")
            if [ -d "$BUILD_DIR/POS.app" ] || [ -f "$BUILD_DIR/POS" ]; then
                echo -e "${GREEN}✓ Main application built successfully${NC}"
            else
                echo -e "${RED}❌ Main application not found${NC}"
                return 1
            fi
            ;;
        "Windows")
            if [ -f "$BUILD_DIR/POS.exe" ]; then
                echo -e "${GREEN}✓ Main application built successfully: POS.exe${NC}"
                ls -lh "$BUILD_DIR/POS.exe"
            else
                echo -e "${RED}❌ Main application not found${NC}"
                return 1
            fi
            ;;
        "Linux")
            if [ -f "$BUILD_DIR/POS" ]; then
                echo -e "${GREEN}✓ Main application built successfully: POS${NC}"
                ls -lh "$BUILD_DIR/POS"
            else
                echo -e "${RED}❌ Main application not found${NC}"
                return 1
            fi
            ;;
    esac
    
    # Check optional tools
    if [ -f "$BUILD_DIR/migrate" ] || [ -f "$BUILD_DIR/migrate.exe" ]; then
        echo -e "${GREEN}✓ Migration utility built successfully${NC}"
    fi
    
    if [ -f "$BUILD_DIR/test_ui" ] || [ -f "$BUILD_DIR/test_ui.exe" ]; then
        echo -e "${GREEN}✓ Test UI built successfully${NC}"
    fi
    
    echo ""
}

# Create macOS app bundle resources
create_app_bundle_resources() {
    if [ "$OS" != "macOS" ]; then
        return 0
    fi
    
    echo -e "${YELLOW}Creating macOS app bundle resources...${NC}"
    
    APP_DIR="$BUILD_DIR/POS.app/Contents/Resources"
    mkdir -p "$APP_DIR"
    
    # Copy data directory to app bundle
    if [ -d "$PROJECT_ROOT/data" ]; then
        cp -r "$PROJECT_ROOT/data" "$APP_DIR/"
        echo -e "${GREEN}✓ Data files copied to app bundle${NC}"
    fi
}

# Print build summary
print_summary() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}Build Summary${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
    echo -e "Platform: ${GREEN}$OS ($ARCH)${NC}"
    echo -e "Build Directory: ${GREEN}$BUILD_DIR${NC}"
    echo ""
    
    case "$OS" in
        "macOS")
            echo -e "${GREEN}To run the application:${NC}"
            if [ -d "$BUILD_DIR/POS.app" ]; then
                echo "  open $BUILD_DIR/POS.app"
            else
                echo "  $BUILD_DIR/POS"
            fi
            ;;
        "Windows")
            echo -e "${GREEN}To run the application:${NC}"
            echo "  $BUILD_DIR\\POS.exe"
            ;;
        "Linux")
            echo -e "${GREEN}To run the application:${NC}"
            echo "  $BUILD_DIR/POS"
            ;;
    esac
    
    echo ""
    echo -e "Log file: ${GREEN}$LOG_FILE${NC}"
    echo ""
}

# Main build process
main() {
    # Redirect output to log file and terminal
    exec > >(tee -a "$LOG_FILE")
    exec 2>&1
    
    echo "Build started at $(date)"
    echo ""
    
    # Check requirements
    if ! check_requirements; then
        echo ""
        echo -e "${YELLOW}Attempting automatic dependency installation...${NC}"
        case "$OS" in
            "macOS")
                install_macos_dependencies || exit 1
                ;;
            "Linux")
                install_linux_dependencies || exit 1
                ;;
            "Windows")
                echo -e "${YELLOW}Please install dependencies manually using MSYS2 / Homebrew${NC}"
                exit 1
                ;;
        esac
        
        if ! check_requirements; then
            exit 1
        fi
    fi
    
    # Clean and configure
    clean_build || exit 1
    configure_build || exit 1
    
    # Build
    build_project || exit 1
    
    # Verify and create resources
    verify_build || exit 1
    create_app_bundle_resources
    
    # Print summary
    print_summary
    
    echo "Build completed at $(date)"
}

# Run main function
main "$@"
