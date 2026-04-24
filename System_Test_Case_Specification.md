# System Test Case Specification
## POS AI Maxwell – Point of Sale System
**Version:** 1.1  
**Date:** 2026-04-24  
**Standard:** DO-178C / IEC 62304 (adapted for desktop software)  
**Status:** Active

---

## 1. Introduction

### 1.1 Purpose
This document defines the system-level test cases for the POS AI Maxwell application. Each test case is derived from the system requirements documented in `REQUIREMENTS.txt` and validates that the implemented software functions correctly under normal, boundary, and error conditions.

### 1.2 Scope
These test cases cover:
- User authentication and session management
- Password hashing and security
- Inventory management (CRUD operations, pagination)
- Sales transaction processing
- Reporting and data export
- Barcode scanner integration
- Theme system and UI customization
- Data persistence

### 1.3 Definitions
| Term | Definition |
|------|-----------|
| SUT  | System Under Test (pos.exe) |
| TC   | Test Case |
| UT   | Unit Test |
| ST   | System Test |
| PASS | Test result meets the acceptance criterion |
| FAIL | Test result does not meet the acceptance criterion |

---

## 2. Test Environment

| Component | Specification |
|-----------|--------------|
| OS | Windows 10/11 (64-bit) |
| Compiler | MinGW64 / g++ with C++17 |
| UI Framework | wxWidgets 3.2 (static) |
| Database | LMDB (optional, CSV default) |
| Data files | `data/productos.csv`, `data/usuarios.csv`, `data/ventas.txt` |
| Build tool | `build.sh` (MSYS2 / Git Bash) |

---

## 3. Test Cases — Authentication Module

### TC-AUTH-001: Successful Login with Valid Credentials
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-001 |
| **Requirement** | REQ-1.3 (User Authentication) |
| **Priority** | High |
| **Preconditions** | Application started; `data/usuarios.csv` contains user `admin` with password `1234` |
| **Steps** | 1. Enter username `admin` <br> 2. Enter password `1234` <br> 3. Click "Ingresar" |
| **Expected Result** | MainWindow opens showing the correct username and role; LoginWindow closes |
| **Pass Criterion** | MainWindow title displays "Punto de Venta"; user label reads "admin (Administrador)" |

---

### TC-AUTH-002: Login with Incorrect Password
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-002 |
| **Requirement** | REQ-1.3 |
| **Priority** | High |
| **Preconditions** | Application started; valid user `admin` exists |
| **Steps** | 1. Enter username `admin` <br> 2. Enter password `wrongpass` <br> 3. Click "Ingresar" |
| **Expected Result** | Error label displays "Usuario o contraseña incorrectos"; LoginWindow remains open |
| **Pass Criterion** | No navigation to MainWindow occurs |

---

### TC-AUTH-003: Login with Non-Existent User
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-003 |
| **Requirement** | REQ-1.3 |
| **Priority** | High |
| **Preconditions** | Application started |
| **Steps** | 1. Enter username `unknown_user` <br> 2. Enter password `anypass` <br> 3. Click "Ingresar" |
| **Expected Result** | Error label shown; no session created |
| **Pass Criterion** | LoginWindow stays open; no crash |

---

### TC-AUTH-004: Login Without Password (requierePassword = false)
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-004 |
| **Requirement** | REQ-1.3 |
| **Priority** | Medium |
| **Preconditions** | A standard user exists with `requierePassword=0` in `usuarios.csv` |
| **Steps** | 1. Enter username of that user <br> 2. Check "Entrar sin contraseña" <br> 3. Click "Ingresar" |
| **Expected Result** | Login succeeds; MainWindow opens |
| **Pass Criterion** | No error shown; correct user displayed in MainWindow |

---

