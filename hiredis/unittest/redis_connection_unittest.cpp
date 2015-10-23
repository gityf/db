/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class RedisConnectionTest.
*/
#include "redis_connection.h"
#include <sstream>
#include "test_harness.h"

TEST(RedisConnectionTest, RedisBasicTest) {
    RedisConnection m_redis_conn;
    RedisConnection::Options option;
    int ret = m_redis_conn.Connect(option);
    ASSERT_EQ(ret, 0);

    std::string key("name");
    std::stringstream ss;
    ss << "set " << key << " pengchong";
    std::string command;
    command = ss.str();
    int command_ret = m_redis_conn.RunCmd(command.c_str());
    EXPECT_EQ(command_ret, 0);

    string result;
    command_ret = m_redis_conn.GetStr(key, &result);
    EXPECT_EQ(command_ret, 0);
    EXPECT_EQ(result.length(), 9);
    EXPECT_EQ(result, "pengchong");

    ss.str("");

    ss << "del " << key;
    command = "";

    command = ss.str();
    command_ret = m_redis_conn.RunCmd(command.c_str());

    EXPECT_EQ(command_ret, 0);

    ss.str("");
    ss << "zadd testset 1 a 1 b";
    command_ret = m_redis_conn.RunCmd(ss.str().c_str());
    EXPECT_EQ(command_ret, 0);

    ss.str("");
    ss << "zrange testset 0 -1";
    std::vector<std::string> set;

    command_ret = m_redis_conn.RunCmdToArray(ss.str(), &set);

    EXPECT_EQ((size_t)2, set.size());

    std::vector<std::string>::iterator iter = set.begin();
    bool eq = *iter == std::string("a");
    EXPECT_TRUE(eq);
    iter++;
    eq = *iter == std::string("b");
    EXPECT_TRUE(eq);

    std::string hash_name = "team_hash";
    std::string team_id_1 = "1";
    std::string team_id_1_val = "1 is best";
    std::string team_id_2 = "2";
    std::string team_id_2_val = "2 is better";

    command_ret = m_redis_conn.HashSet(hash_name, team_id_1, team_id_1_val);
    EXPECT_EQ(command_ret, 0);

    command_ret = m_redis_conn.HashSet(hash_name, team_id_2, team_id_2_val);

    std::vector<std::string> hkeys;

    command_ret = m_redis_conn.HashKeys(hash_name,
                                            &hkeys);
    EXPECT_TRUE(hkeys[0] == "1" || hkeys[0] == "2");
    EXPECT_TRUE(hkeys[1] == "2" || hkeys[1] == "1");

    command_ret = m_redis_conn.HashDelKey(hash_name, team_id_1);
    EXPECT_EQ(command_ret, 0);

    command_ret = m_redis_conn.HashKeys(hash_name,
                                            &hkeys);

    EXPECT_TRUE(hkeys[0] == "2");

    m_redis_conn.DisConnect();
};
