#include <iostream>
#include <string>
#include "datos/MigrationUtility.h"

/**
 * @brief Migration utility program to convert CSV/TXT data to LMDB
 * 
 * Usage:
 *   migrate [--help]
 *   migrate [--db-path <path>] [--csv-productos <path>] [--csv-usuarios <path>] [--txt-ventas <path>]
 */

void printHelp() {
    std::cout << "\n=== POS System CSV to LMDB Migration Utility ===" << std::endl;
    std::cout << "\nUsage:" << std::endl;
    std::cout << "  migrate                                  - Migrate using default paths" << std::endl;
    std::cout << "  migrate --help                          - Show this help message" << std::endl;
    std::cout << "  migrate --db-path <path>                - Set LMDB database path (default: inventario.db)" << std::endl;
    std::cout << "  migrate --csv-productos <path>          - Set productos CSV path (default: data/productos.csv)" << std::endl;
    std::cout << "  migrate --csv-usuarios <path>           - Set usuarios CSV path (default: data/usuarios.csv)" << std::endl;
    std::cout << "  migrate --txt-ventas <path>             - Set ventas TXT path (default: data/ventas.txt)" << std::endl;
    
    std::cout << "\nExample:" << std::endl;
    std::cout << "  migrate --db-path ./db/pos.db --csv-productos data/productos.csv" << std::endl;
    
    std::cout << "\nDefaults:" << std::endl;
    std::cout << "  Database:       inventario.db" << std::endl;
    std::cout << "  Productos CSV:  data/productos.csv" << std::endl;
    std::cout << "  Usuarios CSV:   data/usuarios.csv" << std::endl;
    std::cout << "  Ventas TXT:     data/ventas.txt" << std::endl;
    
    std::cout << "\nNotes:" << std::endl;
    std::cout << "  - This is a one-way migration (CSV → LMDB)" << std::endl;
    std::cout << "  - Existing LMDB data will be appended to (not cleared)" << std::endl;
    std::cout << "  - All CSV files must exist" << std::endl;
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    std::string dbPath = "inventario.db";
    std::string csvProductos = "data/productos.csv";
    std::string csvUsuarios = "data/usuarios.csv";
    std::string txtVentas = "data/ventas.txt";
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
        else if (arg == "--db-path" && i + 1 < argc) {
            dbPath = argv[++i];
        }
        else if (arg == "--csv-productos" && i + 1 < argc) {
            csvProductos = argv[++i];
        }
        else if (arg == "--csv-usuarios" && i + 1 < argc) {
            csvUsuarios = argv[++i];
        }
        else if (arg == "--txt-ventas" && i + 1 < argc) {
            txtVentas = argv[++i];
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printHelp();
            return 1;
        }
    }
    
    // Run migration
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  CSV to LMDB Migration Utility        ║" << std::endl;
    std::cout << "║  POS System Scalability Feature      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    
    try {
        pos::MigrationUtility migrator(dbPath);
        
        auto stats = migrator.migrateAll(csvProductos, csvUsuarios, txtVentas);
        
        if (stats.success) {
            std::cout << "\n✓ Migration completed successfully!" << std::endl;
            std::cout << "  LMDB database created at: " << dbPath << std::endl;
            return 0;
        } else {
            std::cerr << "\n✗ Migration failed: " << stats.message << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
