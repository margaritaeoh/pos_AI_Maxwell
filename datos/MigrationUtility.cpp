#include "MigrationUtility.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace pos {

MigrationUtility::MigrationUtility(const std::string& dbPath)
    : db(std::make_unique<DatabaseManager>(dbPath)),
      dbPath(dbPath) {
}

MigrationUtility::MigrationStats MigrationUtility::migrateAll(
    const std::string& csvProductos,
    const std::string& csvUsuarios,
    const std::string& txtVentas) {
    
    MigrationStats stats;
    
    try {
        std::cout << "\n=== Starting Full Migration to LMDB ===" << std::endl;
        
        // Create index for products
        auto index = std::make_unique<IndexManager>();
        
        // Migrate products
        std::cout << "Migrating products from " << csvProductos << "..." << std::endl;
        stats.productosCount = migrateProductos(csvProductos, index.get());
        std::cout << "✓ Migrated " << stats.productosCount << " products" << std::endl;
        
        // Migrate users
        std::cout << "Migrating users from " << csvUsuarios << "..." << std::endl;
        stats.usuariosCount = migrateUsuarios(csvUsuarios);
        std::cout << "✓ Migrated " << stats.usuariosCount << " users" << std::endl;
        
        // Migrate sales
        std::cout << "Migrating sales from " << txtVentas << "..." << std::endl;
        stats.ventasCount = migrateVentas(txtVentas);
        std::cout << "✓ Migrated " << stats.ventasCount << " sales" << std::endl;
        
        // Verify
        std::cout << "\nVerifying database integrity..." << std::endl;
        if (verify()) {
            std::cout << "✓ Database verification successful" << std::endl;
            stats.success = true;
            stats.message = "Migration completed successfully";
        } else {
            std::cout << "✗ Database verification failed" << std::endl;
            stats.success = false;
            stats.message = "Verification failed";
        }
        
        std::cout << "\n=== Migration Summary ===" << std::endl;
        std::cout << "Products: " << stats.productosCount << std::endl;
        std::cout << "Users:    " << stats.usuariosCount << std::endl;
        std::cout << "Sales:    " << stats.ventasCount << std::endl;
        std::cout << "Total:    " << getTotalItems() << " items" << std::endl;
        std::cout << "Status:   " << (stats.success ? "SUCCESS" : "FAILED") << std::endl;
        
        return stats;
    } catch (const std::exception& e) {
        stats.success = false;
        stats.message = std::string("Migration error: ") + e.what();
        std::cerr << "✗ " << stats.message << std::endl;
        return stats;
    }
}

size_t MigrationUtility::migrateProductos(const std::string& csvPath, IndexManager* index) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << csvPath << std::endl;
        return 0;
    }

    size_t count = 0;
    std::string line;
    bool firstLine = true;

    bool batchOk = db->beginBatch();

    while (std::getline(file, line)) {
        if (firstLine) {
            firstLine = false;  // Skip header
            continue;
        }

        if (line.empty()) continue;

        // Parse CSV line (simple approach - assumes no commas in values)
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(iss, token, ',')) {
            fields.push_back(token);
        }

        if (fields.size() < 10) continue;  // Skip malformed lines

        try {
            Producto p;
            p.id = std::stoll(fields[0]);
            p.codigoBarras = fields[1];
            p.codigoLocal = fields[2];
            p.nombre = fields[3];
            p.descripcion = fields[4];
            p.precioPublico = std::stod(fields[5]);
            p.precioMayorista = std::stod(fields[6]);
            p.tipoUnidad = static_cast<TipoUnidad>(std::stoi(fields[7]));
            p.cantidadInventario = std::stod(fields[8]);
            p.anioCaducidad = std::stoi(fields[9]);
            if (fields.size() > 10) p.mesCaducidad = std::stoi(fields[10]);
            if (fields.size() > 11) p.diaCaducidad = std::stoi(fields[11]);
            if (fields.size() > 12) p.presentacion = static_cast<TipoPresentacion>(std::stoi(fields[12]));
            if (fields.size() > 13) p.piezasPorPresentacion = std::stoi(fields[13]);

            std::string key = "prod_" + std::to_string(p.id);
            std::string value = productoToJson(p);

            if (db->put(key, value)) {
                if (index) {
                    index->addProducto(p);
                }
                count++;
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not parse line: " << line << " (" << e.what() << ")" << std::endl;
        }
    }

    if (batchOk) {
        db->commitBatch();
    }

    file.close();
    return count;
}

