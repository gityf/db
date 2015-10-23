/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class TcbDB.
*/
#include "TcbDB.hpp"
#include <assert.h>
#include "log/log.h"

// START_NAMESPACE
TcbDB::TcbDB() : tcbdb_(NULL)
{
}

TcbDB::~TcbDB() {
    if (tcbdb_) {
        tcbdbsync(tcbdb_);
        tcbdbclose(tcbdb_);
        tcbdbdel(tcbdb_);
        tcbdb_ = NULL;
    }
}

void TcbDB::setOption(const Options& opt) {
    options_ = opt;
}

int TcbDB::open(const std::string& db) {
    do {
        tcbdb_ = tcbdbnew();
        if (tcbdb_ == NULL) {
            LOG_ERROR("Attention: Unable to create the database.");
            break;
        }

        if (!tcbdbsetmutex(tcbdb_)) {
            LOG_ERROR("tcbdbsetmutex error.");
            break;
        }
        // Set the tuning parameters of a B+ tree database object.
        if (!tcbdbtune(tcbdb_, options_.membersOfLeaf,
            options_.membersOfNonLeaf,
            options_.elementsOfArray,
            options_.apow,
            options_.fpow,
            options_.opts)) {
            LOG_ERROR("tcbdbtune error.");
            break;
        }
        if (!tcbdbsetcache(tcbdb_, options_.maxNumOfLeaf * 2,
            options_.maxNumOfLeaf)) {
            LOG_ERROR("tcbdbsetcache error.");
            break;
        }
        if (!tcbdbsetxmsiz(tcbdb_, options_.extraMemorySize)) {
            LOG_ERROR("tcbdbsetxmsiz error.");
            break;
        }

        if (!tcbdbopen(tcbdb_, db.c_str(), BDBOWRITER | BDBOCREAT))
        {
            LOG_ERROR("Attention: Unable to open the database.");
            break;
        }
        return OK;
    } while (0);
    return FAILED;
}

int TcbDB::put(const std::string& key, const std::string& value) {
    if (!tcbdb_) {
        return FAILED;
    }
    if (!tcbdbput(tcbdb_, key.c_str(), key.length(), value.c_str(), value.length())) {
        return FAILED;
    }
    return OK;
}

int TcbDB::multiPut(const std::string& key, const std::vector<std::string>& values) {
    if (values.empty()) {
        return FAILED;
    }
    
    TCLIST* tcbListPtr = tclistnew2(values.size());
    if (tcbListPtr == NULL) {
        return FAILED;
    }
    for (size_t ii = 0; ii < values.size(); ++ii) {
        tclistpush(tcbListPtr, values[ii].c_str(), values[ii].length());
    }
    if (!tcbdbputdup3(tcbdb_, key.c_str(), key.length(), tcbListPtr)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::putInt(const std::string& key, int value) {
    if (!tcbdbaddint(tcbdb_, key.c_str(), key.length(), value)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::putDouble(const std::string& key, double value) {
    if (!tcbdbadddouble(tcbdb_, key.c_str(), key.length(), value)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::get(const std::string& key, std::string& value) {
    if (!tcbdb_) {
        return FAILED;
    }
    int len = 0;
    char *val = static_cast<char*>(tcbdbget(tcbdb_, key.c_str(), key.length(), &len));
    if (val == NULL) {
        return FAILED;
    }
    value.assign(val, len);
    free(val);

    return OK;
}

int TcbDB::multiGet(const std::string& beginKey,
    const std::string& endKey, int limits,
    std::vector<std::string>& values) {
    TCLIST* rets = tcbdbrange(tcbdb_,
        beginKey.c_str(), beginKey.length(), true,
        endKey.c_str(), endKey.length(), true, limits);
    if (rets == NULL) {
        return FAILED;
    }
    TCLIST2vector(rets, &values);

    return OK;
}

int TcbDB::getPrefix(const std::string& key, int limits,
    std::vector<std::string>& values) {
    TCLIST* rets = tcbdbfwmkeys(tcbdb_,
        key.c_str(), key.length(), limits);
    if (rets == NULL) {
        return FAILED;
    }
    TCLIST2vector(rets, &values);
    return OK;
}

int TcbDB::del(const std::string& key) {
    if (!tcbdb_) {
        return FAILED;
    }

    if (!tcbdbout(tcbdb_, key.c_str(), key.length())) {
        return FAILED;
    }
    return OK;
}

int TcbDB::append(const std::string& key, const std::string& value) {
    if (!tcbdbputcat(tcbdb_, key.c_str(), key.length(), value.c_str(), value.length())) {
        return FAILED;
    }
    return OK;
}

int TcbDB::putDup(const std::string& key, const std::string& value) {
    if (!tcbdbputdup(tcbdb_, key.c_str(), key.length(), value.c_str(), value.length())) {
        return FAILED;
    }
    return OK;
}

uint64_t TcbDB::keyCounts() {
    return tcbdbrnum(tcbdb_);
}

uint64_t TcbDB::dbSize() {
    return tcbdbfsiz(tcbdb_);
}

int TcbDB::numOfKey(const std::string& key) {
    return tcbdbvnum(tcbdb_, key.c_str(), key.length());
}

int TcbDB::valueSizeOfKey(const std::string& key) {
    return tcbdbvsiz(tcbdb_, key.c_str(), key.length());
}

int TcbDB::valuesOfKey(const std::string& key, std::vector<std::string>& values) {
    TCLIST* rets = tcbdbget4(tcbdb_, key.c_str(), key.length());
    if (rets == NULL) {
        return FAILED;
    }
    TCLIST2vector(rets, &values);
    
    return OK;    
}

int TcbDB::save() {
    if (!tcbdbsync(tcbdb_)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::destory() {
    if (!tcbdbvanish(tcbdb_)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::optimize(const Options& opt) {
    options_ = opt;
    if (!tcbdboptimize(tcbdb_, options_.membersOfLeaf,
        options_.membersOfNonLeaf,
        options_.elementsOfArray,
        options_.apow,
        options_.fpow,
        options_.opts)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::copy(const std::string& toDbPath) {
    if (!tcbdbcopy(tcbdb_, toDbPath.c_str())) {
        return FAILED;
    }
    return OK;
}

int TcbDB::startTransaction() {
    if (!tcbdbtranbegin(tcbdb_)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::commit() {
    if (!tcbdbtrancommit(tcbdb_)) {
        return FAILED;
    }
    return OK;
}

int TcbDB::rollback() {
    if (!tcbdbtranabort(tcbdb_)) {
        return FAILED;
    }
    return OK;
}

std::string TcbDB::dbPath() {
    return tcbdbpath(tcbdb_);
}

void TcbDB::TCLIST2vector(TCLIST* rets, std::vector<std::string>* values) {
    for (size_t ii = 0; ii < rets->anum; ++ii) {
        string value;
        value.assign(rets->array[ii].ptr, rets->array[ii].size);
        values->push_back(value);
    }
}
// END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
