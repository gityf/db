// Copyright (c) 2015, kuaidi.
// All rights reserved.
//
// Author: Wang Yaofu
// Created: 04/21/15
// Description: header file of redis connection utils.

#include <iostream>
#include <sstream>
#include "redis_connection.h"
#include "log/log.h"

#define RET_ERROR -1
#define RET_OK     0

int RedisConnection::Connect(const Options& opt) {
    option_ = opt;
    return ReConnect(option_.mDbId);
}

int RedisConnection::ReConnect(int aDbId) {
    DisConnect();

    mResultCode = kOk;
    if (option_.mTimeOut <= 0) {
        mRedisConn = redisConnect(option_.mHost.c_str(), option_.mPort);
    } else {
        struct timeval timeOut;
        timeOut.tv_sec = option_.mTimeOut / 1000;
        timeOut.tv_usec = (option_.mTimeOut % 1000) * 1000;
        mRedisConn = redisConnectWithTimeout(option_.mHost.c_str(),
            option_.mPort, timeOut);
    }

    if (NULL == mRedisConn || mRedisConn->err) {
        LOG_FATAL("connect to redis server %s:%d, failed:%s.",
            option_.mHost.c_str(), option_.mPort, mRedisConn->errstr);
        return RET_ERROR;
    }

    if (option_.mSessionTimeout > 0) {
        struct timeval timeOut;
        timeOut.tv_sec = option_.mSessionTimeout;
        timeOut.tv_usec = (option_.mSessionTimeout % 1000) * 1000;
        redisSetTimeout(mRedisConn, timeOut);
    }

    std::stringstream ss;
    ss << aDbId;
    std::string select_table = "select " + ss.str();
    int cmd_ret = this->RunCmd(select_table.c_str());

    LOG_INFO("connect to redis server %s:%d,"
        "select table:%s, select table result:%d. ",
        option_.mHost.c_str(), option_.mPort,
        option_.mDbId, cmd_ret);

    return RET_OK;
}

void RedisConnection::DisConnect() {
    if (NULL != mRedisConn) {
        redisFree(mRedisConn);
        mRedisConn = NULL;
    }
}