### TC-AUTH-005: Password Hash Upgrade on Login (Legacy → SHA-256)
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-005 |
| **Requirement** | REQ-6.1 (Password Security) |
| **Priority** | High |
| **Preconditions** | `usuarios.csv` contains a user with a legacy `H:<plaintext>` hash |
| **Steps** | 1. Log in successfully with the user's plaintext password <br> 2. Inspect `usuarios.csv` after login |
| **Expected Result** | The hash stored in `usuarios.csv` is updated to `H2:<sha256hex>` format |
| **Pass Criterion** | No `H:` prefix remains for the logged-in user; subsequent logins with the same password still succeed |

---

### TC-AUTH-006: New User Registration
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-006 |
| **Requirement** | REQ-1.3 |
| **Priority** | High |
| **Preconditions** | Application started; SignUpWindow accessible |
| **Steps** | 1. Click "Registrarse" on LoginWindow <br> 2. Fill in username, password, confirm password <br> 3. Select role <br> 4. Click "Registrarse" |
| **Expected Result** | User saved to `usuarios.csv` with `H2:` hash format; success message shown |
| **Pass Criterion** | New user appears in CSV; hash begins with `H2:` |

---

### TC-AUTH-007: Duplicate User Registration Rejected
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-007 |
| **Requirement** | REQ-1.3 |
| **Priority** | Medium |
| **Preconditions** | User `admin` already exists |
| **Steps** | 1. Open SignUpWindow <br> 2. Enter username `admin` <br> 3. Click "Registrarse" |
| **Expected Result** | Error message "El usuario ya existe" shown; no duplicate written to CSV |
| **Pass Criterion** | `usuarios.csv` unchanged |

---

### TC-AUTH-008: Logout Returns to Login Screen
| Field | Value |
|-------|-------|
| **ID** | TC-AUTH-008 |
| **Requirement** | REQ-1.3 |
| **Priority** | High |
| **Preconditions** | User is logged in to MainWindow |
| **Steps** | 1. Click "Cerrar Sesión" button |
| **Expected Result** | MainWindow closes; LoginWindow opens with empty fields |
| **Pass Criterion** | No previous session data visible in LoginWindow |

---

## 4. Test Cases — Inventory Module

### TC-INV-001: Add New Product
| Field | Value |
|-------|-------|
| **ID** | TC-INV-001 |
| **Requirement** | REQ-1.5 |
| **Priority** | High |
| **Preconditions** | Admin user logged in; Inventario tab active |
| **Steps** | 1. Click "Agregar" button <br> 2. Fill in product name, price, stock, category <br> 3. Click "Guardar" |
| **Expected Result** | Product appears in inventory grid; `productos.csv` updated |
| **Pass Criterion** | New row visible in grid; CSV contains new entry |

---

### TC-INV-002: Edit Existing Product
| Field | Value |
|-------|-------|
| **ID** | TC-INV-002 |
| **Requirement** | REQ-1.5 |
| **Priority** | High |
| **Preconditions** | At least one product exists in inventory |
| **Steps** | 1. Select product in grid <br> 2. Click "Editar" <br> 3. Modify price <br> 4. Click "Guardar" |
| **Expected Result** | Grid reflects updated price; CSV updated |
| **Pass Criterion** | Updated price persists after application restart |

---

### TC-INV-003: Pagination — Navigate to Next Page
| Field | Value |
|-------|-------|
| **ID** | TC-INV-003 |
| **Requirement** | REQ-1.5, SCALABILITY |
| **Priority** | Medium |
| **Preconditions** | More than 50 products in inventory |
| **Steps** | 1. Open Inventario tab <br> 2. Click "Siguiente →" button |
| **Expected Result** | Grid shows page 2 of products; page indicator updates |
| **Pass Criterion** | "Página 2 de N" shown; different products visible |

---

### TC-INV-004: Role Restriction — Standard User Cannot Add Products
| Field | Value |
|-------|-------|
| **ID** | TC-INV-004 |
| **Requirement** | REQ-1.5, REQ-6.1 |
| **Priority** | High |
| **Preconditions** | Standard (non-admin) user logged in |
| **Steps** | 1. Navigate to Inventario tab <br> 2. Observe "Agregar" button state |
| **Expected Result** | "Agregar" button is disabled or hidden for standard users |
| **Pass Criterion** | No product creation possible without admin role |

