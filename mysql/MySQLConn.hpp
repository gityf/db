#pragma once

#include <string>
#include <mysql.h>

// START_NAMESPACE

class MySQLConn {
public:
    enum Status {
        OK,
        FAILED
    };
    struct Options {
        std::string host   = {"127.0.0.1"};
        int    port        = {3306};
        std::string user   = {"root"};
        std::string passwd;
        std::string database;
    };
    MySQLConn();
    MySQLConn(const Options& opt);
    virtual ~MySQLConn();
    void init(const Options& opt);

    virtual int connect();
    virtual int close();
    virtual MYSQL* handler();

private:
    MYSQL*     conn_;
    Options    option_;
};
// END_NAMESPACE
