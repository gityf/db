/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class GdbmDBTest.
*/
#include <test_harness.h>
#include <iostream>
#include "gdbmdb.hpp"
#include "log/log.h"

TEST(GdbmDBTest, BasicTest) {
    log_init(LL_ALL, "gdbmdb", "./log");
    EXPECT_TRUE(true);
    GdbmDB gdbmDb;
    gdbmDb.open("./db.gdbm");
    std::string value;
    std::string key = "key123";
    int status = gdbmDb.put(key, "value123");
    EXPECT_EQ(status, 0);
    status = gdbmDb.get(key, value);
    EXPECT_EQ(status, 0);
    EXPECT_EQ(value, "value123");
    EXPECT_EQ(gdbmDb.exist(key), true);
    gdbmDb.put("k2", "v2");
    EXPECT_EQ(gdbmDb.dbCount(), 2);
    std::map<string, string> keyVals;

    gdbmDb.dump(keyVals);
    for (std::map<string, string>::iterator it = keyVals.begin();
        it != keyVals.end(); ++it) {
        TEST_INFO("key:%s, value:%s",
            it->first.c_str(), it->second.c_str());
    }
    status = gdbmDb.dump("to.gdbm.new", 1);
    EXPECT_EQ(status, 0);
    status = gdbmDb.load("to.gdbm.new");
    EXPECT_EQ(status, 0);
    EXPECT_EQ(gdbmDb.dbCount(), 2);
    status = gdbmDb.del(key);
    EXPECT_EQ(status, 0);
}
