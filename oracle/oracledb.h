/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class OracleDB.
*/
#ifndef _DB_CORACLEDB_
#define _DB_CORACLEDB_
#include <string>
//
#define OTL_ORA9I // Compile OTL 4.0/OCI9i
#define OTL_ORA_TIMESTAMP // enable Oracle 9i TIMESTAMPs [with [local] time zone]
#include "otlv4.h"

#define RET_ERROR -1
#define RET_OK    0
using std::string;

struct SOracleConnect {
    // user-name for database.
    string userName_;
    // database password for database.
    string password_;
    // database name.
    string database_;
};

class OracleDB {
public:
    // initialize operation.
    int InitData(const SOracleConnect& aOracleConn);
    // connect to database.
    int ConnDb();
    // execution sql statement.
    int ExecSql(const string &aSql, int aCommitFlag);
    // execution select sql statement.
    int ExecSelectSql(const string& aSql);
    // fetch result after select is executed.
    int FetchNext(char *aOut) {
        if (aOut != NULL && otlStream_.good() && !otlStream_.eof()) {
            otlStream_ >> aOut;
            return RET_OK;
        }
        return RET_ERROR;
    }
    int FetchNext(int &aOut) {
        if (otlStream_.good() && !otlStream_.eof()) {
            otlStream_>>aOut;
            return RET_OK;
        }
        return RET_ERROR;
    }
    int FetchNext(long &aOut) {
        if (otlStream_.good() && !otlStream_.eof()) {
            otlStream_>>aOut;
            return RET_OK;
        }
        return RET_ERROR;
    }

    void Logoff() {
        try {
            otlConn_.logoff();
        } catch (...) {
        }
        isOtlConnOK_ = false;
    }
    // commit operation.
    int Commit();
    // roll back operation.
    int Rollback();
    // heart beat to oracle-db.
    int HB();
    OracleDB();
    ~OracleDB();
private:
    // database infos.
    struct SOracleConnect oracleConn_;
    // otl_connect
    otl_connect otlConn_;
    otl_stream otlStream_;
    bool isOtlConnOK_;
};
#endif  // _DB_CORACLEDB_