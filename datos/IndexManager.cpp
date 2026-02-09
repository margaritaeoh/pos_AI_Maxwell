#include "IndexManager.h"
#include <algorithm>
#include <cctype>

namespace pos {

void IndexManager::addProducto(const Producto& p) {
    idIndex[p.id] = &p;
    codigoBarrasIndex[p.codigoBarras] = p.id;
    codigoLocalIndex[p.codigoLocal] = p.id;
    // Index first 50 chars of nombre
    std::string nombreKey = p.nombre.substr(0, 50);
    nombreIndex[nombreKey] = p.id;
}

void IndexManager::updateProducto(const Producto& p) {
    // Remove old entries and add new ones
    removeProducto(p.id);
    addProducto(p);
}

void IndexManager::removeProducto(int64_t id) {
    auto it = idIndex.find(id);
    if (it == idIndex.end()) return;

    // Remove from all secondary indexes
    for (auto& [codigo, pid] : codigoBarrasIndex) {
        if (pid == id) {
            codigoBarrasIndex.erase(codigo);
            break;
        }
    }
    for (auto& [codigo, pid] : codigoLocalIndex) {
        if (pid == id) {
            codigoLocalIndex.erase(codigo);
            break;
        }
    }
    for (auto& [nombre, pid] : nombreIndex) {
        if (pid == id) {
            nombreIndex.erase(nombre);
            break;
        }
    }
    idIndex.erase(it);
}

void IndexManager::clear() {
    idIndex.clear();
    codigoBarrasIndex.clear();
    codigoLocalIndex.clear();
    nombreIndex.clear();
}

const Producto* IndexManager::findByCodigoBarras(const std::string& codigo) const {
    auto it = codigoBarrasIndex.find(codigo);
    if (it != codigoBarrasIndex.end()) {
        auto idIt = idIndex.find(it->second);
        if (idIt != idIndex.end()) {
            return idIt->second;
        }
    }
    return nullptr;
}

const Producto* IndexManager::findByCodigoLocal(const std::string& codigo) const {
    auto it = codigoLocalIndex.find(codigo);
    if (it != codigoLocalIndex.end()) {
        auto idIt = idIndex.find(it->second);
        if (idIt != idIndex.end()) {
            return idIt->second;
        }
    }
    return nullptr;
}

const Producto* IndexManager::findByProductId(int64_t id) const {
    auto it = idIndex.find(id);
    return (it != idIndex.end()) ? it->second : nullptr;
}

bool IndexManager::startsWithCaseInsensitive(const std::string& str, const std::string& prefix) {
    if (str.size() < prefix.size()) return false;
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(str[i]) != std::tolower(prefix[i])) return false;
    }
    return true;
}

std::vector<const Producto*> IndexManager::searchByNombre(const std::string& prefix, size_t maxResults) const {
    std::vector<const Producto*> results;
    for (const auto& [nombre, productId] : nombreIndex) {
        if (startsWithCaseInsensitive(nombre, prefix)) {
            auto it = idIndex.find(productId);
            if (it != idIndex.end()) {
                results.push_back(it->second);
                if (results.size() >= maxResults) break;
            }
        }
    }
    return results;
}

std::vector<const Producto*> IndexManager::searchByCodigo(const std::string& prefix, size_t maxResults) const {
    std::vector<const Producto*> results;
    
    for (const auto& [codigo, productId] : codigoBarrasIndex) {
        if (startsWithCaseInsensitive(codigo, prefix)) {
            auto it = idIndex.find(productId);
            if (it != idIndex.end()) {
                results.push_back(it->second);
                if (results.size() >= maxResults) break;
            }
        }
    }
    
    // Also search in codigoLocal
    for (const auto& [codigo, productId] : codigoLocalIndex) {
        if (startsWithCaseInsensitive(codigo, prefix)) {
            // Avoid duplicates
            auto existing = std::find_if(results.begin(), results.end(),
                [productId](const Producto* p) { return p->id == productId; });
            if (existing == results.end()) {
                auto it = idIndex.find(productId);
                if (it != idIndex.end()) {
                    results.push_back(it->second);
                    if (results.size() >= maxResults) break;
                }
            }
        }
    }
    return results;
}

std::vector<int64_t> IndexManager::getAllProductIds() const {
    std::vector<int64_t> ids;
    ids.reserve(idIndex.size());
    for (const auto& [id, _] : idIndex) {
        ids.push_back(id);
    }
    return ids;
}

} // namespace pos
