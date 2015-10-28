/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class Sqlite3Query.
*/
#include "sqlite3query.hpp"
#include "log/log.h"

// START_NAMESPACE

Sqlite3Query::Sqlite3Query() {
    db_ = NULL;
    results_ = NULL;
    rowOffset_ = rows_ = columns_ = 0;
}

Sqlite3Query::Sqlite3Query(const std::string& dbPath) {
    open(dbPath);
}

int Sqlite3Query::open(const std::string& dbPath) {
    dbPath_ = dbPath;
    int ret = sqlite3_open(dbPath.c_str(), &db_);
    if (ret != SQLITE_OK){
        LOG_FATAL("Cannot open db:%s, errmsg:%s\n",
            dbPath_.c_str(), sqlite3_errmsg(db_));
        return FAILED;
    }
    return OK;
}

Sqlite3Query::~Sqlite3Query() {
    if (db_ != NULL) {
        sqlite3_close(db_);
    }    
}

int Sqlite3Query::start_transaction() {
    if (OK != execute("BEGIN TRANSACTION;")) {
        LOG_FATAL("BEGIN TRANSACTION failed.");
        return FAILED;
    }
    return OK;
}
int Sqlite3Query::rollback() {
    if (OK != execute("ROLLBACK TRANSACTION;")) {
        LOG_FATAL("ROLLBACK TRANSACTION failed.");
        return FAILED;
    }
    return OK;
}
int Sqlite3Query::commit() {
    if (OK != execute("COMMIT TRANSACTION;")) {
        LOG_FATAL("COMMIT TRANSACTION failed.");
        return FAILED;
    }
    return OK;
}

int Sqlite3Query::execute(const std::string& query, bool isSelect) {
    char* errMsg = NULL;
    int ret = SQLITE_OK;
    if (isSelect) {
        if (results_ != NULL) {
            sqlite3_free_table(results_);
        }
        ret = sqlite3_get_table(db_, query.c_str(),
            &results_, &rows_, &columns_, &errMsg);
        rowOffset_ = 0;
    }
    else {
        ret = sqlite3_exec(db_, query.c_str(), NULL, NULL, &errMsg);
    }
    
    if (ret != SQLITE_OK){
        LOG_FATAL("exec sql:[%s] fail: %s\n", query.c_str(), errMsg);
        sqlite3_free(errMsg);
        return FAILED;
    }
    sqlite3_free(errMsg);
    return OK;
}

int Sqlite3Query::select(const std::string& query) {
    return execute(query, true);
}

int Sqlite3Query::next(std::vector<char*>& row_data) {
    if (rowOffset_ >= rows_) {
        return NORESULT;
    }
    row_data.clear();
    row_data.reserve(columns_);
    rowOffset_++;
    int offset = rowOffset_* columns_;
    for (int i = 0; i < columns_; ++i) {
        row_data.push_back(results_[i+offset]);
        LOG_DEBUG("column_name:%s,column_value:%s",
            results_[i], results_[i + offset]);
    }
    return OK;
}

std::string Sqlite3Query::getDbPath() {
    return dbPath_;
}

sqlite3* Sqlite3Query::getDb() {
    return db_;
}

//
// //prepare statement
// sqlite3_stmt *stmt;
// sqlite3_prepare_v2(db, "insert into t(id,msg) values(?,?)", -1, &stmt, 0);
// for (i = 10; i < 20; i++) {
//     sprintf(ca, "HELLO#%i", i);
//     sqlite3_bind_int(stmt, 1, i);
//     sqlite3_bind_text(stmt, 2, ca, strlen(ca), SQLITE_STATIC);
//     sqlite3_step(stmt);
//     sqlite3_reset(stmt);
// }
// sqlite3_finalize(stmt);

// END_NAMESPACE
