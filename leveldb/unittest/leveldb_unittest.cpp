/*
** Copyright (C) 2015 www.xiaojukeji.com
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class LevelDBTest.
*/
#include <test_harness.h>
#include <iostream>
#include "LevelDB.hpp"

TEST(LevelDBTest, BasicTest) {
    EXPECT_TRUE(true);
    LevelDB levelDb;
    levelDb.Open("./db");
    std::string value;
    std::string key = "key123";
    int status = levelDb.Put(key, "value123");
    EXPECT_EQ(status, 0);
    status = levelDb.Get(key, value);
    EXPECT_EQ(status, 0);
    EXPECT_EQ(value, "value123");
    status = levelDb.Del(key);
    EXPECT_EQ(status, 0);
}