---

## 5. Test Cases — Sales Module

### TC-VEN-001: Search Product by Name
| Field | Value |
|-------|-------|
| **ID** | TC-VEN-001 |
| **Requirement** | REQ-1.6 |
| **Priority** | High |
| **Preconditions** | Products loaded; Ventas tab active |
| **Steps** | 1. Type part of a product name in the search field |
| **Expected Result** | Suggestions list populates with matching products |
| **Pass Criterion** | At least one relevant suggestion appears within 1 second |

---

### TC-VEN-002: Add Product Line to Sale
| Field | Value |
|-------|-------|
| **ID** | TC-VEN-002 |
| **Requirement** | REQ-1.6 |
| **Priority** | High |
| **Preconditions** | Product found in search; Ventas tab active |
| **Steps** | 1. Select product from suggestions <br> 2. Enter quantity <br> 3. Click "Agregar" |
| **Expected Result** | Line appears in sale grid; total updated |
| **Pass Criterion** | Grid row shows product, quantity, and calculated price |

---

### TC-VEN-003: Finalize Sale
| Field | Value |
|-------|-------|
| **ID** | TC-VEN-003 |
| **Requirement** | REQ-1.6 |
| **Priority** | High |
| **Preconditions** | At least one line added to current sale |
| **Steps** | 1. Click "Finalizar" button |
| **Expected Result** | Sale recorded in `ventas.txt`; grid cleared; total reset to $0.00 |
| **Pass Criterion** | `ventas.txt` contains new sale entry with timestamp |

---

### TC-VEN-004: Cancel Sale
| Field | Value |
|-------|-------|
| **ID** | TC-VEN-004 |
| **Requirement** | REQ-1.6 |
| **Priority** | Medium |
| **Preconditions** | At least one line added to current sale |
| **Steps** | 1. Click "Cancelar" button |
| **Expected Result** | Sale lines cleared; total reset; nothing written to `ventas.txt` |
| **Pass Criterion** | Sale count in `ventas.txt` unchanged |

---

### TC-VEN-005: Fractional Quantity Input
| Field | Value |
|-------|-------|
| **ID** | TC-VEN-005 |
| **Requirement** | REQ-1.6 |
| **Priority** | Medium |
| **Preconditions** | Product selected in Ventas tab |
| **Steps** | 1. Enter fractional quantity (e.g., 0.5) in quantity field <br> 2. Click "Agregar" |
| **Expected Result** | Line added with fractional quantity; price computed correctly |
| **Pass Criterion** | Total reflects fractional unit price |

---

### TC-VEN-006: Barcode Scanner Input Triggers Product Search
| Field | Value |
|-------|-------|
| **ID** | TC-VEN-006 |
| **Requirement** | REQ-1.1 |
| **Priority** | Medium |
| **Preconditions** | Product with matching barcode exists; scanner connected or barcode entered manually |
| **Steps** | 1. Type barcode into barcode field <br> 2. Press ENTER |
| **Expected Result** | Product auto-populated in search field; line ready to be added |
| **Pass Criterion** | Correct product identified from barcode |

---

## 6. Test Cases — Reporting Module

### TC-REP-001: Generate Inventory Report
| Field | Value |
|-------|-------|
| **ID** | TC-REP-001 |
| **Requirement** | REQ-1.7 |
| **Priority** | Medium |
| **Preconditions** | Admin user logged in; Reportes tab visible |
| **Steps** | 1. Click "Generar Reporte de Inventario" |
| **Expected Result** | Grid populated with current inventory data |
| **Pass Criterion** | All products shown with correct stock levels |

---

