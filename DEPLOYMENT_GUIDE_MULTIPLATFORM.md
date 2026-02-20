# POS Application Deployment Guide

## Overview

This guide covers deploying the POS Application binaries on Windows and macOS. The application is now multiplatform compatible and can run on both operating systems.

## Table of Contents

1. [System Requirements](#system-requirements)
2. [Windows Deployment](#windows-deployment)
3. [macOS Deployment](#macos-deployment)
4. [Post-Installation Configuration](#post-installation-configuration)
5. [Troubleshooting](#troubleshooting)
6. [Updating](#updating)

## System Requirements

### Windows
- **OS**: Windows 7 or later (64-bit)
- **RAM**: 2 GB minimum
- **Storage**: 500 MB for application and data
- **Display**: 1024x768 minimum resolution
- **Dependencies**: 
  - Visual C++ Redistributables (if using MSVC build)
  - Or MinGW libraries (if using MinGW build)

### macOS
- **OS**: macOS 10.13 or later
- **Architecture**: Intel (x86_64) or Apple Silicon (arm64)
- **RAM**: 2 GB minimum
- **Storage**: 500 MB for application and data
- **Display**: 1024x768 minimum resolution

## Windows Deployment

### Option 1: Standalone Executable (Simplest)

1. **Locate the executable**
   ```
   build/POS.exe
   build/migrate.exe (optional - for database migration)
   ```

2. **Prepare distribution package**
   ```bash
   mkdir pos_release
   cp build/POS.exe pos_release/
   cp -r data/ pos_release/
   cp README.txt pos_release/readme.txt
   ```

3. **Create desktop shortcut** (optional)
   - Right-click `POS.exe`
   - Select "Send to" → "Desktop (create shortcut)"
   - Rename if desired

4. **Run the application**
   - Double-click `POS.exe`

### Option 2: Windows Installer (NSIS)

1. **Build the installer**
   ```bash
   "C:\Program Files (x86)\NSIS\makensis.exe" POS_Installer.nsi
   ```

2. **Distribute**
   - Share `POS_Setup.exe`
   - Users can run installer to install with shortcuts and uninstall support

3. **Installation process for users**
   - Double-click `POS_Setup.exe`
   - Follow wizard
   - Application installs to `C:\Program Files\POS` (or custom location)
   - Desktop shortcut created automatically

### Option 3: Portable ZIP Archive

1. **Create portable package**
   ```bash
   mkdir POS_Portable
   cp build/POS.exe POS_Portable/
   cp -r data/ POS_Portable/
   cp README.txt POS_Portable/
   
   # Compress
   powershell Compress-Archive -Path POS_Portable -DestinationPath POS_Portable.zip
   ```

2. **Share the ZIP**
   - Users extract anywhere
   - Run `POS.exe` from extracted folder
   - No installation required

### Deployment Checklist (Windows)
- [ ] Build in Release mode: `cmake --build . --config Release`
- [ ] Verify no dependencies outside build/ directory
- [ ] Test on clean Windows 7/10/11 machine
- [ ] Include data/ folder with CSV files
- [ ] Document any system requirements
- [ ] Create shortcut for users
- [ ] Test database operations (login, transactions)

## macOS Deployment

### Option 1: Application Bundle (Recommended)

1. **Build as macOS app bundle**
   ```bash
   mkdir -p build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . -j$(sysctl -n hw.ncpu)
   ```

2. **The app bundle is created at**
   ```
   build/POS.app
   ```

3. **Run the application**
   ```bash
   open build/POS.app
   # Or double-click in Finder
   ```

4. **Create DMG for distribution**
   ```bash
   cd build
   hdiutil create -volname POS -srcfolder . -ov -format UDZO -imagekey zlib-level=9 ../POS.dmg
   ```

5. **Installation for users**
   - Mount DMG (double-click)
   - Drag `POS.app` to Applications folder
   - Eject DMG
   - Run from Applications

### Option 2: Standalone App with Resources

1. **Prepare app bundle structure**
   ```bash
   mkdir -p POS_Distribution/POS.app/Contents/{MacOS,Resources}
   cp build/POS POS_Distribution/POS.app/Contents/MacOS/
   cp -r data/ POS_Distribution/POS.app/Contents/Resources/
   ```

2. **Make executable**
   ```bash
   chmod +x POS_Distribution/POS.app/Contents/MacOS/POS
   ```

3. **Create Info.plist**
   ```bash
   cat > POS_Distribution/POS.app/Contents/Info.plist << 'EOF'
   <?xml version="1.0" encoding="UTF-8"?>
   <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
   <plist version="1.0">
   <dict>
       <key>CFBundleDevelopmentRegion</key>
       <string>en</string>
       <key>CFBundleExecutable</key>
       <string>POS</string>
       <key>CFBundleIdentifier</key>
       <string>com.pos.application</string>
       <key>CFBundleInfoDictionaryVersion</key>
       <string>6.0</string>
       <key>CFBundleName</key>
       <string>POS</string>
       <key>CFBundlePackageType</key>
       <string>APPL</string>
       <key>CFBundleVersion</key>
       <string>1.0</string>
       <key>NSHighResolutionCapable</key>
       <true/>
   </dict>
   </plist>
   EOF
   ```

### Option 3: Code Signing (for Mac App Store/Distribution)

```bash
# First, get a signing certificate (Apple Developer account required)

# Sign the app
codesign --deep --force --verify --verbose --sign "Apple Development" \
  build/POS.app

# Verify signing
codesign --verify --verbose=2 build/POS.app
```

### Deployment Checklist (macOS)
- [ ] Build for correct architecture (Intel x86_64 or Apple Silicon arm64)
- [ ] Create app bundle with all resources
- [ ] Test on macOS 10.13+
- [ ] Verify `data/` folder is included in app bundle
- [ ] Test database operations (login, transactions)
- [ ] Code sign if distributing through App Store
- [ ] Create DMG for easy distribution

## Post-Installation Configuration

### Data Files Location

#### Windows
- **Portable/ZIP**: Data files in same directory as `.exe`
- **NSIS Installer**: `C:\Program Files\POS\data\`
- **User data**: `C:\Users\[username]\AppData\Local\POS\`

#### macOS
- **App Bundle**: `POS.app/Contents/Resources/data/`
- **User data**: `~/Library/Application Support/POS/`

### First Run Setup

1. **Launch Application**
   - Windows: Double-click `POS.exe`
   - macOS: Double-click `POS.app` or `open POS.app`

2. **Login**
   - Default test account (if seeds exist):
     - Username: (check usuarios.csv)
     - Password: (check usuarios.csv)

3. **Check Data Files**
   - Verify CSV files exist in `data/` folder:
     - `usuarios.csv` (users database)
     - `productos.csv` (products database)
     - `ventas.txt` (sales log)

### Configuration Files

The application stores settings in platform-specific locations:

**Windows:**
```
C:\Users\[user]\AppData\Local\POS\theme_settings.ini
```

**macOS:**
```
~/Library/Application Support/POS/theme_settings.ini
```

## Updating

### Updating on Windows

#### For Standalone Executable
1. Download new `POS.exe`
2. Replace old executable in your folder
3. Restart application

#### For NSIS Installation
1. Run new `POS_Setup.exe`
2. Follow installation wizard (can choose to replace existing)
3. Restart application

#### For Portable ZIP
1. Extract new version to new location
2. Copy user data (optional):
   ```bash
   copy old\data\*.txt new\data\
   copy old\data\*.csv new\data\
   ```
3. Use new version

### Updating on macOS

#### Using DMG
1. Mount new DMG
2. Drag new `POS.app` to Applications (replaces old version)
3. Eject DMG
4. Restart application

#### Using App Bundle Directly
1. Move old `POS.app` to Trash
2. Copy new `POS.app` to Applications
3. Restart application

## Troubleshooting

### Windows

#### "DLL not found" error
- Ensure all wxWidgets and LMDB DLL dependencies are available
- Consider using static build: `cmake -DSTATIC_BUILD=ON ..`
- Install Visual C++ Redistributables if using MSVC build

**Solution:**
```bash
# Rebuild with static linking
cd build
cmake -DCMAKE_CXX_FLAGS="-static-libgcc -static-libstdc++" ..
cmake --build . --config Release
```

#### Application won't start
1. Check Windows Defender/antivirus (may block executable)
2. Verify Visual C++ Redistributables installed
3. Check data folder exists with required CSV files
4. Look for `pos_startup.log` in application directory

#### Data files not found
- Ensure `data/` folder is in same directory as `POS.exe`
- Check file paths in CSV files are relative to application

### macOS

#### "POS.app can't be opened"
This usually means the app isn't code-signed for your system.

**Solution:**
```bash
# Remove quarantine attribute
xattr -d com.apple.quarantine /Applications/POS.app

# Or allow in Security settings:
# System Preferences → Security & Privacy → General
# Allow "POS" to open
```

#### "Unknown developer" warning
- This is normal for unsigned universal binaries
- Click "Open" -> "Open" to allow execution

**Solution:**
```bash
# Sign the app (if you have a certificate)
codesign -s - /Applications/POS.app
```

#### Crash on startup
1. Check `pos_startup.log` in application directory
2. Verify wxWidgets libraries are installed:
   ```bash
   brew install wxwidgets lmdb
   ```
3. Check data folder path

#### Performance issues on Apple Silicon
- Verify build includes ARM64 support:
  ```bash
  lipo -info build/POS
  # Should show: "Mach-O 64-bit executable arm64"
  ```

## Performance Monitoring

### Check Application Processes

**Windows:**
```cmd
tasklist | findstr POS.exe
```

**macOS:**
```bash
ps aux | grep POS
```

### Monitor Resource Usage

**Windows (PowerShell):**
```powershell
Get-Process | Where-Object {$_.Name -like "*POS*"} | Select-Object Name, Memory, ProcessorTime
```

**macOS:**
```bash
activity_monitor  # GUI tool
# Or command line:
ps aux | grep -i pos
```

## Deployment Distribution Methods

### GitHub Releases
1. Create release on GitHub
2. Upload binaries:
   - `POS.exe` (Windows)
   - `POS_Setup.exe` (Windows installer)
   - `POS.dmg` (macOS)
3. Include README with instructions

### Cloud Storage
- Google Drive / OneDrive / Dropbox
- Upload built executables
- Share download link

### Web Server
```bash
# Create simple server
python3 -m http.server 8000

# Access at http://localhost:8000/
```

## Rollback Procedures

### Windows
- Keep backup of previous version
- If update fails, restore from backup
- Data preserved in separate locations

### macOS
- Time Machine backups
- Keep previous `POS.app` in Downloads
- Data preserved in `~/Library/Application Support/POS/`

## License & Legal

- Include LICENSE file in distribution
- Ensure compliance with wxWidgets (wxWindows License - LGPL compatible)
- Ensure compliance with LMDB (OpenLDAP Public License)

## Support & Feedback

For issues or improvements:
1. Check logs: `pos_startup.log`
2. Test on clean machine
3. Report with OS version and error details
4. Submit issue to: https://github.com/margaritaeoh/pos_AI_Maxwell

---

Last updated: February 2026
