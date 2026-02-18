================================================================================
POS SYSTEM v1.0 - README
Build Date: February 17, 2026
================================================================================

QUICK START
===========

INSTALLATION:
  1. Right-click install.bat
  2. Select "Run as Administrator"
  3. Follow on-screen prompts
  4. Launch from Start Menu or C:\Program Files\POS System\pos.exe

DEFAULT INSTALLATION:
  Location: C:\Program Files\POS System\

FIRST TIME USE:
  • Database files are empty (usuarios.csv, productos.csv, ventas.txt)
  • Create admin user on first launch
  • Add products and begin sales recording
  • Use Tools → Data Import to migrate legacy data (if needed)

================================================================================
SYSTEM REQUIREMENTS
================================================================================

Minimum:
  • Windows 7 or later
  • 50 MB free disk space
  • Administrator rights for installation

Recommended:
  • Windows 10/11 (64-bit)
  • 4 GB RAM
  • 100 MB disk space
  • Modern processor

NO ADDITIONAL SETUP REQUIRED:
  ✓ All libraries included in executable
  ✓ No .NET Framework needed
  ✓ No additional DLL downloads
  ✓ Works standalone

================================================================================
WHAT'S INCLUDED
================================================================================

Main Application (pos.exe):
  ✓ Modern dark theme with VS Code appearance
  ✓ Light theme option for classic look
  ✓ Full Spanish language support with proper character encoding
  ✓ Inventory management with product tracking
  ✓ Sales transaction recording
  ✓ Comprehensive reporting module
  ✓ LMDB database backend with indexing
  ✓ Customizable UI colors, fonts, and button styles
  ✓ Theme persistence across sessions
  ✓ Resizable views for better usability
  ✓ Barcode scanner integration (COM and USB ports)
  ✓ ENTER key functionality in text fields

Migration Utility (migrate.exe):
  ✓ Convert legacy data formats to LMDB
  ✓ Data validation and integrity checking
  ✓ Database indexing and optimization

Empty Databases:
  ✓ usuarios.csv - User credentials (empty)
  ✓ productos.csv - Product inventory (empty)
  ✓ ventas.txt - Sales transactions (empty)
  ✓ reporte_inventario.csv - Inventory reports
  ✓ reporte_ventas.csv - Sales reports

Documentation:
  ✓ INSTALLATION_GUIDE.txt - Complete installation instructions
  ✓ DEPLOYMENT_GUIDE.txt - Distribution and deployment information
  ✓ REQUIREMENTS.txt - Feature list and system specifications
  ✓ DISTRIBUTION_CHECKLIST.txt - Package preparation guide
  ✓ BARCODE_SCANNER_GUIDE.txt - Barcode scanner setup and troubleshooting
  ✓ ENTER_KEY_FUNCTIONALITY_GUIDE.txt - ENTER key usage and workflows
  ✓ README.txt - This file

================================================================================
KEY FEATURES
================================================================================

AUTHENTICATION:
  • Email/password login
  • User role management
  • Session management

THEME SYSTEM:
  • Dark theme (default)
  • Light theme
  • Custom color palette editor
  • Font size customization
  • Button styling options
  • Theme saves automatically

INVENTORY MANAGEMENT:
  • Add/edit/delete products
  • Track stock levels
  • Organize by categories
  • Price management

SALES MANAGEMENT:
  ✓ Record transactions
  ✓ Support fractional quantities
  ✓ Customer information tracking
  ✓ Transaction timestamping
  ✓ Barcode scanner integration
  ✓ Auto-detect laser readers on COM/USB ports
  ✓ Real-time product entry from barcode data

REPORTING:
  ✓ Inventory reports
  ✓ Sales summaries
  ✓ Revenue tracking
  ✓ Export to CSV

================================================================================
KEYBOARD SHORTCUTS
================================================================================

  ENTER           Activate button or action in text fields
  Ctrl+T          Switch theme (Dark ↔ Light)
  Ctrl+P          Customize theme and UI (Personalizar Vistas)
  Ctrl+Q          Exit application
  Alt+F4          Close window
  Alt+File        Open File menu
  Alt+View        Open View menu
  Alt+Help        Open Help menu

================================================================================
INSTALLATION OPTIONS
================================================================================

Option A: Simple (Recommended)
  • Run: install.bat
  • Automatic dialog management
  • Works on all Windows versions
  • User must run as Administrator

Option B: Professional (Enterprise)
  • Requires NSIS installer tool
  • Creates: POS_System_Installer_v1.0.exe
  • Professional wizard interface
  • Better for large organizations
  • See DEPLOYMENT_GUIDE.txt

