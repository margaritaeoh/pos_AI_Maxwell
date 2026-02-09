#include "DatabaseManager.h"
#include <iostream>
#include <filesystem>

namespace pos {

DatabaseManager::DatabaseManager(const std::string& dbPath, size_t maxSizeGB)
    : dbPath(dbPath) {
    try {
        std::filesystem::create_directories(dbPath);
        
        if (!openEnvironment()) {
            std::cerr << "Warning: Failed to initialize LMDB environment. Database will be unavailable." << std::endl;
            isInitialized = false;
        } else {
            isInitialized = true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: DatabaseManager initialization error: " << e.what() << std::endl;
        isInitialized = false;
    }
}

DatabaseManager::~DatabaseManager() {
    closeEnvironment();
}

bool DatabaseManager::openEnvironment() {
    int rc = mdb_env_create(&env);
    if (rc) return false;

    // Set max size
    rc = mdb_env_set_mapsize(env, 10UL * 1024UL * 1024UL * 1024UL);  // 10GB default
    if (rc) return false;

    // Open environment
    rc = mdb_env_open(env, dbPath.c_str(), 0, 0664);
    if (rc) {
        mdb_env_close(env);
        return false;
    }

    // Begin read transaction
    rc = mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn);
    if (rc) {
        mdb_env_close(env);
        return false;
    }

    // Open database
    rc = mdb_dbi_open(txn, nullptr, 0, &dbi);
    if (rc) {
        mdb_txn_abort(txn);
        mdb_env_close(env);
        return false;
    }

    mdb_txn_commit(txn);
    txn = nullptr;

    return true;
}

void DatabaseManager::closeEnvironment() {
    if (batchTxn) mdb_txn_abort(batchTxn);
    if (txn) mdb_txn_abort(txn);
    if (dbi) mdb_dbi_close(env, dbi);
    if (env) mdb_env_close(env);
}

bool DatabaseManager::put(const std::string& key, const std::string& value) {
    if (!isInitialized) return false;

    MDB_txn* tx = batchTxn;  // Use batch txn if active
    if (!tx) {
        if (mdb_txn_begin(env, nullptr, 0, &tx)) return false;
    }

    MDB_val k, v;
    k.mv_data = const_cast<char*>(key.data());
    k.mv_size = key.size();
    v.mv_data = const_cast<char*>(value.data());
    v.mv_size = value.size();

    int rc = mdb_put(tx, dbi, &k, &v, 0);
    
    if (!batchTxn && tx) {
        if (rc == 0) mdb_txn_commit(tx);
        else mdb_txn_abort(tx);
    }

    return rc == 0;
}

bool DatabaseManager::get(const std::string& key, std::string& outValue) const {
    if (!isInitialized) return false;

    MDB_txn* readTxn = nullptr;
    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &readTxn)) return false;

    MDB_val k, v;
    k.mv_data = const_cast<char*>(key.data());
    k.mv_size = key.size();

    int rc = mdb_get(readTxn, dbi, &k, &v);
    
    if (rc == 0) {
        outValue = std::string(static_cast<char*>(v.mv_data), v.mv_size);
    }

    mdb_txn_abort(readTxn);
    return rc == 0;
}

bool DatabaseManager::del(const std::string& key) {
    if (!isInitialized) return false;

    MDB_txn* tx = batchTxn;
    if (!tx) {
        if (mdb_txn_begin(env, nullptr, 0, &tx)) return false;
    }

    MDB_val k;
    k.mv_data = const_cast<char*>(key.data());
    k.mv_size = key.size();
    int rc = mdb_del(tx, dbi, &k, nullptr);

    if (!batchTxn && tx) {
        if (rc == 0) mdb_txn_commit(tx);
        else mdb_txn_abort(tx);
    }

    return rc == 0;
}

bool DatabaseManager::exists(const std::string& key) const {
    std::string dummy;
    return get(key, dummy);
}

bool DatabaseManager::beginBatch() {
    if (batchTxn) return false;  // Already in batch
    return mdb_txn_begin(env, nullptr, 0, &batchTxn) == 0;
}

bool DatabaseManager::commitBatch() {
    if (!batchTxn) return false;
    bool result = (mdb_txn_commit(batchTxn) == 0);
    batchTxn = nullptr;
    return result;
}

void DatabaseManager::abortBatch() {
    if (batchTxn) {
        mdb_txn_abort(batchTxn);
        batchTxn = nullptr;
    }
}

bool DatabaseManager::getAllKeys(std::vector<std::string>& outKeys) const {
    if (!isInitialized) return false;

    MDB_txn* readTxn = nullptr;
    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &readTxn)) return false;

    MDB_cursor* cursor = nullptr;
    if (mdb_cursor_open(readTxn, dbi, &cursor)) {
        mdb_txn_abort(readTxn);
        return false;
    }

    MDB_val key, value;
    while (mdb_cursor_get(cursor, &key, &value, MDB_NEXT) == 0) {
        outKeys.emplace_back(static_cast<char*>(key.mv_data), key.mv_size);
    }

    mdb_cursor_close(cursor);
    mdb_txn_abort(readTxn);
    return true;
}

bool DatabaseManager::getAllValues(std::vector<std::string>& outValues) const {
    if (!isInitialized) return false;

    MDB_txn* readTxn = nullptr;
    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &readTxn)) return false;

    MDB_cursor* cursor = nullptr;
    if (mdb_cursor_open(readTxn, dbi, &cursor)) {
        mdb_txn_abort(readTxn);
        return false;
    }

    MDB_val key, value;
    while (mdb_cursor_get(cursor, &key, &value, MDB_NEXT) == 0) {
        outValues.emplace_back(static_cast<char*>(value.mv_data), value.mv_size);
    }

    mdb_cursor_close(cursor);
    mdb_txn_abort(readTxn);
    return true;
}

size_t DatabaseManager::getSize() const {
    if (!isInitialized) return 0;

    MDB_txn* readTxn = nullptr;
    if (mdb_txn_begin(env, nullptr, MDB_RDONLY, &readTxn)) return 0;

    MDB_stat stat;
    mdb_stat(readTxn, dbi, &stat);
    size_t size = stat.ms_entries;

    mdb_txn_abort(readTxn);
    return size;
}

bool DatabaseManager::sync() {
    if (!isInitialized) return false;
    return mdb_env_sync(env, 1) == 0;
}

bool DatabaseManager::vacuum() {
    // LMDB doesn't have explicit vacuum, but we can compact by copying
    // For now, just sync
    return sync();
}

} // namespace pos
