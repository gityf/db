#pragma once
#include <libmemcached/memcached.h>
#include <stdio.h>

#define VALUELEN 150000

class ssMemcached {
public:
    memcached_server_st *servers_;
    memcached_st *memc_;
    memcached_return retCode_;
    ssMemcached();
    int destory();
    int initServer(char *pServerInfo, int nPort);
    int get(char *sKey, size_t nKeyLen, char *pReturnData, size_t *pReturnLen);
    int multiget(char *sKeySet[], size_t * nKeySetLen,
        int nKeySetNum, char *pReturnData[], char *pKeySet[], const int nValueLen);
    int set(char *sKey, size_t nKeyLen, char *pValue, int pValueLen, int nTime);
    int add(char *sKey, size_t nKeyLen, char *pValue, int pValueLen, int nTime);
};