### TC-REP-002: Generate Sales Report
| Field | Value |
|-------|-------|
| **ID** | TC-REP-002 |
| **Requirement** | REQ-1.7 |
| **Priority** | Medium |
| **Preconditions** | At least one sale recorded |
| **Steps** | 1. Click "Generar Reporte de Ventas" |
| **Expected Result** | Grid populated with sales summary |
| **Pass Criterion** | Revenue totals match recorded sales |

---

### TC-REP-003: Export Report to CSV
| Field | Value |
|-------|-------|
| **ID** | TC-REP-003 |
| **Requirement** | REQ-1.7 |
| **Priority** | Medium |
| **Preconditions** | Report generated (TC-REP-001 or TC-REP-002 passed) |
| **Steps** | 1. Click export button <br> 2. Confirm file path |
| **Expected Result** | CSV file created with report data |
| **Pass Criterion** | CSV readable in Excel/LibreOffice with correct columns |

---

## 7. Test Cases — Theme & UI Customization

### TC-THEME-001: Toggle Dark / Light Theme
| Field | Value |
|-------|-------|
| **ID** | TC-THEME-001 |
| **Requirement** | REQ-1.4 |
| **Priority** | Low |
| **Preconditions** | User logged in; MainWindow visible |
| **Steps** | 1. Press Ctrl+T or use View → Cambiar Tema |
| **Expected Result** | All UI panels switch between dark and light styles |
| **Pass Criterion** | Background/text colors visually change; no crash |

---

### TC-THEME-002: Customize Theme via Dialog
| Field | Value |
|-------|-------|
| **ID** | TC-THEME-002 |
| **Requirement** | REQ-1.4 |
| **Priority** | Low |
| **Preconditions** | User logged in |
| **Steps** | 1. Press Ctrl+P <br> 2. Select custom colors and fonts <br> 3. Click OK |
| **Expected Result** | Theme applied to MainWindow; saved to `custom_theme.ini` |
| **Pass Criterion** | Colors match selection; theme persists after app restart |

---

### TC-THEME-003: Theme Persistence Across Sessions
| Field | Value |
|-------|-------|
| **ID** | TC-THEME-003 |
| **Requirement** | REQ-1.4 |
| **Priority** | Low |
| **Preconditions** | Custom theme saved in TC-THEME-002 |
| **Steps** | 1. Close and reopen application <br> 2. Log in |
| **Expected Result** | Custom theme loaded automatically from `custom_theme.ini` |
| **Pass Criterion** | Colors match those chosen in previous session |

---

## 8. Test Cases — Data Persistence & Recovery

### TC-DATA-001: Data Survives Application Restart
| Field | Value |
|-------|-------|
| **ID** | TC-DATA-001 |
| **Requirement** | REQ-1.8 |
| **Priority** | High |
| **Preconditions** | Products and sales exist |
| **Steps** | 1. Record products and sales <br> 2. Close application <br> 3. Reopen and log in |
| **Expected Result** | All previously recorded data visible |
| **Pass Criterion** | Inventory and sales counts match pre-restart state |

---

### TC-DATA-002: Graceful Handling of Missing Data File
| Field | Value |
|-------|-------|
| **ID** | TC-DATA-002 |
| **Requirement** | REQ-5.4 |
| **Priority** | Medium |
| **Preconditions** | `data/productos.csv` renamed or deleted |
| **Steps** | 1. Start application <br> 2. Log in |
| **Expected Result** | Application starts; empty inventory shown; no crash |
| **Pass Criterion** | Error logged; UI functional with empty state |

---

## 9. Test Cases — Password Security

### TC-SEC-001: SHA-256 Hash Verification
| Field | Value |
|-------|-------|
| **ID** | TC-SEC-001 |
| **Requirement** | REQ-6.1 |
| **Priority** | High |
| **Preconditions** | PasswordHasher.h compiled into test binary |
| **Steps** | 1. Call `PasswordHasher::hashPassword("admin", "1234")` <br> 2. Verify output begins with `H2:` <br> 3. Verify length = 3 + 64 = 67 characters |
| **Expected Result** | Hash is deterministic, 67 chars, starts with `H2:` |
| **Pass Criterion** | Same input always produces same output; different inputs produce different hashes |

