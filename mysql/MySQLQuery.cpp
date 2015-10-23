/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class MySQLQuery.
*/
#include <memory>
#include <mysqld_error.h>
#include "MySQLQuery.hpp"
#include "log/log.h"

// START_NAMESPACE

MySQLQuery::MySQLQuery() {

}

MySQLQuery::MySQLQuery(MySQLConn* conn)
    : conn_(conn), result_(nullptr) {
}

void MySQLQuery::init(MySQLConn* conn) {
    conn_ = conn;
    result_ = nullptr;
}

MySQLQuery::~MySQLQuery() {
    if (result_) {
        mysql_free_result(result_);
        result_ = nullptr;
    }
}

int MySQLQuery::start_transaction() {
    if (Status::NOAFFECTED != execute("START TRANSACTION;")) {
        LOG_FATAL("event=[mysqlquery] type=[transaction] status=[failed]");
        return Status::FAILED;
    }
    return Status::OK;
}
int MySQLQuery::rollback() {
    if (Status::NOAFFECTED != execute("ROLLBACK;")) {
        LOG_FATAL("event=[mysqlquery] type=[rollback] status=[failed]");
        return Status::FAILED;
    }
    return Status::OK;
}
int MySQLQuery::commit() {
    if (Status::NOAFFECTED != execute("COMMIT;")) {
        LOG_FATAL("event=[mysqlquery] type=[commit] status=[failed]");
        return Status::FAILED;
    }
    return Status::OK;
}

int MySQLQuery::execute(const std::string& query)  {
    if (!conn_) {
        LOG_FATAL("event=[mysqlquery] type=[execute] status=[invalid]");
        return Status::NOHANLDER;
    }
    MYSQL* mysql = conn_->handler();
    if (!mysql) {
        LOG_FATAL("event=[mysqlquery] type=[execute] status=[invalid]");
        return Status::NOHANLDER;
    }
    if (result_) {
        mysql_free_result(result_);
        result_ = nullptr;
    }

    // first try
    if (mysql_real_query(mysql, query.c_str(), query.size())) {
        if (ER_DUP_ENTRY == mysql_errno(mysql)) {
            LOG_WARN("event=[mysqlquery] type=[execute] status=[failed] message=[%s]",
                mysql_error(mysql));
            return Status::DUPPRIM;
        } else {
            LOG_WARN("event=[mysqlquery] type=[execute] status=[failed] message=[%s]",
                mysql_error(mysql));
            // reconnect
            if (MySQLConn::Status::OK != conn_->connect()) {
                LOG_WARN("event=[mysqlquery] type=[execute] status=[failed] message=["
                    "reconnect failed]");
                return Status::FAILED;
            } else {
                // second try
                if (mysql_real_query(mysql, query.c_str(), query.size())) {
                    if (ER_DUP_ENTRY == mysql_errno(mysql)) {
                        LOG_WARN("event=[mysqlquery] type=[execute] status=[failed] message=[%s]",
                            mysql_error(mysql));
                        return Status::DUPPRIM;
                    } else {
                        LOG_WARN("event=[mysqlquery] type=[execute] status=[failed] message=[%s]",
                            mysql_error(mysql));
                        return Status::FAILED;
                    }
                }
            }
        }
    }


    result_ = mysql_store_result(mysql);
    if (!result_) {
        if (!mysql_field_count(mysql)) {
            // query does not return data
            // (it was not a SELECT)
            my_ulonglong affected_rows_ = (my_ulonglong) mysql_affected_rows(mysql);
            if (affected_rows_ == 0) {
                LOG_DEBUG("event=[mysqlquery] type=[execute] status=[finshed] message=["
                    "there is no affected rows: %s]", query.c_str());
                return NOAFFECTED;
            }
        } else {
            LOG_WARN("event=[mysqlquery] type=[execute] status=[failed] message=[%s]",
                mysql_error(mysql));
            return Status::FAILED;
        }
    }
    return Status::OK;
}

int MySQLQuery::next(std::vector<char*>& row_data) {
    if (!conn_) {
        LOG_WARN("event=[mysqlquery] type=[execute] status=[invalid]");
        return Status::NOHANLDER;
    }
    row_data.clear();

    MYSQL_ROW row = nullptr;
    MYSQL* mysql = conn_->handler();
    if (!mysql) {
        LOG_WARN("event=[mysqlquery] type=[execute] status=[invalid]");
        return Status::NOHANLDER;
    }
    if (!result_) {
        LOG_WARN("event=[mysqlquery] type=[execute] status=[invalid]");
        return Status::NORESULT;
    }
    row = mysql_fetch_row(result_);
    if (!row) {
        //null row and none 0 errno, indicationg an error
        if (mysql_errno(mysql)) {
            LOG_WARN("event=[mysqlquery] type=[execute] status=[failed] message=[%s]",
                mysql_error(mysql));
            return Status::FAILED;
        } else {
            if (result_) {
                mysql_free_result(result_);
                result_ = nullptr;
            }
            return Status::OK;
        }
    }
    int fields_num = mysql_field_count(mysql);
    row_data.reserve(fields_num);

    for (int i = 0; i < fields_num; ++i) {
        row_data.push_back(row[i]);
    }
    return Status::OK;
}

std::string MySQLQuery::escape(const std::string& raw) {
    if (!conn_) {
        LOG_WARN("event=[mysqlquery] type=[execute] status=[invalid]");
        return "";
    }
    std::unique_ptr<char[]> escaped_string( new char[raw.size() << 1 + 1]);

    int size = mysql_real_escape_string(conn_->handler(), escaped_string.get(), raw.c_str(), raw.size());

    return std::string(escaped_string.get(), size);
}

// END_NAMESPACE
