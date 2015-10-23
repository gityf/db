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

    int Open(const std::string& db);
    int Put(const std::string& key, const std::string& value);
    int Get(const std::string& key, std::string& value);
    int Del(const std::string& key);

private:
    leveldb::DB* leveldb_;
    leveldb::WriteOptions writeOptions;
    leveldb::ReadOptions readOptions;
};

// END_NAMESPACE