---

### TC-SEC-002: Legacy Hash Backward Compatibility
| Field | Value |
|-------|-------|
| **ID** | TC-SEC-002 |
| **Requirement** | REQ-6.1 |
| **Priority** | High |
| **Preconditions** | User with legacy hash `H:1234` in `usuarios.csv` |
| **Steps** | 1. Call `PasswordHasher::verifyPassword("admin", "1234", "H:1234")` |
| **Expected Result** | Returns `true` |
| **Pass Criterion** | Legacy users can still log in; hash is upgraded after successful login |

---

### TC-SEC-003: Incorrect Password Rejected
| Field | Value |
|-------|-------|
| **ID** | TC-SEC-003 |
| **Requirement** | REQ-6.1 |
| **Priority** | High |
| **Preconditions** | None |
| **Steps** | 1. Call `PasswordHasher::verifyPassword("admin", "wrong", "H2:<correct_hash>")` |
| **Expected Result** | Returns `false` |
| **Pass Criterion** | No false positives from hash collisions |

---

## 10. Traceability Matrix

| Requirement | Test Case(s) |
|-------------|-------------|
| REQ-1.1 Barcode Scanner | TC-VEN-006 |
| REQ-1.3 Authentication | TC-AUTH-001 – TC-AUTH-008 |
| REQ-1.4 Theme System | TC-THEME-001 – TC-THEME-003 |
| REQ-1.5 Inventory | TC-INV-001 – TC-INV-004 |
| REQ-1.6 Sales | TC-VEN-001 – TC-VEN-006 |
| REQ-1.7 Reporting | TC-REP-001 – TC-REP-003 |
| REQ-1.8 Data Persistence | TC-DATA-001 – TC-DATA-002 |
| REQ-6.1 Security | TC-AUTH-005, TC-SEC-001 – TC-SEC-003 |

---

## 11. Test Results Log

| TC ID | Date | Tester | Result | Notes |
|-------|------|--------|--------|-------|
| TC-AUTH-001 | — | — | Pending | — |
| TC-AUTH-002 | — | — | Pending | — |
| TC-AUTH-003 | — | — | Pending | — |
| TC-AUTH-004 | — | — | Pending | — |
| TC-AUTH-005 | — | — | Pending | Automated upgrade on login |
| TC-AUTH-006 | — | — | Pending | — |
| TC-AUTH-007 | — | — | Pending | — |
| TC-AUTH-008 | — | — | Pending | — |
| TC-INV-001 | — | — | Pending | — |
| TC-INV-002 | — | — | Pending | — |
| TC-INV-003 | — | — | Pending | — |
| TC-INV-004 | — | — | Pending | — |
| TC-VEN-001 | — | — | Pending | — |
| TC-VEN-002 | — | — | Pending | — |
| TC-VEN-003 | — | — | Pending | — |
| TC-VEN-004 | — | — | Pending | — |
| TC-VEN-005 | — | — | Pending | — |
| TC-VEN-006 | — | — | Pending | — |
| TC-REP-001 | — | — | Pending | — |
| TC-REP-002 | — | — | Pending | — |
| TC-REP-003 | — | — | Pending | — |
| TC-THEME-001 | — | — | Pending | — |
| TC-THEME-002 | — | — | Pending | — |
| TC-THEME-003 | — | — | Pending | — |
| TC-DATA-001 | — | — | Pending | — |
| TC-DATA-002 | — | — | Pending | — |
| TC-SEC-001 | — | — | Pending | — |
| TC-SEC-002 | — | — | Pending | — |
| TC-SEC-003 | — | — | Pending | — |

---

*End of System Test Case Specification*
