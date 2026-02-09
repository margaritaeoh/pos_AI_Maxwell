#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <lmdb.h>

namespace pos {

/**
 * @brief Manages LMDB environment and transactions
 * Single-threaded, read-optimized wrapper around LMDB
 */
class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath, size_t maxSizeGB = 10);
    ~DatabaseManager();

    // Delete copy/move for simplicity
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Key-value operations
    bool put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& outValue) const;
    bool del(const std::string& key);
    bool exists(const std::string& key) const;

    // Batch operations
    bool beginBatch();
    bool commitBatch();
    void abortBatch();

    // Iteration
    bool getAllKeys(std::vector<std::string>& outKeys) const;
    bool getAllValues(std::vector<std::string>& outValues) const;
    size_t getSize() const;

    // Maintenance
    bool sync();
    bool vacuum();

private:
    MDB_env* env = nullptr;
    MDB_dbi dbi = 0;
    MDB_txn* txn = nullptr;  // current transaction
    MDB_txn* batchTxn = nullptr;  // batch transaction
    std::string dbPath;
    bool isInitialized = false;

    bool openEnvironment();
    void closeEnvironment();
};

} // namespace pos
