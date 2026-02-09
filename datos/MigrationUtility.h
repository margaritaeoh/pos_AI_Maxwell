#pragma once

#include <string>
#include <vector>
#include "DatabaseManager.h"
#include "IndexManager.h"
#include "../modelos/Producto.h"
#include "../modelos/Usuario.h"
#include "../modelos/Venta.h"

namespace pos {

/**
 * @brief Utility to migrate data from CSV files to LMDB database
 * Supports one-way migration from existing CSV/TXT files to LMDB backend
 */
class MigrationUtility {
public:
    struct MigrationStats {
        size_t productosCount = 0;
        size_t usuariosCount = 0;
        size_t ventasCount = 0;
        bool success = false;
        std::string message;
    };

    MigrationUtility(const std::string& dbPath = "inventario.db");

    /**
     * @brief Migrate all data (productos, usuarios, ventas) to LMDB
     * @return Migration statistics
     */
    MigrationStats migrateAll(
        const std::string& csvProductos = "data/productos.csv",
        const std::string& csvUsuarios = "data/usuarios.csv",
        const std::string& txtVentas = "data/ventas.txt"
    );

    /**
     * @brief Migrate only productos from CSV to LMDB
     * @return Number of products migrated
     */
    size_t migrateProductos(const std::string& csvPath, IndexManager* index = nullptr);

    /**
     * @brief Migrate only usuarios from CSV to LMDB
     * @return Number of users migrated
     */
    size_t migrateUsuarios(const std::string& csvPath);

    /**
     * @brief Migrate only ventas from TXT to LMDB
     * @return Number of sales migrated
     */
    size_t migrateVentas(const std::string& txtPath);

    /**
     * @brief Verify database integrity
     * @return true if migration appears successful
     */
    bool verify();

    /**
     * @brief Get total items count in LMDB
     */
    size_t getTotalItems() const;

    /**
     * @brief Clear database (destructive operation)
     */
    bool clearDatabase();

private:
    std::unique_ptr<DatabaseManager> db;
    std::string dbPath;

    // Helpers to convert objects to JSON
    std::string productoToJson(const Producto& p) const;
    std::string usuarioToJson(const Usuario& u) const;
    std::string ventaToJson(const Venta& v) const;
};

} // namespace pos
