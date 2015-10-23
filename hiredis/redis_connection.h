// Copyright (c) 2015, kuaidi.
// All rights reserved.
//
// Author: Wang Yaofu
// Created: 04/21/15
// Description: header file of redis connection utils.

#ifndef _DB_REDIS_REDIS_CONNECTION_H
#define _DB_REDIS_REDIS_CONNECTION_H
#pragma once

#include "hiredis.h"
#include <vector>
#include <string>
#include <stdint.h>
using std::string;
using std::vector;

enum ERedisResult {
    kOk = 0,
    kNullParam,
    kNoResult,
    kError,
    kConnErr
};

class RedisConnection {
public:
    struct Options {
        std::string mHost = { "127.0.0.1" }; // host ip
        int mPort = { 6379};                   // redis host port
        uint64_t mTimeOut = { 1500 };           // timeout
        uint64_t mSessionTimeout = { 3600000 }; // socket timeout
        int mDbId = { 0 };                      // table index
    };
    RedisConnection() : mRedisConn(NULL) {}
    RedisConnection(const Options& opt) {
        option_ = opt;
        mRedisConn = NULL;
    }

    virtual ~RedisConnection() {}

    ERedisResult GetResultCode() {
        return mResultCode;
    }
    string GetErrDesc();

    int Connect(const Options& opt);

    void DisConnect();

    int RunCmdToArray(const string& aSetCmd,
                      vector<string>* aOutSets);

    int RunCmd(const char* aCmdStr);

    void FreeReply(redisReply* pReply);

    const redisContext* GetRedisConnection() const;

    int Set(const string& aKey, const string& aValue);

    int GetStr(const string& aKey, string* aValue);

    int Get(const string& aKey, string* aValue);

    int HashGet(const string& aKey,
                const string& aFieldKey,
                string* aFieldValue);

    int HashSet(const string& aKey,
                const string& aFieldKey,
                const string& aFieldValue);

    int HashSetNum(const string& aKey,
                   const string& aFieldKey,
                   const int& aFieldVal);

    int HashKeys(const string& aKey,
                 vector<string>* aOutKeys);
    int HashDelKey(const string& aKey, const string& aFieldKey);

    int HashIncr(const string& aKey,
                 const string& aFieldKey,
                 const int& nIncrValue = 1);

    int ZsetIncr(const string& aKey,
                 const string& aMember,
                 const int& nIncrValue = 1);
    int Eval(const string& aScript,
             const vector<string>& aKeys,
             const vector<string>& aArgs);


private:
    int ReConnect(int table_index=0);

private:
    Options option_;
    
    redisContext* mRedisConn;  // redis connection object context
    ERedisResult mResultCode;
};
// typedef std::shared_ptr<RedisConnection> RedisConnectionPtr;
#endif // _DB_REDIS_REDIS_CONNECTION_H
