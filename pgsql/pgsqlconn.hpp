/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class PgSQLConn.
*/
#pragma once

#include <string>
#include <libpq-fe.h>

// START_NAMESPACE

class PgSQLConn {
public:
    enum Status {
        OK,
        FAILED
    };
    struct Options {
        std::string host   = {"127.0.0.1"};
        int    port        = {5432};
        std::string user   = {"root"};
        std::string passwd;
        std::string database;
    };
    PgSQLConn();
    PgSQLConn(const Options& opt);
    virtual ~PgSQLConn();
    void init(const Options& opt);
    std::string dbConnStr(const Options& opt);

    virtual int connect();
    virtual int close();
    virtual PGconn* handler();

private:
    PGconn*    conn_;
    Options    option_;
};
// END_NAMESPACE
