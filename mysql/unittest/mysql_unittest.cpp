/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class MySQLTest.
*/
#include <test_harness.h>
#include <iostream>
#include "MySQLQuery.hpp"
#include "log/log.h"

TEST(MySQLTest, BasicTest) {
    EXPECT_TRUE(true);
    MySQLConn* conn = new MySQLConn();
    MySQLConn::Options opt;
    opt.database = "test";
    opt.host = "127.0.0.1";
    opt.port = 3306;
    opt.user = "root";
    opt.passwd = "123456";
    LOG_INFO("db:[%s],host:[%s],port:[%d],user:[%s],passwd:[%s]",
        opt.database.c_str(),
        opt.host.c_str(),
        opt.port,
        opt.user.c_str(),
        opt.passwd.c_str());
    conn->init(opt);
    int ret = 0;
    EXPECT_EQ(ret, conn->connect());
    MySQLQuery* mysqlQry_ = new MySQLQuery();
    mysqlQry_->init(conn);

    // creat table
    string sql = "create table testdb(a varchar(10), b int)";
    EXPECT_EQ(ret, mysqlQry_->execute(sql));
    sql = "insert into testdb(a, b) values('str.111', 100)";
    EXPECT_EQ(ret, mysqlQry_->execute(sql));
    sql = "update testdb set a='str.222' where b>99";
    EXPECT_EQ(ret, mysqlQry_->execute(sql));
    sql = "select a, b from testdb where b>99";
    EXPECT_EQ(ret, mysqlQry_->execute(sql));
    std::vector<char*> res;
    EXPECT_EQ(ret, mysqlQry_->next(res));
    EXPECT_EQ(res.size(), 2);
    string val = "str.222";
    EXPECT_EQ(val, res[0]);
    val = "100";
    EXPECT_EQ(val, res[1]);
    sql = "delete from testdb where b=100";
    EXPECT_EQ(ret, mysqlQry_->execute(sql));
    sql = "drop table testdb";
    EXPECT_EQ(ret, mysqlQry_->execute(sql));
    delete conn;
    delete mysqlQry_;
}
