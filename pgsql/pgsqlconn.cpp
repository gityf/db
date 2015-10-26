/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class PgSQLConn.
*/
#include <sstream>
#include "pgsqlconn.hpp"
#include "log/log.h"

// START_NAMESPACE

PgSQLConn::PgSQLConn() {
}

PgSQLConn::PgSQLConn(const Options& opt) : conn_(nullptr), option_() {
    init(opt);
}

void PgSQLConn::init(const Options& opt) {
    option_.host = opt.host;
    option_.port = opt.port;
    option_.user = opt.user;
    option_.passwd = opt.passwd;
    option_.database = opt.database;
}

std::string PgSQLConn::dbConnStr(const Options& opt) {
    std::ostringstream o;
    o << "host = " << opt.host
        << " port = " << opt.port
        << " dbname = " << opt.database;
    if (!opt.user.empty()) {
        o << " user = " << opt.user
            << " password = " << opt.passwd;
    }
    return o.str();
}

PgSQLConn::~PgSQLConn() {
    close();
}

int PgSQLConn::connect() {
    string connStr = dbConnStr(option_);
    conn_ = PQconnectdb(connStr.c_str());
    if (PQstatus(conn_) == CONNECTION_BAD) {
        LOG_FATAL("connect pgsql failed:%s.", PQerrorMessage(conn_));
        return Status::FAILED;
    }
    LOG_INFO("connected to pgsql host=[%s:%d]",
        option_.host.c_str(), option_.port);
    return Status::OK;
}

int PgSQLConn::close() {
    if (conn_) {
        LOG_INFO("close pgsql host=[%s:%d]",
        option_.host.c_str(), option_.port);
        PQfinish(conn_);
        conn_ = nullptr;
    }
    return 0;
}

PGconn* PgSQLConn::handler() {
    return conn_;
}

// END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
