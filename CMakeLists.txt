cmake_minimum_required(VERSION 3.16)
project(POS_Application CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets package (cross-platform)
find_package(wxWidgets REQUIRED COMPONENTS core base adv html)
include(${wxWidgets_USE_FILE})

# Find LMDB library (cross-platform)
find_package(LMDB REQUIRED)

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    set(APP_ICON_MACOSX "${CMAKE_CURRENT_SOURCE_DIR}/macos/POS.icns")
elseif(WIN32)
    # Windows-specific settings
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Source files - Main Application
set(MAIN_SOURCES
    main.cpp
    
    # UI Components
    ui/LoginWindow.cpp
    ui/LoginWindow.h
    ui/MainWindow.cpp
    ui/MainWindow.h
    ui/MenuInventario.cpp
    ui/MenuInventario.h
    ui/MenuReportes.cpp
    ui/MenuReportes.h
    ui/MenuVentas.cpp
    ui/MenuVentas.h
    ui/ProductoForm.cpp
    ui/ProductoForm.h
    ui/SignUpWindow.cpp
    ui/SignUpWindow.h
    ui/BarcodeScanner.cpp
    ui/BarcodeScanner.h
    ui/CantidadFraccionDialog.cpp
    ui/CantidadFraccionDialog.h
    ui/UICustomizationDialog.cpp
    ui/UICustomizationDialog.h
    ui/ThemeHelper.cpp
    ui/ThemeHelper.h
    
    # Models
    modelos/Producto.h
    modelos/Usuario.h
    modelos/Venta.h
    
    # Business Logic
    negocio/Autenticacion.cpp
    negocio/Autenticacion.h
    negocio/Inventario.cpp
    negocio/Inventario.h
    negocio/ProcesadorVentas.cpp
    negocio/ProcesadorVentas.h
    negocio/Reportes.cpp
    negocio/Reportes.h
    
    # Data Handling
    datos/ArchivoProductos.cpp
    datos/ArchivoProductos.h
    datos/ArchivoUsuarios.cpp
    datos/ArchivoUsuarios.h
    datos/ArchivoVentas.cpp
    datos/ArchivoVentas.h
    datos/DatabaseManager.cpp
    datos/DatabaseManager.h
    datos/IndexManager.cpp
    datos/IndexManager.h
    datos/PageInfo.h
)

# Source files - Migration Utility
set(MIGRATE_SOURCES
    migrate.cpp
    datos/MigrationUtility.cpp
    datos/MigrationUtility.h
    datos/DatabaseManager.cpp
    datos/DatabaseManager.h
    datos/IndexManager.cpp
    datos/IndexManager.h
    datos/PageInfo.h
)

# Source files - Test UI
set(TEST_UI_SOURCES
    test_ui.cpp
)

# Create main application executable
add_executable(POS ${MAIN_SOURCES})

# Link libraries
target_link_libraries(POS ${wxWidgets_LIBRARIES} LMDB::lmdb)

# Platform-specific link settings
if(APPLE)
    # macOS specific settings
    target_link_libraries(POS "-framework CoreServices")
elseif(WIN32)
    # Windows specific settings
    if(MSVC)
        target_link_libraries(POS setupapi)
    else()
        target_link_libraries(POS -static-libgcc -static-libstdc++  setupapi)
    endif()
endif()

# Create migration utility executable (no wxWidgets)
add_executable(migrate ${MIGRATE_SOURCES})
target_link_libraries(migrate LMDB::lmdb)

if(WIN32 AND NOT MSVC)
    target_link_libraries(migrate -static-libgcc -static-libstdc++)
endif()

# Create test UI executable
add_executable(test_ui ${TEST_UI_SOURCES})
target_link_libraries(test_ui ${wxWidgets_LIBRARIES})

if(WIN32 AND NOT MSVC)
    target_link_libraries(test_ui -static-libgcc -static-libstdc++)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(POS PRIVATE /W4)
    target_compile_options(migrate PRIVATE /W4)
    target_compile_options(test_ui PRIVATE /W4)
else()
    target_compile_options(POS PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(migrate PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(test_ui PRIVATE -Wall -Wextra -Wpedantic)
endif()

# macOS app bundle properties
if(APPLE)
    set_target_properties(POS PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.pos.application"
        MACOSX_BUNDLE_BUNDLE_NAME "POS"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2024. All rights reserved."
        MACOSX_BUNDLE_INFO_STRING "POS Application for macOS and Windows"
    )
    
    # Set icon if available
    if(EXISTS "${APP_ICON_MACOSX}")
        set_target_properties(POS PROPERTIES MACOSX_BUNDLE_ICON_FILE "POS.icns")
        set_source_files_properties("${APP_ICON_MACOSX}" PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
        target_sources(POS PRIVATE "${APP_ICON_MACOSX}")
    endif()
endif()

# Print build information
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "wxWidgets: ${wxWidgets_FOUND}")
message(STATUS "LMDB: ${LMDB_FOUND}")
