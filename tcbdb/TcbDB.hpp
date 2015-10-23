/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class TcbDB.
*/
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <tcbdb.h>

// START_NAMESPACE

struct Options {
    int32_t membersOfLeaf = { 1024 };
    int32_t membersOfNonLeaf = { 2048 };
    int32_t elementsOfArray = { 50000000 };
    int8_t  apow = { 8 };
    int8_t  fpow = { 10 };
    int8_t  opts = { BDBTLARGE };
    // maximum number of leaf nodes to be cached.
    int32_t maxNumOfLeaf = { 1048576 };
    // the size of the extra mapped memory
    int32_t extraMemorySize = { 1024 };
};

class TcbDB {
public:
    enum Status {
        OK,
        FAILED
    };

    explicit TcbDB();
    virtual ~TcbDB();

    void setOption(const Options& opt);
    int open(const std::string& db);
    // Store record into a B+ tree database object.
    int put(const std::string& key, const std::string& value);
    // Store records into a B+ tree database object with allowing duplication of keys.
    int multiPut(const std::string& key, const std::vector<std::string>& values);
    // Store int record into a B+ tree database object.
    int putInt(const std::string& key, int value);
    // Store double record into a B+ tree database object.
    int putDouble(const std::string& key, double value);
    // Store a record into a B+ tree database object with allowing duplication of keys.
    int putDup(const std::string& key, const std::string& value);
    // Retrieve a record in a B+ tree database object.
    int get(const std::string& key, std::string& value);
    // Retrieve records in a B+ tree database object.
    int multiGet(const std::string& beginKey,
        const std::string& endKey, int limits,
        std::vector<std::string>& values);
    // Get forward matching keys in a B+ tree database object.
    int getPrefix(const std::string& key, int limits,
        std::vector<std::string>& values);
    // Remove records of a B+ tree database object.
    int del(const std::string& key);
    // Concatenate a value at the end of the existing record in a B+ tree database object.
    int append(const std::string& key, const std::string& value);

    // Get the number of records of a B+ tree database object.
    uint64_t keyCounts();
    // Get the size of the database file of a B+ tree database object.
    uint64_t dbSize();
    // Get the number of records corresponding a string key in a B + tree database object.
    int numOfKey(const std::string& key);
    // Get the number of records corresponding a key in a B+ tree database object.
    int valueSizeOfKey(const std::string& key);
    // Retrieve records in a B+ tree database object.
    int valuesOfKey(const std::string& key, std::vector<std::string>& values);
    // Synchronize updated contents of a B+ tree database object with the file and the device.
    int save();
    // Remove all records of a B+ tree database object.
    int destory();
    // Optimize the file of a B+ tree database object.
    int optimize(const Options& opt);
    // Copy the database file of a B+ tree database object.
    int copy(const std::string& toDbPath);
    // Begin the transaction of a B+ tree database object.
    int startTransaction();
    // Abort the transaction of a B+ tree database object.
    int rollback();
    // Commit the transaction of a B+ tree database object.
    int commit();
    // Get the file path of a B+ tree database object.
    std::string dbPath();
    TCBDB* getTcbDbRawPtr() {
        return tcbdb_;
    }
private:
    void TCLIST2vector(TCLIST* rets, std::vector<std::string>* values);
private:
    // tcbdb pointer.
    TCBDB *tcbdb_;
    Options options_;
};

// END_NAMESPACE