size_t MigrationUtility::migrateUsuarios(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << csvPath << std::endl;
        return 0;
    }

    size_t count = 0;
    std::string line;
    bool firstLine = true;

    bool batchOk = db->beginBatch();

    while (std::getline(file, line)) {
        if (firstLine) {
            firstLine = false;  // Skip header
            continue;
        }

        if (line.empty()) continue;

        // Parse CSV line
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(iss, token, ',')) {
            fields.push_back(token);
        }

        if (fields.size() < 3) continue;

        try {
            Usuario u;
            u.username = fields[0];
            u.passwordHash = fields[1];
            u.rol = static_cast<RolUsuario>(std::stoi(fields[2]));

            std::string key = "user_" + u.username;
            std::string value = usuarioToJson(u);

            if (db->put(key, value)) {
                count++;
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not parse user line: " << line << " (" << e.what() << ")" << std::endl;
        }
    }

    if (batchOk) {
        db->commitBatch();
    }

    file.close();
    return count;
}

size_t MigrationUtility::migrateVentas(const std::string& txtPath) {
    std::ifstream file(txtPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << txtPath << std::endl;
        return 0;
    }

    size_t count = 0;
    std::string line;
    
    bool batchOk = db->beginBatch();

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Simple parsing: assume format is "id,usuario,total,fechaHora"
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(iss, token, ',')) {
            fields.push_back(token);
        }

        if (fields.size() < 4) continue;

        try {
            Venta v;
            v.id = std::stoll(fields[0]);
            v.usuario = fields[1];
            v.total = std::stod(fields[2]);
            v.fechaHora = fields[3];

            std::string key = "venta_" + std::to_string(v.id);
            std::string value = ventaToJson(v);

            if (db->put(key, value)) {
                count++;
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not parse sales line: " << line << " (" << e.what() << ")" << std::endl;
        }
    }

    if (batchOk) {
        db->commitBatch();
    }

    file.close();
    return count;
}

bool MigrationUtility::verify() {
    // Check if we can retrieve some data
    std::vector<std::string> keys;
    return db->getAllKeys(keys) && keys.size() > 0;
}

size_t MigrationUtility::getTotalItems() const {
    return db->getSize();
}

bool MigrationUtility::clearDatabase() {
    // LMDB doesn't have a built-in clear all
    // This is a no-op for safety - recommend deleting db file instead
    std::cout << "Warning: Clear database not supported. Delete DB file manually." << std::endl;
    return false;
}

std::string MigrationUtility::productoToJson(const Producto& p) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"id\":" << p.id
       << ",\"nombre\":\"" << p.nombre << "\""
       << ",\"descripcion\":\"" << p.descripcion << "\""
       << ",\"codigoBarras\":\"" << p.codigoBarras << "\""
       << ",\"codigoLocal\":\"" << p.codigoLocal << "\""
       << ",\"precioPublico\":" << p.precioPublico
       << ",\"precioMayorista\":" << p.precioMayorista
       << ",\"tipoUnidad\":" << static_cast<int>(p.tipoUnidad)
       << ",\"cantidadInventario\":" << p.cantidadInventario
       << ",\"anioCaducidad\":" << p.anioCaducidad
       << ",\"mesCaducidad\":" << p.mesCaducidad
       << ",\"diaCaducidad\":" << p.diaCaducidad
       << ",\"presentacion\":" << static_cast<int>(p.presentacion)
       << ",\"piezasPorPresentacion\":" << p.piezasPorPresentacion
       << "}";
    return ss.str();
}

std::string MigrationUtility::usuarioToJson(const Usuario& u) const {
    std::stringstream ss;
    ss << "{"
       << "\"username\":\"" << u.username << "\""
       << ",\"passwordHash\":\"" << u.passwordHash << "\""
       << ",\"rol\":" << static_cast<int>(u.rol)
       << "}";
    return ss.str();
}

std::string MigrationUtility::ventaToJson(const Venta& v) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"id\":" << v.id
       << ",\"usuario\":\"" << v.usuario << "\""
       << ",\"total\":" << v.total
       << ",\"fechaHora\":\"" << v.fechaHora << "\""
       << "}";
    return ss.str();
}

} // namespace pos
