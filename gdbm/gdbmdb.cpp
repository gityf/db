/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class GdbmDB.
*/
#include "gdbmdb.hpp"
#include <assert.h>
#include <stdlib.h>
#include "log/log.h"

// START_NAMESPACE

GdbmDB::GdbmDB() : gdbmdb_(NULL)
{
}

GdbmDB::~GdbmDB() {
    if (gdbmdb_ != NULL) {
        gdbm_close(gdbmdb_);
    }
}

void GdbmDB::setOption(const Options& opt) {
    option_ = opt;
}

int GdbmDB::open(const std::string& db) {
    if (gdbmdb_ != NULL) {
        gdbm_close(gdbmdb_);
        gdbmdb_ = NULL;
    }
    gdbmdb_ = gdbm_open(db.c_str(), option_.blockSize,
        option_.readWrite, option_.mode, 0);
    if (gdbmdb_ == NULL) {
        LOG_ERROR("gdbm_open failed:%s", errMsg());
        return FAILED;
    }
    return OK;
}

int GdbmDB::put(const std::string& key, const std::string& value) {
    if (!gdbmdb_) {
        return FAILED;
    }
    datum datKey;
    datKey.dptr = const_cast<char*>(key.c_str());
    datKey.dsize = key.length();

    datum datValue;
    datValue.dptr = const_cast<char*>(value.c_str());
    datValue.dsize = value.length();
    int flag = exist(key) ? GDBM_REPLACE : GDBM_INSERT;
    if (gdbm_store(gdbmdb_, datKey, datValue, flag) < 0) {
        LOG_ERROR("gdbm_store failed:%s", errMsg());
        return FAILED;
    }
    gdbm_sync(gdbmdb_);
    return OK;
}

int GdbmDB::get(const std::string& key, std::string& value) {
    if (!gdbmdb_) {
        return FAILED;
    }
    datum datKey;
    datKey.dptr = const_cast<char*>(key.c_str());
    datKey.dsize = key.length();

    datum datValue = gdbm_fetch(gdbmdb_, datKey);
    if (datValue.dptr == NULL) {
        LOG_ERROR("gdbm_fetch failed:%s", errMsg());
        return FAILED;
    }
    value.assign(datValue.dptr, datValue.dsize);

    return OK;
}

int GdbmDB::del(const std::string& key) {
    if (!gdbmdb_) {
        return FAILED;
    }
    if (key.empty()) {
        return FAILED;
    }

    datum datKey;
    datKey.dptr = const_cast<char*>(key.c_str());
    datKey.dsize = key.length();

    if (gdbm_delete(gdbmdb_, datKey) < 0){
        LOG_ERROR("gdbm_delete failed:%s", errMsg());
        return FAILED;
    }
    gdbm_sync(gdbmdb_);
    return OK;
}

bool GdbmDB::exist(const std::string& key) {
    // true when return value of gdbm_exists is not zero.
    datum datKey;
    datKey.dptr = const_cast<char*>(key.c_str());
    datKey.dsize = key.length();
    return gdbm_exists(gdbmdb_, datKey) != 0;
}

int GdbmDB::reorganize() {
    if (gdbm_reorganize(gdbmdb_) < 0) {
        LOG_ERROR("gdbm_reorganize failed:%s", errMsg());
        return FAILED;
    }
    return OK;
}

gdbm_count_t GdbmDB::dbCount() {
    gdbm_count_t size;
    if (gdbm_count(gdbmdb_, &size) < 0) {
        LOG_ERROR("gdbm_count failed:%s", errMsg());
        return 0;
    }
    return size;
}

int GdbmDB::load(const std::string& fromFile) {
    unsigned long errLine;
    if (gdbm_load(&gdbmdb_, fromFile.c_str(), GDBM_REPLACE,
        GDBM_META_MASK_MODE | GDBM_META_MASK_OWNER, &errLine) < 0) {
        LOG_ERROR("gdbm_load line:%lu failed:%s", errLine, errMsg());
        return FAILED;
    }
    return OK;
}

int GdbmDB::dump(const std::string& toFile, int fmt) {
    if (gdbm_dump(gdbmdb_, toFile.c_str(), fmt, GDBM_NEWDB, 00664) < 0) {
        LOG_ERROR("gdbm_dump failed:%s", errMsg());
        return FAILED;
    }
    return OK;
}


int GdbmDB::dump(std::map<std::string, std::string>& toKeyVals) {
    datum key = gdbm_firstkey(gdbmdb_);
    datum nextkey;
    while (key.dptr != NULL) {
        datum value = gdbm_fetch(gdbmdb_, key);
        if (value.dptr != NULL){
            std::string toKey, toVal;
            toKey.assign(key.dptr, key.dsize);
            toVal.assign(value.dptr, value.dsize);
            toKeyVals.insert(std::make_pair(toKey, toVal));
            free(value.dptr);
        }
        nextkey = gdbm_nextkey(gdbmdb_, key);
        free(key.dptr);
        key = nextkey;
    }
    return OK;
}

const char* GdbmDB::errMsg() {
    return gdbm_strerror(gdbm_errno);
}
// END_NAMESPACE
/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
