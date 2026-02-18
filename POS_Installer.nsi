; POS System Installer Script
; Built with NSIS (Nullsoft Scriptable Install System)
; This script creates an installer for the POS application with all necessary files

!include "MUI2.nsh"
!include "x64.nsh"

; Basic Settings
Name "POS System v1.0"
OutFile "POS_System_Installer_v1.0.exe"
InstallDir "$PROGRAMFILES\POS System"
InstallDirRegKey HKCU "Software\POS System" "Install_Dir"

; Request admin rights for installation
RequestExecutionLevel admin

; Variables for shortcuts and uninstaller
Var StartMenuFolder

; MUI2 Settings
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU "Application" $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Spanish"

; Installer Sections
Section "Install POS Application" SecApp
  SetOutPath "$INSTDIR"
  
  ; Copy main executable
  File "pos.exe"
  
  ; Copy migration utility
  File "migrate.exe"
  
  ; Create data directories with empty CSVs for first-time setup
  CreateDirectory "$INSTDIR\data"
  CreateDirectory "$INSTDIR\data\lmdb"
  
  ; Create empty CSV files for first-time setup
  FileOpen $0 "$INSTDIR\data\usuarios.csv" w
  FileWrite $0 "email,password,nombre,tipo"
  FileWrite $0 "$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\data\productos.csv" w
  FileWrite $0 "id,nombre,descripcion,precio,cantidad,categoria"
  FileWrite $0 "$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\data\ventas.txt" w
  FileClose $0
  
  FileOpen $0 "$INSTDIR\data\reporte_inventario.csv" w
  FileWrite $0 "producto,cantidad,precio_unitario,valor_total"
  FileWrite $0 "$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\data\reporte_ventas.csv" w
  FileWrite $0 "fecha,producto,cantidad,precio_unitario,total"
  FileWrite $0 "$\r$\n"
  FileClose $0
  
  ; Store install folder in registry
  WriteRegStr HKCU "Software\POS System" "Install_Dir" "$INSTDIR"
  WriteUninstaller "$INSTDIR\uninstall.exe"
  
  ; Create Start Menu shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\POS System.lnk" "$INSTDIR\pos.exe"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
  
  SetAutoClose true
SectionEnd

Section "Install Database Migration Utility" SecMigrate
  ; Migration utility is optional but recommended to install
  SetOutPath "$INSTDIR"
  File "migrate.exe"
SectionEnd

; Uninstaller Section
Section "Uninstall"
  ; Remove files
  Delete "$INSTDIR\pos.exe"
  Delete "$INSTDIR\migrate.exe"
  Delete "$INSTDIR\uninstall.exe"
  
  ; Remove shortcuts
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"
  
  ; Remove registry keys
  DeleteRegKey /ifempty HKCU "Software\POS System"
  
  ; Note: Data directory is preserved on uninstall to prevent data loss
  ; Users can manually delete "$INSTDIR\data" if desired
  
  MessageBox MB_ICONINFORMATION "POS System has been uninstalled.$\n$\nData files in $INSTDIR\data have been preserved."
SectionEnd

; Descriptions for sections
LangString DESC_SecApp ${LANG_ENGLISH} "Install the main POS System application"
LangString DESC_SecMigrate ${LANG_ENGLISH} "Install the database migration utility"

LangString DESC_SecApp ${LANG_SPANISH} "Instalar la aplicación principal del Sistema POS"
LangString DESC_SecMigrate ${LANG_SPANISH} "Instalar la utilidad de migración de base de datos"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecApp} $(DESC_SecApp)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMigrate} $(DESC_SecMigrate)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
