#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <vector>
#include <memory>
#include "../modelos/Producto.h"
#include "../datos/ArchivoProductos.h"
#include "../datos/DatabaseManager.h"
#include "../datos/IndexManager.h"
#include "../datos/PageInfo.h"

namespace pos {

class Inventario {
public:
    explicit Inventario(ArchivoProductos& archivo);
    ~Inventario();

    // Core operations
    bool cargar();
    bool guardar();

    // Lookups (O(1) using indexes)
    Producto* buscarPorId(int64_t id);
    Producto* buscarPorCodigoBarras(const std::string& codigo);
    Producto* buscarPorCodigoLocal(const std::string& codigo);

    // Legacy compatibility (non-paginated, for small result sets)
    const std::vector<Producto>& obtenerTodos() const;

    // Pagination methods (for large datasets)
    std::vector<Producto> obtenerPagina(size_t pageNum, size_t pageSize = 50);
    std::vector<Producto> buscarPorNombrePaginado(const std::string& nombre, 
                                                   size_t pageNum, size_t pageSize = 50);
    std::vector<Producto> buscarPorCodigoPaginado(const std::string& codigo,
                                                   size_t pageNum, size_t pageSize = 50);
    
    size_t getTotalProductos() const;
    size_t getTotalPages(size_t pageSize = 50) const;

    // Modifications
    bool agregarProducto(const Producto& producto);
    bool actualizarProducto(const Producto& producto);
    bool eliminarProducto(int64_t id);

    // Public access to index for advanced searches
    IndexManager* getIndex() { return index.get(); }
    
    // Enable/disable LMDB backend (disabled by default, enable after migration)
    void setUseLMDB(bool enabled) { useLMDB = enabled; }
    bool isUsingLMDB() const { return useLMDB; }

private:
    ArchivoProductos& archivo;
    std::vector<Producto> productos;  // Cache for current page
    
    // New scalability components
    std::unique_ptr<DatabaseManager> db;
    std::unique_ptr<IndexManager> index;
    bool useLMDB = false;  // Disabled by default - enable after migration

    // Helper: convert Producto to JSON string for storage
    std::string productoToJson(const Producto& p) const;
    Producto productoFromJson(const std::string& json) const;
    
    // Helper: get product by ID from DB
    Producto* fetchProductoFromDB(int64_t id);
};

} // namespace pos

#endif // INVENTARIO_H