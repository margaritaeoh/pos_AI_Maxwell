#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../modelos/Producto.h"

namespace pos {

/**
 * @brief In-memory index for fast lookups without full DB scans
 * Maintains multiple indexes for different search criteria
 */
class IndexManager {
public:
    IndexManager() = default;

    // Add/Update/Remove with full Producto object
    void addProducto(const Producto& p);
    void updateProducto(const Producto& p);
    void removeProducto(int64_t id);
    void clear();

    // Lookups (O(1) average) - returns pointer to cached Producto
    const Producto* findByCodigoBarras(const std::string& codigo) const;
    const Producto* findByCodigoLocal(const std::string& codigo) const;
    const Producto* findByProductId(int64_t id) const;

    // Search with prefix matching (returns vector of pointers)
    std::vector<const Producto*> searchByNombre(const std::string& prefix, size_t maxResults = 100) const;
    std::vector<const Producto*> searchByCodigo(const std::string& prefix, size_t maxResults = 100) const;

    // Get all indexed product IDs
    std::vector<int64_t> getAllProductIds() const;

    size_t getIndexSize() const { return idIndex.size(); }

private:
    // Primary index: productId -> Producto pointer
    std::unordered_map<int64_t, const Producto*> idIndex;
    
    // Secondary indexes: key -> productId
    std::unordered_map<std::string, int64_t> codigoBarrasIndex;
    std::unordered_map<std::string, int64_t> codigoLocalIndex;
    std::unordered_map<std::string, int64_t> nombreIndex;  // First 50 chars of nombre

    // Prefix search helpers
    static bool startsWithCaseInsensitive(const std::string& str, const std::string& prefix);
};

} // namespace pos
