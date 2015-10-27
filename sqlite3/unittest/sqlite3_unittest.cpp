/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class Sqlite3Test.
*/
#include <test_harness.h>
#include <iostream>
#include "sqlite3query.hpp"
#include "log/log.h"

TEST(Sqlite3Test, BasicTest) {
    EXPECT_TRUE(true);
    log_init(LL_ALL, "sqlite3", "./log");
    Sqlite3Query* sqlite3Qry_ = new Sqlite3Query();
    sqlite3Qry_->open("sqlite3.db");
    int ret = 0;
    // creat table
    string sql = "create table testdb(a varchar(10), b int)";
    EXPECT_EQ(ret, sqlite3Qry_->execute(sql));
    sql = "insert into testdb(a, b) values('str.111', 100)";
    EXPECT_EQ(ret, sqlite3Qry_->execute(sql));
    sql = "update testdb set a='str.222' where b>99";
    EXPECT_EQ(ret, sqlite3Qry_->execute(sql));
    sql = "select a, b from testdb where b>99";
    EXPECT_EQ(ret, sqlite3Qry_->execute(sql, true));
    std::vector<char*> res;
    EXPECT_EQ(ret, sqlite3Qry_->next(res));
    EXPECT_EQ(res.size(), 2);
    string val = "str.222";
    EXPECT_EQ(val, res[0]);
    val = "100";
    EXPECT_EQ(val, res[1]);
    sql = "delete from testdb where b=100";
    EXPECT_EQ(ret, sqlite3Qry_->execute(sql));
    sql = "drop table testdb";
    EXPECT_EQ(ret, sqlite3Qry_->execute(sql));
    delete sqlite3Qry_;
}