int RedisConnection::RunCmdToArray(const string& aSetCmd, vector<string>* aOutSets) {
    if (NULL == aOutSets) {
        LOG_ERROR("out container is nil.");
        mResultCode = kNullParam;
        return RET_ERROR;
    }
    aOutSets->clear();
    int tryTimes = 2, ret = RET_OK;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn, aSetCmd.c_str());
        if (NULL != pReply) {
            if (pReply->type == REDIS_REPLY_ARRAY) {
                for (unsigned int i = 0; i < pReply->elements; ++i) {
                    redisReply* child_reply = pReply->element[i];
                    if (child_reply->type == REDIS_REPLY_STRING) {
                        aOutSets->push_back(string(child_reply->str));
                    }
                }
            } else {
                mResultCode = kNoResult;
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::RunCmd(const char* aCmdStr) {
    if (NULL == aCmdStr) {
        LOG_ERROR("redis command is nil.");
        mResultCode = kNullParam;
        return RET_ERROR;
    }

    int tryTime = 2, ret = RET_OK;
    do {
        ret = RET_OK;
        redisReply *pReply = (redisReply*)redisCommand(mRedisConn, aCmdStr);
        if (NULL != pReply) {
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect failed in safe command");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTime > 0);

    return ret;
}

void RedisConnection::FreeReply(redisReply* pReply) {
    if (NULL == pReply) {
        LOG_ERROR("free nil redis reply");
        return;
    }

    freeReplyObject(pReply);
}

const redisContext* RedisConnection::GetRedisConnection() const {
    return mRedisConn;
}

int RedisConnection::Set(const string& aKey, const string& aValue) {
    int tryTimes = 2, ret = RET_OK;
    mResultCode = kOk;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "SET %b %b",
            aKey.c_str(), aKey.length(), aValue.c_str(), aValue.length());
        if (NULL != pReply) {
            if (REDIS_REPLY_STATUS == pReply->type && pReply->str[0] == 'O') {
                // successfully
            } else {
                mResultCode = kError;
                ret = RET_ERROR;
                LOG_ERROR("set key:value %s:%s, error info:%s.",
                    aKey.c_str(), aValue.c_str(), pReply->str);
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);

    return ret;
}

int RedisConnection::GetStr(const string& aKey, string* aValue) {
    if (NULL == aValue) {
        LOG_ERROR("aValue is nil");
        mResultCode = kNullParam;
        return RET_ERROR;
    }
    mResultCode = kOk;
    int tryTimes = 2, ret = RET_OK;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "GET %s", aKey.c_str());
        if (NULL != pReply) {
            if (pReply->type == REDIS_REPLY_STRING) {
                aValue->assign(pReply->str, pReply->len);
            } else {
                mResultCode = kNoResult;
                LOG_ERROR("get key %s, error info:%s.",
                    aKey.c_str(), pReply->str);
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);

    return ret;
}

int RedisConnection::Get(const string& aKey, string* aValue) {
    if (NULL == aValue) {
        LOG_ERROR("aValue is nil");
        mResultCode = kNullParam;
        return RET_ERROR;
    }
    mResultCode = kOk;
    int tryTimes = 2, ret = RET_OK;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "GET %b", aKey.c_str(), aKey.length());
        if (NULL != pReply) {
            if (pReply->type == REDIS_REPLY_STRING) {
                aValue->assign(pReply->str, pReply->len);
            } else {
                mResultCode = kNoResult;
                LOG_ERROR("get key %s, error info:%s.",
                    aKey.c_str(), pReply->str);
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);

    return ret;
}

int RedisConnection::HashGet(const string& aKey,
                              const string& aFieldKey,
                              string* aFieldValue) {
    if (NULL == aFieldValue) {
        LOG_ERROR("aFieldValue is nil");
        mResultCode = kNullParam;
        return RET_ERROR;
    }
    int tryTimes = 2, ret = RET_OK;
    mResultCode = kOk;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "HGET %b %b",
            aKey.c_str(), aKey.length(), aFieldKey.c_str(), aFieldKey.length());
        if (NULL != pReply) {
            if (REDIS_REPLY_STRING != pReply->type) {
                mResultCode = kError;
                ret = RET_ERROR;
            } else {
                aFieldValue->assign(pReply->str, pReply->len);
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect failed");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::HashSet(const string& aKey,
                              const string& aFieldKey,
                              const string& aFieldValue) {
    int tryTimes = 2, ret = RET_OK;
    mResultCode = kOk;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "HSET %s %s %b",
            aKey.c_str(), aFieldKey.c_str(), aFieldValue.c_str(), aFieldValue.length());
        if (NULL != pReply) {
            if (REDIS_REPLY_INTEGER != pReply->type) {
                mResultCode = kError;
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::HashSetNum(const string& aKey,
                                 const string& aFieldKey,
                                 const int& aFieldVal) {
    int tryTimes = 2, ret = RET_OK;
    mResultCode = kOk;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "HSET %b %b %d",
            aKey.c_str(), aKey.length(), aFieldKey.c_str(), aFieldKey.length(),
            aFieldVal);
        if (NULL != pReply) {
            if (REDIS_REPLY_INTEGER != pReply->type) {
                mResultCode = kError;
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::HashKeys(const string& aKey,
                               vector<string>* aOutKeys) {
    if (aOutKeys == NULL) {
        LOG_ERROR("aOutKeys is nil");
        return RET_ERROR;
        mResultCode = kNullParam;
    }
    mResultCode = kOk;
    aOutKeys->clear();
    int tryTimes = 2, ret = RET_OK;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "HKEYS %b",
            aKey.c_str(), aKey.length());
        if (NULL != pReply) {
            if (pReply->type == REDIS_REPLY_ARRAY) {
                for (unsigned int i = 0; i < pReply->elements; ++i) {
                    redisReply* child_reply = pReply->element[i];
                    if (child_reply->type == REDIS_REPLY_STRING) {
                        aOutKeys->push_back(string(child_reply->str));
                    }
                }
            } else {
                mResultCode = kError;
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::HashDelKey(const string& aKey, const string& aFieldKey) {
    int tryTimes = 2, ret = RET_OK;
    mResultCode = kOk;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "HDEL %b %b",
            aKey.c_str(), aKey.length(), aFieldKey.c_str(), aFieldKey.length());
        if (NULL != pReply) {
            if (REDIS_REPLY_INTEGER != pReply->type) {
                mResultCode = kError;
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::HashIncr(const string& aKey,
                               const string& aFieldKey,
                               const int& nIncrValue) {
    int tryTimes = 2, ret = RET_OK;
    mResultCode = kOk;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "HINCRBY %b %b %d",
            aKey.c_str(), aKey.length(), aFieldKey.c_str(), aFieldKey.length(),
            nIncrValue);
        if (NULL != pReply) {
            if (REDIS_REPLY_INTEGER != pReply->type) {
                mResultCode = kError;
                ret = RET_ERROR;
            }
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::ZsetIncr(const string& aKey,
                               const string& aMember,
                               const int& nIncrValue) {
    int tryTimes = 2, ret = RET_OK;
    mResultCode = kOk;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "ZINCRBY %b %d %b",
            aKey.c_str(), aKey.length(), nIncrValue,
            aMember.c_str(), aMember.length());
        if (NULL != pReply) {
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}

int RedisConnection::Eval(const string& aScript,
                           const vector<string>& aKeys,
                           const vector<string>& aArgs) {
    mResultCode = kOk;
    string keys = " ";
    string args = " ";
    for (int ii = 0; ii < aKeys.size(); ii++) {
        keys.append(aKeys[ii]);
        args.append(aArgs[ii]);
        keys += " ";
        args += " ";
    }
    int tryTimes = 2, ret = RET_OK;
    do {
        ret = RET_OK;
        redisReply* pReply = (redisReply*)redisCommand(mRedisConn,
            "EVAL %s %d %s %s",
            aScript.c_str(), aKeys.size(), keys.c_str(), args.c_str());
        if (NULL != pReply) {
            freeReplyObject(pReply);
            break;
        }
        if (ReConnect() < 0) {
            LOG_ERROR("reconnect redis failed.");
            ret = RET_ERROR;
            break;
        }
    } while(--tryTimes > 0);
    return ret;
}