Option C: Portable
  • No installation required
  • Extract and run pos.exe
  • All data on USB or portable drive
  • Good for demonstrations

See INSTALLATION_GUIDE.txt for detailed instructions.

================================================================================
UNINSTALLATION
================================================================================

Method 1: From Program Files
  1. Navigate to: C:\Program Files\POS System
  2. Double-click: uninstall.bat
  3. Confirm uninstallation

Method 2: From Start Menu
  1. Start Menu → POS System → Uninstall
  2. Follow prompts

Method 3: Windows Control Panel
  1. Settings → Apps → Programs and Features
  2. Find: POS System v1.0
  3. Click Uninstall

NOTE: Your data files are preserved during uninstallation.
      To remove data: manually delete C:\Program Files\POS System\data

================================================================================
BARCODE SCANNER SETUP
================================================================================

CONNECTING A BARCODE SCANNER:

1. Compatible Ports:
   • COM1-COM9 (serial/USB-to-serial devices)
   • USB ports (native USB barcode readers)

2. To Connect:
   • In Ventas (Sales) tab, click "Connect Scanner"
   • Application auto-detects available readers
   • Alternatively, specify port manually (COM3, COM4, etc.)
   • Status indicator shows "Connected" when successful

3. Supported Equipment:
   • Laser barcode readers (standard RS-232)
   • USB barcode scanners (appear as COM port)
   • Old and new reader models supported
   • Standard baud rates: 9600 (default), 19200, 38400, 57600, 115200

4. Using the Scanner:
   • Focus on barcode input field in sales tab
   • Scan product barcode with laser reader
   • Product automatically added to transaction
   • Quantity can be adjusted with ENTER key
   • No manual typing required

5. Troubleshooting:
   • Scanner not detected? Check USB/COM port connection
   • Try different COM port if auto-detect fails
   • Ensure scanner sends CR/LF line endings
   • Check baud rate compatibility (default 9600)
   • Try "Connect Scanner" button again

================================================================================
TROUBLESHOOTING
================================================================================

"Administrator access required" error:
  → Right-click install.bat → "Run as Administrator"

"pos.exe not found":
  → Ensure pos.exe is in same folder as install.bat
  → Run installer from correct directory

Application won't launch:
  → Ensure Windows 7 or later
  → Try:
     1. Uninstall application
     2. Restart computer
     3. Reinstall application
  → Check Windows Event Viewer for errors

Spanish characters not displaying:
  → This is automatic with proper UTF-8 support
  → If not working: language support may need update
  → Try: Windows → Settings → Region & Language

database or data errors:
  → Check if data folder is read-only:
     Right-click data folder → Properties → uncheck "Read-only"
  → Verify sufficient disk space
  → Try running as Administrator

For more help, see: INSTALLATION_GUIDE.txt → Section 7: Troubleshooting

================================================================================
FIRST-TIME SETUP
================================================================================

1. After Installation
   • Database files are created but empty
   • This is normal - first-time setup

2. Launch Application
   • Click Start → POS System
   • Or double-click: C:\Program Files\POS System\pos.exe

3. Create Admin User
   • Enter email: admin@pos.local
   • Enter password: (choose secure password)
   • Confirm password
   • Click "Register" or "Create"

4. First Login
   • Email: admin@pos.local
   • Password: (your chosen password)
   • Click "Login"

5. Initial Configuration (Optional)
   • View → Personalizar Vistas (Ctrl+P)
   • Customize colors, fonts, button styles
   • Click OK to save

6. Start Working
   • Go to Inventario (Inventory) tab to add products
   • Go to Ventas (Sales) tab to record sales
   • Go to Reportes (Reports) tab to view reports

7. Data Migration (Optional, for Existing Data)
   • If migrating from legacy system:
   • Run: migrate.exe
   • Follow migration utility instructions
   • Restart pos.exe to see new data

================================================================================
FEATURES BY TAB
================================================================================

VENTAS (Sales)
  ├─ Record new sales transactions
  ├─ Select products and quantities
  ├─ Support for fractional units
  ├─ Customer information
  ├─ Transaction timestamp
  ├─ Barcode scanner integration
  ├─ Auto-detect laser readers (COM/USB ports)
  ├─ Real-time product entry via barcode input
  ├─ Connect/disconnect scanner button
  ├─ Scanner status indicator
  └─ Sales history view

