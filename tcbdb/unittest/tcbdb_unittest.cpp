/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class TcbDBTest.
*/
#include <test_harness.h>
#include <iostream>
#include "TcbDB.hpp"

TEST(TcbDBTest, BasicTest) {
    EXPECT_TRUE(true);
    TcbDB tcbDb;
    tcbDb.open("./tcb.db");
    std::string value;
    std::string key = "key123";
    int status = tcbDb.put(key, "value123");
    EXPECT_EQ(status, 0);
    status = tcbDb.get(key, value);
    EXPECT_EQ(status, 0);
    EXPECT_EQ(value, "value123");
    TcbDB toDb;
    status = tcbDb.copy("./tcb.new.db");
    toDb.open("./tcb.new.db");
    EXPECT_EQ(status, 0);
    value = "";
    toDb.get(key, value);
    EXPECT_EQ(value, "value123");
    tcbDb.append(key, "value123");
    status = tcbDb.get(key, value);
    EXPECT_EQ(status, 0);
    EXPECT_EQ(value, "value123value123");
    status = tcbDb.putDup(key, "v2");
    EXPECT_EQ(status, 0);
    EXPECT_EQ(16, tcbDb.valueSizeOfKey(key));

    status = tcbDb.del(key);
    EXPECT_EQ(status, 0);
}
