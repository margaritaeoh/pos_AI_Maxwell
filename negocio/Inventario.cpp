#include "Inventario.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace pos {

Inventario::Inventario(ArchivoProductos& archivo)
    : archivo(archivo),
      db(std::make_unique<DatabaseManager>("inventario.db")),
      index(std::make_unique<IndexManager>()) {
    // useLMDB defaults to false - enabled only after migration via setUseLMDB()
}

Inventario::~Inventario() {
    // Save any pending changes
    if (useLMDB) {
        guardar();
    }
}

bool Inventario::cargar() {
    try {
        // First, load from CSV file
        if (!archivo.cargar(productos)) {
            std::cerr << "Falló al cargar CSV" << std::endl;
            return true;  // Still OK to continue without products
        }

        // Always populate the index from CSV products (for search functionality)
        if (index) {
            for (const auto& p : productos) {
                index->addProducto(p);
            }
            std::cout << "Índice de búsqueda poblado con " << productos.size() << " productos" << std::endl;
        }

        if (!useLMDB) {
            return true;
        }

        // If LMDB is enabled, also save to database
        if (db && index) {
            size_t dbCount = 0;
            for (const auto& p : productos) {
                std::string key = std::to_string(p.id);
                std::string value = productoToJson(p);
                
                if (db->put(key, value)) {
                    dbCount++;
                }
            }

            if (dbCount > 0) {
                std::cout << "Cargados " << dbCount << " productos en LMDB" << std::endl;
            }
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error cargando inventario: " << e.what() << std::endl;
        return true;  // Don't crash - continue with CSV data
    }
}

bool Inventario::guardar() {
    if (!useLMDB) {
        return archivo.guardar(productos);
    }

    // Save current products to CSV
    return archivo.guardar(productos);
}

Producto* Inventario::buscarPorId(int64_t id) {
    // Try index first (O(1))
    const auto& result = index->findByProductId(id);
    if (result) {
        // Find in current productos vector
        for (auto& p : productos) {
            if (p.id == id) return &p;
        }
        // If not in cache, fetch from DB
        return fetchProductoFromDB(id);
    }
    return nullptr;
}

Producto* Inventario::buscarPorCodigoBarras(const std::string& codigo) {
    // Use index (O(1) lookup)
    const auto& result = index->findByCodigoBarras(codigo);
    if (result) {
        for (auto& p : productos) {
            if (p.codigoBarras == codigo) return &p;
        }
        return fetchProductoFromDB(result->id);
    }
    return nullptr;
}

Producto* Inventario::buscarPorCodigoLocal(const std::string& codigo) {
    // Use index (O(1) lookup)
    const auto& result = index->findByCodigoLocal(codigo);
    if (result) {
        for (auto& p : productos) {
            if (p.codigoLocal == codigo) return &p;
        }
        return fetchProductoFromDB(result->id);
    }
    return nullptr;
}

const std::vector<Producto>& Inventario::obtenerTodos() const {
    return productos;
}

std::vector<Producto> Inventario::obtenerPagina(size_t pageNum, size_t pageSize) {
    std::vector<Producto> result;
    
    if (productos.empty()) return result;

    size_t startIdx = (pageNum - 1) * pageSize;
    size_t endIdx = std::min(startIdx + pageSize, productos.size());

    if (startIdx >= productos.size()) {
        return result;  // Page out of bounds
    }

    result.insert(result.end(), 
                  productos.begin() + startIdx, 
                  productos.begin() + endIdx);
    return result;
}

std::vector<Producto> Inventario::buscarPorNombrePaginado(const std::string& nombre, 
                                                           size_t pageNum, size_t pageSize) {
    std::vector<Producto> result;
    
    // Use index for prefix search
    auto matches = index->searchByNombre(nombre, 1000);  // Get top 1000 matches
    
    size_t startIdx = (pageNum - 1) * pageSize;
    size_t endIdx = std::min(startIdx + pageSize, matches.size());

    if (startIdx >= matches.size()) {
        return result;  // Page out of bounds
    }

    for (size_t i = startIdx; i < endIdx; ++i) {
        for (auto& p : productos) {
            if (p.id == matches[i]->id) {
                result.push_back(p);
                break;
            }
        }
    }

    return result;
}

std::vector<Producto> Inventario::buscarPorCodigoPaginado(const std::string& codigo,
                                                           size_t pageNum, size_t pageSize) {
    std::vector<Producto> result;
    
    // First try exact match
    auto exact = index->findByCodigoBarras(codigo);
    if (exact) {
        for (auto& p : productos) {
            if (p.codigoBarras == codigo || p.codigoLocal == codigo) {
                result.push_back(p);
            }
        }
    }
    // Then try prefix search on local codes
    else {
        for (auto& p : productos) {
            if (p.codigoLocal.find(codigo) == 0 || 
                p.codigoBarras.find(codigo) == 0) {
                result.push_back(p);
            }
        }
    }

    // Apply pagination
    std::vector<Producto> paginated;
    size_t startIdx = (pageNum - 1) * pageSize;
    size_t endIdx = std::min(startIdx + pageSize, result.size());

    if (startIdx < result.size()) {
        paginated.insert(paginated.end(), 
                        result.begin() + startIdx, 
                        result.begin() + endIdx);
    }

    return paginated;
}

size_t Inventario::getTotalProductos() const {
    return productos.size();
}

size_t Inventario::getTotalPages(size_t pageSize) const {
    if (productos.empty()) return 1;
    return (productos.size() + pageSize - 1) / pageSize;
}

bool Inventario::agregarProducto(const Producto& producto) {
    // No hard limit anymore
    productos.push_back(producto);
    
    if (useLMDB) {
        std::string key = std::to_string(producto.id);
        std::string value = productoToJson(producto);
        
        db->put(key, value);
        index->addProducto(producto);
    }
    
    return true;
}

bool Inventario::actualizarProducto(const Producto& producto) {
    for (auto& p : productos) {
        if (p.id == producto.id) {
            p = producto;
            
            if (useLMDB) {
                std::string key = std::to_string(producto.id);
                std::string value = productoToJson(producto);
                db->put(key, value);
                index->updateProducto(producto);
            }
            
            return true;
        }
    }
    return false;
}

bool Inventario::eliminarProducto(int64_t id) {
    auto it = std::find_if(productos.begin(), productos.end(),
                          [id](const Producto& p) { return p.id == id; });
    
    if (it != productos.end()) {
        if (useLMDB) {
            std::string key = std::to_string(id);
            db->del(key);
            index->removeProducto(id);
        }
        productos.erase(it);
        return true;
    }
    return false;
}

std::string Inventario::productoToJson(const Producto& p) const {
    std::stringstream ss;
    ss << "{\"id\":" << p.id
       << ",\"nombre\":\"" << p.nombre << "\""
       << ",\"descripcion\":\"" << p.descripcion << "\""
       << ",\"codigoBarras\":\"" << p.codigoBarras << "\""
       << ",\"codigoLocal\":\"" << p.codigoLocal << "\""
       << ",\"precioPublico\":" << p.precioPublico
       << ",\"precioMayorista\":" << p.precioMayorista
       << ",\"cantidadInventario\":" << p.cantidadInventario
       << "}";
    return ss.str();
}

Producto Inventario::productoFromJson(const std::string& json) const {
    Producto p;
    // Simple JSON parsing - in production use a real JSON library
    // For now, return empty producto
    return p;
}

Producto* Inventario::fetchProductoFromDB(int64_t id) {
    if (!useLMDB) return nullptr;
    
    std::string key = std::to_string(id);
    std::string value;
    
    if (db->get(key, value)) {
        // In production, parse JSON back to Producto
        // For now, search in memoria cache
        for (auto& p : productos) {
            if (p.id == id) return &p;
        }
    }
    
    return nullptr;
}

} // namespace pos
