#!/bin/bash
# Redirect ALL output to both console and log file
LOG_FILE="$(cd "$(dirname "$0")" && pwd)/build_output.log"
exec > >(tee -a "$LOG_FILE")
exec 2>&1

echo "Compilation STARTED"
echo "Log file: $LOG_FILE"
echo ""

set -x

# Determine project root as a POSIX path (MSYS) and collect source files
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
echo "Project root (POSIX path): $SCRIPT_DIR"
# Build array of source files so spaces in filenames are handled correctly
# Exclude test_ui.cpp and migrate.cpp from main build (they have their own wxIMPLEMENT_APP)
SRC=("$SCRIPT_DIR"/main.cpp "$SCRIPT_DIR"/ui/*.cpp "$SCRIPT_DIR"/negocio/*.cpp "$SCRIPT_DIR"/datos/*.cpp)

# Compile with wx-config, output explicitly to project dir
OUT_EXE="$SCRIPT_DIR/pos.exe"

# Choose wx-config: prefer mingw64 one if available
if [ -x "/mingw64/bin/wx-config" ]; then
  WX_CONFIG="/mingw64/bin/wx-config"
elif command -v wx-config >/dev/null 2>&1; then
  WX_CONFIG="$(command -v wx-config)"
else
  echo "ERROR: wx-config not found. Install wxWidgets (mingw-w64-x86_64-wxWidgets) or add wx-config to PATH."
  exit 1
fi

echo "Using wx-config: $WX_CONFIG"
# Try static wxWidgets build (--static) or DLL (default)
WX_CXXFLAGS="$($WX_CONFIG --static --cxxflags 2>/dev/null || $WX_CONFIG --cxxflags)"
WX_LIBS_RAW="$($WX_CONFIG --static --libs 2>/dev/null || $WX_CONFIG --libs)"
# Remove -shared-libgcc if present and add static link options for libgcc/libstdc++
WX_LIBS="$(echo "$WX_LIBS_RAW" | sed 's/-shared-libgcc//g') -static-libgcc -static-libstdc++"

# Check for LMDB
LMDB_LIBS="-llmdb"
if ! pkg-config --exists lmdb 2>/dev/null; then
  echo "WARNING: LMDB not found via pkg-config. Using default -llmdb"
fi

echo "Compiling to: $OUT_EXE"

# Remove old executables if they exist
if [ -f "$OUT_EXE" ]; then
    echo "Removing old executable: $OUT_EXE"
    rm -f "$OUT_EXE"
fi

# Build main application
echo "g++ -std=c++17 ${WX_CXXFLAGS} \"${SRC[@]}\" -o \"$OUT_EXE\" ${WX_LIBS} ${LMDB_LIBS}"
g++ -std=c++17 ${WX_CXXFLAGS} "${SRC[@]}" -o "$OUT_EXE" ${WX_LIBS} ${LMDB_LIBS}
EXIT_CODE=$?

# Also build migration utility (unless main build failed)
OUT_MIGRATE="$SCRIPT_DIR/migrate.exe"
OUT_TEST_UI="$SCRIPT_DIR/test_ui.exe"

if [ $EXIT_CODE -eq 0 ]; then
    echo ""
    echo "Building migration utility: $OUT_MIGRATE"
    
    if [ -f "$OUT_MIGRATE" ]; then
        rm -f "$OUT_MIGRATE"
    fi
    
    # Only compile MigrationUtility, DatabaseManager, and IndexManager (no wxWidgets needed)
    MIGRATE_SRC=("$SCRIPT_DIR/migrate.cpp" "$SCRIPT_DIR/datos/MigrationUtility.cpp" "$SCRIPT_DIR/datos/DatabaseManager.cpp" "$SCRIPT_DIR/datos/IndexManager.cpp")
    echo "g++ -std=c++17 \"${MIGRATE_SRC[@]}\" -o \"$OUT_MIGRATE\" ${LMDB_LIBS} -static-libgcc -static-libstdc++"
    g++ -std=c++17 "${MIGRATE_SRC[@]}" -o "$OUT_MIGRATE" ${LMDB_LIBS} -static-libgcc -static-libstdc++
    MIGRATE_EXIT=$?
    
    if [ $MIGRATE_EXIT -eq 0 ]; then
        echo "Migration utility compiled successfully"
        ls -l "$OUT_MIGRATE"
    else
        echo "Warning: Migration utility compilation failed (exit code $MIGRATE_EXIT)"
    fi

    echo ""
    echo "Building test UI: $OUT_TEST_UI"
    
    if [ -f "$OUT_TEST_UI" ]; then
        rm -f "$OUT_TEST_UI"
    fi
    
    # Compile simple test UI (no dependencies, just wxWidgets)
    echo "g++ -std=c++17 ${WX_CXXFLAGS} \"$SCRIPT_DIR/test_ui.cpp\" -o \"$OUT_TEST_UI\" ${WX_LIBS}"
    g++ -std=c++17 ${WX_CXXFLAGS} "$SCRIPT_DIR/test_ui.cpp" -o "$OUT_TEST_UI" ${WX_LIBS}
    TEST_UI_EXIT=$?
    
    if [ $TEST_UI_EXIT -eq 0 ]; then
        echo "Test UI compiled successfully"
        ls -l "$OUT_TEST_UI"
    else
        echo "Warning: Test UI compilation failed (exit code $TEST_UI_EXIT)"
    fi
fi

set +x

# Write summary to log file
{
    echo "================================"
    echo "BUILD SUMMARY"
    echo "================================"
    echo "Date: $(date)"
    echo "Exit Code: $EXIT_CODE"
    echo ""
    
    if [ $EXIT_CODE -eq 0 ]; then
        echo "✓ MAIN BUILD SUCCESSFUL"
        echo "Executable: $OUT_EXE"
        ls -lh "$OUT_EXE"
        echo ""
    else
        echo "✗ MAIN BUILD FAILED (exit code $EXIT_CODE)"
        echo "Check error messages above"
        echo ""
    fi
    
    if [ $EXIT_CODE -eq 0 ]; then
        echo "✓ MIGRATION UTILITY BUILD"
        if [ $MIGRATE_EXIT -eq 0 ]; then
            echo "✓ migrate.exe compiled successfully"
            ls -lh "$OUT_MIGRATE"
        else
            echo "✗ migrate.exe compilation failed (exit code $MIGRATE_EXIT)"
        fi
    else
        echo "✗ Migration utility skipped (main build failed)"
    fi
    
    echo ""
    echo "Log file: $LOG_FILE"
    echo "================================"
} | tee -a "$LOG_FILE"

echo ""
echo "Full build log saved to: $LOG_FILE"
echo "Open this file to check for errors."
read -p "Press Enter to exit..."
