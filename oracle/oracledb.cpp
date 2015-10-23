#include "log/log.h"
#include "oracledb.h"

// constructor
OracleDB::OracleDB() {
    isOtlConnOK_ = false;
}
// destructor
OracleDB::~OracleDB() {
}

// initialize operation.
int OracleDB::InitData(const SOracleConnect& aOracleConn) {
    oracleConn_ = aOracleConn;
    return ConnDb();
}
// connect to database.
int OracleDB::ConnDb() {
    LOG_DEBUG("Begin");
    try {
        LOG_VARS("try to connect db.");
        // initialize OCI environment
        otl_connect::otl_initialize();
        string dbstr = oracleConn_.userName_;
        dbstr += "/";
        dbstr += oracleConn_.password_;
        dbstr += "@";
        dbstr += oracleConn_.database_;
        // connect to Oracle
        otlConn_.rlogon(dbstr.c_str());
        LOG_DEBUG("connect db ok.");
        isOtlConnOK_ = true;
        return RET_OK;
    } catch (otl_exception &p) {
        LOG_ERROR("connect db error, msg=(%s).", p.msg);
    } catch (...) {
        LOG_ERROR("connect db error.");
    }
    isOtlConnOK_ = false;
    return RET_ERROR;
}

// execution sql statement.
int OracleDB::ExecSql(const string& aSql, int aCommitFlag) {
    try {
        if (!isOtlConnOK_ && RET_ERROR == ConnDb()) {
            LOG_ERROR("can not connect to db.");
            return RET_ERROR;
        }
        LOG_DEBUG("sql=(%s)", aSql.c_str());
        otl_cursor::direct_exec(otlConn_,
            aSql.c_str(),
            otl_exception::enabled);

        if (aCommitFlag == 1) {
            otl_cursor::direct_exec(otlConn_,
                "commit",
                otl_exception::enabled);
        }
        LOG_DEBUG("OK.End");
        return RET_OK;
    } catch (otl_exception &p) {
         LOG_ERROR("{%s} error, msg=(%s),stm_text=(%s),"
             "sqlstate=(%s),var_info=(%s).",
             aSql.c_str(), p.msg, p.stm_text, p.sqlstate, p.var_info);
    } catch (...) {
        LOG_ERROR("{%s} error.", aSql.c_str());
    }
    if (aCommitFlag == 1) {
        int retv = Rollback();
        LOG_INFO("rollback-(%s)", (RET_OK == retv) ? "OK":"Fail");
    }
    LOG_INFO("Err.End");
    return RET_ERROR;
}

// execution sql statement.
int OracleDB::ExecSelectSql(const string& aSql) {
    try {
        if (!isOtlConnOK_ && RET_ERROR == ConnDb()) {
            LOG_ERROR("can not connect to db.");
            return RET_ERROR;
        }
        otlStream_.clean();
        otlStream_.close();
        otlStream_.open(1000, aSql.c_str(), otlConn_);
        LOG_DEBUG("OK.End");
        return RET_OK;
    }
    catch (otl_exception &p) {
        LOG_ERROR("{%s} error, msg=(%s),stm_text=(%s),"
            "sqlstate=(%s),var_info=(%s).",
            aSql.c_str(), p.msg, p.stm_text, p.sqlstate, p.var_info);
    }
    catch (...) {
        LOG_ERROR("{%s} error.", aSql.c_str());
    }
    return RET_ERROR;
}

// commit operation.
int OracleDB::Commit() {
    try {
        otl_cursor::direct_exec(otlConn_,
            "commit",
            otl_exception::enabled);

        return RET_OK;
    }
    catch (otl_exception &p) {
        LOG_ERROR("commit error, msg=(%s),stm_text=(%s),"
            "sqlstate=(%s),var_info=(%s).",
            p.msg, p.stm_text, p.sqlstate, p.var_info);
    }
    catch (...) {
        LOG_ERROR("commit unknown error.");
    }
    isOtlConnOK_ = false;
    return RET_ERROR;
}

// roll back operation.
int OracleDB::Rollback() {
    try {
        otlConn_.rollback();
    } catch (...) {
        try {
            otlConn_.logoff();
        } catch (...) {
        }
        isOtlConnOK_ = false;
        return RET_ERROR;
    }

    return RET_OK;
}

// heart beat to oracle-db.
int OracleDB::HB() {
    const string hbsql = "select sysdate from dual";
    return ExecSql(hbsql, 0);
}