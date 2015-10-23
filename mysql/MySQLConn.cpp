#include "MySQLConn.hpp"
#include <assert.h>
#include "log/log.h"
#include <mutex>

// START_NAMESPACE

static std::mutex cg_conn_init_mutex;
MySQLConn::MySQLConn() {

}

MySQLConn::MySQLConn(const Options& opt) : conn_(nullptr), option_() {
    init(opt);
}

void MySQLConn::init(const Options& opt) {
    option_.host = opt.host;
    option_.port = opt.port;
    option_.user = opt.user;
    option_.passwd = opt.passwd;
    option_.database = opt.database;

    LOG_INFO("init mysql connection host=[%s:%d]",
        option_.host.c_str(), option_.port);

    std::lock_guard<std::mutex> lock(cg_conn_init_mutex);

    mysql_library_init(0, nullptr, nullptr);

    // mysql_init(nullptr) is not threadsafe
    conn_ = mysql_init(nullptr);
    assert(conn_);
}

MySQLConn::~MySQLConn() {
    close();
}
int MySQLConn::connect() {
    MYSQL* result =
        mysql_real_connect(conn_,
                           option_.host.c_str(),
                           option_.user.c_str(),
                           option_.passwd.c_str(),
                           option_.database.c_str(),
                           option_.port,
                           nullptr,
                           0);
    if (!result) {
        LOG_FATAL("event=[connectmysqlfailed] type=[connect]");
        return Status::FAILED;
    }

    int ret = mysql_set_character_set(conn_, "utf8");
    if (ret) {
        LOG_FATAL("connect mysql failed type=[setcharset]");
        return Status::FAILED;
    }
    LOG_INFO("connected to mysql host=[%s:%d]",
        option_.host.c_str(), option_.port);
    return Status::OK;
}
int MySQLConn::close() {
    if (conn_) {
        LOG_INFO("close mysql host=[%s:%d]",
        option_.host.c_str(), option_.port);
        mysql_close(conn_);
        conn_ = nullptr;
    }
    return 0;
}
MYSQL* MySQLConn::handler() {
    return conn_;
}

// END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
