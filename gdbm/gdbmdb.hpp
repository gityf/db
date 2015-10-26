/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class GdbmDB.
**Help of gdbm:http://www.gnu.org.ua/software/gdbm/manual/gdbm.html
*/
#pragma once

#include <map>
#include <string>
#include <gdbm.h>

// START_NAMESPACE
class GdbmDB {
public:
    enum Status {
        OK,
        FAILED
    };
    struct Options {
        Options() {
            blockSize = 512;
            readWrite = GDBM_WRCREAT;
            mode = 00664;
        }
        int blockSize;
        int readWrite;
        int mode;
    };

    explicit GdbmDB();
    virtual ~GdbmDB();
    void setOption(const Options& opt);
    // readWrite
    // GDBM_READER   0  A reader.
    // GDBM_WRITER   1  A writer.
    // GDBM_WRCREAT  2  A writer.  Create the db if needed.
    // GDBM_NEWDB    3  A writer.  Always create a new db.
    // GDBM_OPENMASK 7  Mask for the above.
    int open(const std::string& db);
    int put(const std::string& key, const std::string& value);
    int get(const std::string& key, std::string& value);
    int del(const std::string& key);
    bool exist(const std::string& key);
    int reorganize();
    // unsigned long long int gdbm_count_t
    gdbm_count_t dbCount();
    // load from file to gdbmdb.
    int load(const std::string& fromFile);
    // dump from gdbmdb to file.
    // fmt: 0-binary, 1-ASCII
    int dump(const std::string& toFile, int fmt);
    int dump(std::map<std::string, std::string>& toKeyVals);
    const char* errMsg();
private:
    GDBM_FILE gdbmdb_;
    Options option_;
};

// END_NAMESPACE
