@echo off
REM POS System Installer - Batch Version
REM This script installs the POS application and sets up empty databases

setlocal enabledelayedexpansion

echo ========================================
echo POS System v1.0 - Installation Script
echo ========================================
echo.

REM Check if running as admin
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: This installer must be run as Administrator
    echo.
    echo Please right-click this script and select "Run as Administrator"
    pause
    exit /b 1
)

REM Define installation path
set INSTALL_PATH=%PROGRAMFILES%\POS System
set DATA_PATH=%INSTALL_PATH%\data
set LMDB_PATH=%DATA_PATH%\lmdb

echo Installation Path: %INSTALL_PATH%
echo.

REM Check if pos.exe exists in current directory
if not exist "pos.exe" (
    echo ERROR: pos.exe not found in current directory
    echo Please run this installer from the POS build directory
    pause
    exit /b 1
)

REM Create installation directory
echo Creating installation directories...
if not exist "%INSTALL_PATH%" mkdir "%INSTALL_PATH%"
if not exist "%DATA_PATH%" mkdir "%DATA_PATH%"
if not exist "%LMDB_PATH%" mkdir "%LMDB_PATH%"

REM Copy main executable
echo Copying POS application...
copy /Y "pos.exe" "%INSTALL_PATH%\pos.exe"
if %errorlevel% neq 0 (
    echo ERROR: Failed to copy pos.exe
    pause
    exit /b 1
)

REM Copy migration utility if exists
if exist "migrate.exe" (
    echo Copying Migration utility...
    copy /Y "migrate.exe" "%INSTALL_PATH%\migrate.exe"
)

REM Create empty data files for first-time setup
echo Creating empty databases...

REM Create empty usuarios.csv
(
    echo email,password,nombre,tipo
) > "%DATA_PATH%\usuarios.csv"

REM Create empty productos.csv
(
    echo id,nombre,descripcion,precio,cantidad,categoria
) > "%DATA_PATH%\productos.csv"

REM Create empty ventas.txt
type nul > "%DATA_PATH%\ventas.txt"

REM Create empty reporte_inventario.csv
(
    echo producto,cantidad,precio_unitario,valor_total
) > "%DATA_PATH%\reporte_inventario.csv"

REM Create empty reporte_ventas.csv
(
    echo fecha,producto,cantidad,precio_unitario,total
) > "%DATA_PATH%\reporte_ventas.csv"

REM Create uninstall script
echo Creating uninstall script...
(
    echo @echo off
    echo echo Uninstalling POS System...
    echo REM Remove files
    echo del /Q "%INSTALL_PATH%\pos.exe"
    echo del /Q "%INSTALL_PATH%\migrate.exe"
    echo del /Q "%INSTALL_PATH%\uninstall.bat"
    echo REM Remove directory
    echo echo.
    echo echo WARNING: Data files in "%DATA_PATH%" will be preserved
    echo echo to prevent accidental data loss.
    echo echo.
    echo echo If you want to remove all data, manually delete:
    echo echo   "%INSTALL_PATH%"
    echo echo.
    echo pause
    echo exit /b 0
) > "%INSTALL_PATH%\uninstall.bat"

REM Create Start Menu shortcut
echo Creating Start Menu shortcuts...
set MENU_PATH=%APPDATA%\Microsoft\Windows\Start Menu\Programs\POS System
if not exist "%MENU_PATH%" mkdir "%MENU_PATH%"

REM Create shortcut using VBS (more compatible than using mklink)
(
    echo Set oWS = WScript.CreateObject("WScript.Shell"^)
    echo sLinkFile = "%MENU_PATH%\POS System.lnk"
    echo Set oLink = oWS.CreateShortcut(sLinkFile^)
    echo oLink.TargetPath = "%INSTALL_PATH%\pos.exe"
    echo oLink.WorkingDirectory = "%INSTALL_PATH%"
    echo oLink.Description = "POS System Application"
    echo oLink.Save
) > "%TEMP%\create_shortcut.vbs"

cscript //nologo "%TEMP%\create_shortcut.vbs"
del /Q "%TEMP%\create_shortcut.vbs"

REM Create uninstall shortcut
(
    echo Set oWS = WScript.CreateObject("WScript.Shell"^)
    echo sLinkFile = "%MENU_PATH%\Uninstall POS System.lnk"
    echo Set oLink = oWS.CreateShortcut(sLinkFile^)
    echo oLink.TargetPath = "%INSTALL_PATH%\uninstall.bat"
    echo oLink.Description = "Uninstall POS System"
    echo oLink.Save
) > "%TEMP%\create_uninstall_shortcut.vbs"

cscript //nologo "%TEMP%\create_uninstall_shortcut.vbs"
del /Q "%TEMP%\create_uninstall_shortcut.vbs"

REM Add to Windows Registry
echo Registering application...
reg add "HKCU\Software\POS System" /v "Install_Dir" /d "%INSTALL_PATH%" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\POS System" ^
    /v "DisplayName" /d "POS System v1.0" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\POS System" ^
    /v "UninstallString" /d "%INSTALL_PATH%\uninstall.bat" /f

echo.
echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo Installation Directory: %INSTALL_PATH%
echo Data Directory: %DATA_PATH%
echo.
echo The following files have been created:
echo   - pos.exe (Main Application^)
echo   - migrate.exe (Migration Utility^)
echo   - data\usuarios.csv (Empty users database^)
echo   - data\productos.csv (Empty products database^)
echo   - data\ventas.txt (Empty sales database^)
echo   - data\reporte_inventario.csv (Inventory reports^)
echo   - data\reporte_ventas.csv (Sales reports^)
echo.
echo You can now launch POS System from:
echo   - Start Menu: POS System
echo   - Installation Directory: %INSTALL_PATH%\pos.exe
echo.
echo To uninstall, run: %INSTALL_PATH%\uninstall.bat
echo.
pause
exit /b 0