INVENTARIO (Inventory)
  ├─ View all products
  ├─ Add new products
  ├─ Edit product information
  ├─ Delete products
  ├─ Track stock levels
  └─ Price management

REPORTES (Reports)
  ├─ Inventory reports
  ├─ Sales summaries
  ├─ Revenue tracking
  ├─ Product performance
  ├─ Export to CSV
  └─ Date range filtering

MENU BAR:
  
  Archivo (File)
    ├─ Exportar Datos (Export Data)
    └─ Salir (Exit)
  
  Vista (View)
    ├─ Cambiar Tema (Change Theme, Ctrl+T)
    └─ Personalizar Vistas (Customize Views, Ctrl+P)
  
  Ayuda (Help)
    ├─ Ver Ayuda (View Help)
    └─ Acerca de (About)

================================================================================
SUPPORT & DOCUMENTATION
================================================================================

Files Included:
  • INSTALLATION_GUIDE.txt - Complete step-by-step installation
  • DEPLOYMENT_GUIDE.txt - How to distribute to other computers
  • REQUIREMENTS.txt - Full feature list and system specs
  • DISTRIBUTION_CHECKLIST.txt - Package preparation guide
  • README.txt - This file
  • SCALABILITY_IMPROVEMENTS.md - Technical improvements
  • readme_migrate_utility.txt - Migration tool documentation

Online Resources:
  • See documentation files for detailed information
  • Check INSTALLATION_GUIDE.txt section 7 for troubleshooting
  • Review REQUIREMENTS.txt for complete feature list

Contact:
  For issues or questions, refer to:
  1. INSTALLATION_GUIDE.txt (Section 7: Troubleshooting)
  2. DEPLOYMENT_GUIDE.txt (if distribution issues)
  3. REQUIREMENTS.txt (feature questions)

================================================================================
IMPORTANT NOTES
================================================================================

Data Integrity:
  ✓ All data is stored in: C:\Program Files\POS System\data\
  ✓ Regular backups recommended
  ✓ Copy data/ folder periodically for safety
  ✓ Uninstallation preserves all data

Performance:
  ✓ Application uses LMDB for fast indexed access
  ✓ Indexing improves search and report performance
  ✓ First run may take a moment to initialize database
  ✓ Subsequent runs will be faster

Security:
  ✓ User passwords stored in usuarios.csv
  ✓ User management available to admin users
  ✓ Session management with login/logout
  ✓ File-based access control

Localization:
  ✓ Full Spanish language support
  ✓ UTF-8 encoding for special characters
  ✓ Spanish currency and date formats
  ✓ All menus and dialogs in Spanish

================================================================================
VERSION INFORMATION
================================================================================

Version: 1.0.0
Build Date: February 17, 2026
Platform: Windows 7 and later
Architecture: 64-bit (static-linked, works on 32-bit too)
Framework: wxWidgets 3.2 (statically linked)
Database: LMDB
Compiler: MinGW-w64 with C++17

Build Status: ✓ Successful (Exit Code 0)
Executable Sizes:
  • pos.exe: 17 MB
  • migrate.exe: 3.6 MB
  • test_ui.exe: 14 MB (optional, for testing)

================================================================================
LICENSE & LEGAL
================================================================================

This software is provided as-is for use by authorized personnel.

wxWidgets License: wxWindows Library Licence
LMDB License: OpenLDAP License
MinGW License: GNU GPL v3 and other open source licenses

All Spanish language features properly support UTF-8 encoding
and are compatible with modern Windows systems.

================================================================================
THANK YOU FOR USING POS SYSTEM
================================================================================

This comprehensive POS application includes:
  • Modern user interface with customizable themes
  • Complete inventory management
  • Sales transaction tracking
  • Advanced reporting capabilities
  • Professional installation process
  • Full Spanish language support
  • Enterprise-grade database backend

For detailed information, always refer to:
  1. INSTALLATION_GUIDE.txt - For setup questions
  2. REQUIREMENTS.txt - For features and capabilities
  3. DEPLOYMENT_GUIDE.txt - For distribution to other computers
  4. BARCODE_SCANNER_GUIDE.txt - For hardware setup and troubleshooting
  5. ENTER_KEY_FUNCTIONALITY_GUIDE.txt - For keyboard workflow optimization
  6. This file (README.txt) - For quick reference

Questions? See the comprehensive documentation files included.
Good luck with your POS System!

================================================================================
Build Information: c:\Cpp_gen_AI\pos\
Installation: C:\Program Files\POS System\
Data Location: C:\Program Files\POS System\data\
================================================================================
