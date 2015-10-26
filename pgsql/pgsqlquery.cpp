/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class PgSQLQuery.
*/
#include <memory>
#include "pgsqlquery.hpp"
#include "log/log.h"

// START_NAMESPACE

PgSQLQuery::PgSQLQuery() {

}

PgSQLQuery::PgSQLQuery(PgSQLConn* conn)
    : conn_(conn), result_(nullptr) {
}

void PgSQLQuery::init(PgSQLConn* conn) {
    conn_ = conn;
    result_ = nullptr;
}

PgSQLQuery::~PgSQLQuery() {
    if (result_) {
        PQclear(result_);
        result_ = nullptr;
    }
}

int PgSQLQuery::start_transaction() {
    if (Status::NOAFFECTED != execute("START TRANSACTION;")) {
        LOG_FATAL("event=[pgsqlquery] type=[transaction] status=[failed]");
        return Status::FAILED;
    }
    return Status::OK;
}
int PgSQLQuery::rollback() {
    if (Status::NOAFFECTED != execute("ROLLBACK;")) {
        LOG_FATAL("event=[pgsqlquery] type=[rollback] status=[failed]");
        return Status::FAILED;
    }
    return Status::OK;
}
int PgSQLQuery::commit() {
    if (Status::NOAFFECTED != execute("COMMIT;")) {
        LOG_FATAL("event=[pgsqlquery] type=[commit] status=[failed]");
        return Status::FAILED;
    }
    return Status::OK;
}

int PgSQLQuery::execute(const std::string& query)  {
    if (!conn_) {
        LOG_FATAL("event=[pgsqlquery] type=[execute] status=[invalid]");
        return Status::NOHANLDER;
    }
    if (result_) {
        PQclear(result_);
        result_ = nullptr;
    }
    bool isFirst = true;
    do {
        PGconn* pgsql = conn_->handler();
        if (!pgsql) {
            LOG_FATAL("event=[pgsqlquery] type=[execute] status=[invalid]");
            return Status::NOHANLDER;
        }
        // exec sql query
        result_ = PQexec(pgsql, query.c_str());
        if (PQresultStatus(result_) == PGRES_FATAL_ERROR) {
            LOG_FATAL("PQexec error:%s", PQerrorMessage(pgsql));
            if (isFirst) {
                isFirst = false;
                if (PgSQLConn::Status::OK != conn_->connect()) {
                    LOG_FATAL("reconnect pgsql failed:%s.", PQerrorMessage(pgsql));
                    return Status::FAILED;
                }
                continue;
            }
            return Status::FAILED;
        }

        // for command:CREATE,UPDATE,DELETE SQL etc.
        if (PQresultStatus(result_) == PGRES_COMMAND_OK) {
            LOG_INFO("%s", PQcmdTuples(result_));
        }

        // for command:SELECT SQL.
        if (PQresultStatus(result_) == PGRES_TUPLES_OK) {
            tuplesOffset_ = 0;
            tuples_ = PQntuples(result_);
        }
        break;
    } while (true);
    
    return Status::OK;
}

int PgSQLQuery::next(std::vector<char*>& row_data) {
    if (!conn_) {
        LOG_WARN("event=[pgsqlquery] type=[execute] status=[invalid]");
        return Status::NOHANLDER;
    }
    row_data.clear();

    PGconn* pgsql = conn_->handler();
    if (!pgsql) {
        LOG_WARN("event=[pgsqlquery] type=[execute] status=[invalid]");
        return Status::NOHANLDER;
    }
    if (!result_) {
        LOG_WARN("event=[pgsqlquery] type=[execute] status=[invalid]");
        return Status::NORESULT;
    }

    int fields_num = PQnfields(result_);
    row_data.reserve(fields_num);

    if (tuplesOffset_ >= tuples_) {
        return Status::NORESULT;
    }

    for (int i = 0; i < fields_num; ++i) {
        row_data.push_back(PQgetvalue(result_, tuplesOffset_, i));
    }
    ++tuplesOffset_;
    
    return Status::OK;
}

// END_NAMESPACE
