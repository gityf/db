/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class LevelDB.
*/
#include "LevelDB.hpp"
#include <assert.h>
#include "log/log.h"

// START_NAMESPACE

LevelDB::LevelDB() : leveldb_(NULL), writeOptions(), readOptions()
{
}

LevelDB::~LevelDB() {
    if (leveldb_) {
        delete leveldb_;
        leveldb_ = NULL;
    }
}

int LevelDB::Open(const std::string& db) {
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, db, &leveldb_);

    if (!status.ok()) {
        LOG_FATAL("leveldb connect failed,msg=[%s]", status.ToString().c_str());
        if (leveldb_) {
            delete leveldb_;
            leveldb_ = NULL;
        }
        return FAILED;
    }
    return OK;
}

int LevelDB::Put(const std::string& key, const std::string& value) {
    if (!leveldb_) {
        return FAILED;
    }
    leveldb::Status status = leveldb_->Put(writeOptions, key, value);
    if (!status.ok()) {
        return FAILED;
    }
    return OK;
}
int LevelDB::Get(const std::string& key, std::string& value) {
    if (!leveldb_) {
        return FAILED;
    }
    leveldb::Status status = leveldb_->Get(readOptions, key, &value);
    if (!status.ok()) {
        if (status.IsNotFound()) {
            return OK;
        }
        return FAILED;
    }

    return OK;
}
int LevelDB::Del(const std::string& key) {
    if (!leveldb_) {
        return FAILED;
    }
    leveldb::Status status = leveldb_->Delete(writeOptions, key);
    if (!status.ok()) {
        return FAILED;
    }
    return OK;
}

// END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
