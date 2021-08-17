/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class LevelDB.
*/
#pragma once

#include <string>
#include <leveldb/db.h>

// START_NAMESPACE
class LevelDB {
public:
    enum Status {
        OK,
        FAILED
    };
    explicit LevelDB();
    virtual ~LevelDB();

    // open db file.
    int Open(const std::string& db);
    // put key/value pair into db.
    int Put(const std::string& key, const std::string& value);
    // get value of key, param value is out value.
    int Get(const std::string& key, std::string& value);
    // to delete key from db.
    int Del(const std::string& key);

private:
    leveldb::DB* leveldb_;
    leveldb::WriteOptions writeOptions;
    leveldb::ReadOptions readOptions;
};

// END_NAMESPACE
