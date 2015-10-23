/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class MySQLQuery.
*/
#pragma once

#include <vector>
#include <string>

#include "MySQLConn.hpp"


// START_NAMESPACE

class MySQLQuery {
public:
    enum Status {
        OK,
        NOHANLDER,
        NORESULT,
        DUPPRIM,
        FAILED,
        NOAFFECTED
    };
    MySQLQuery();
    MySQLQuery(MySQLConn* conn);
    void init(MySQLConn* conn);
    virtual ~MySQLQuery();

    virtual int start_transaction();
    virtual int rollback();
    virtual int commit();

    virtual int execute(const std::string& query);
    virtual int next(std::vector<char*>& row);

    std::string escape(const std::string& raw);

protected:
    MySQLConn* conn_;
    MYSQL_RES* result_;
};

// END_NAMESPACE
