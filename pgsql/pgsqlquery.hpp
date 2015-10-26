/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class PgSQLQuery.
*/
#pragma once

#include <vector>
#include <string>

#include "pgsqlconn.hpp"


// START_NAMESPACE

class PgSQLQuery {
public:
    enum Status {
        OK,
        NOHANLDER,
        NORESULT,
        DUPPRIM,
        FAILED,
        NOAFFECTED
    };
    PgSQLQuery();
    PgSQLQuery(PgSQLConn* conn);
    void init(PgSQLConn* conn);
    virtual ~PgSQLQuery();

    virtual int start_transaction();
    virtual int rollback();
    virtual int commit();

    virtual int execute(const std::string& query);
    virtual int next(std::vector<char*>& row);
    int resultLines() {
        return tuples_;
    }
    PGresult* getResultRaw() {
        return result_;
    }
protected:
    PgSQLConn* conn_;
    PGresult* result_;
    int tuplesOffset_;
    int tuples_;
};

// END_NAMESPACE
