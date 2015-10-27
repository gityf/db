/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class Sqlite3Query.
*/
#pragma once

#include <vector>
#include <string>

#include "sqlite3.h"

// START_NAMESPACE

class Sqlite3Query {
public:
    enum Status {
        OK,
        FAILED,
        NORESULT
    };
    Sqlite3Query();
    Sqlite3Query(const std::string& dbPath);
    int open(const std::string& dbPath);
    virtual ~Sqlite3Query();

    int start_transaction();
    int rollback();
    int commit();

    int execute(const std::string& query, bool isSelect = false);
    int next(std::vector<char*>& row);

    std::string getDbPath();
    sqlite3* getDb();
protected:
    sqlite3* db_;
    std::string dbPath_;
    char** results_;
    int rows_;
    int columns_;
    int rowOffset_;
};

// END_NAMESPACE
