#include "ssMemcached.h"

#define CACHEKEYLEN 32

ssMemcached::ssMemcached() {
    servers_ = NULL;
    memc_ = memcached_create(NULL);
}

//destory memcache
int ssMemcached::destory() {
    memcached_free(memc_);
    memcached_server_free(servers_);
    return 0;
}
//init Server
int ssMemcached::initServer(char *pServerInfo, int nPort) {
    servers_ = memcached_server_list_append(servers_, pServerInfo, nPort, &retCode_);
    retCode_ = memcached_server_push(memc_, servers_);

    if (retCode_ == MEMCACHED_SUCCESS) {
        //fprintf(stderr,"Added server successfully\n");
    }
    else {
        fprintf(stderr, "Couldn't add server: %s\n", memcached_strerror(memc_, retCode_));
        return 0;
    }

    //connect timeout
    memcached_behavior_set(memc_, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 100);
    //read timeout
    memcached_behavior_set(memc_, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 200);
    //write timeout
    memcached_behavior_set(memc_, MEMCACHED_BEHAVIOR_SND_TIMEOUT, 200);
    memcached_behavior_set(memc_, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 400);

    return 1;
}

//memcache get
int ssMemcached::get(char *sKey, size_t nKeyLen,
    char *pReturnData, size_t *pReturnLen) {
    uint32_t flag;
    char *pData;
    size_t nLen;
    //fprintf(stderr, "address : %x",pReturnData);

    pData = memcached_get(memc_, sKey, nKeyLen, &nLen, &flag, &retCode_);
    *pReturnData = nLen;
    strncpy(pReturnData, pData, nLen);

    free(pData);

    if (retCode_ == MEMCACHED_SUCCESS) {
        //fprintf(stderr,"key get successfully. value %s:%x\n",pReturnData,pReturnData);
    }
    else {
        fprintf(stderr, "key:%s get error\n", sKey);
        return 0;
    }
    return 1;
}

int ssMemcached::multiget(char *sKeySet[], size_t *nKeySetLen,
    int nKeySetNum, char *pReturnData[],
    char *pKeySet[], const int nValueLen) {
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;
    uint32_t flags;
    int index = 0;
    memset(return_key, 0, sizeof(return_key));
    retCode_ = memcached_mget(memc_, sKeySet, nKeySetLen, nKeySetNum);
    if (retCode_ == MEMCACHED_SUCCESS) {
        int i = 0;
        return_value = memcached_fetch(memc_, return_key,
            &return_key_length, &return_value_length, &flags, &retCode_);
        while (return_value != NULL) {
            if (retCode_ == MEMCACHED_SUCCESS) {
                for (i = 0; i < nKeySetNum; i++) {
                    index = index % nKeySetNum;
                    if (strcmp(sKeySet[index], return_key) == 0) {
                        break;
                    }
                    else {
                        index = (index + 1) % nKeySetNum;
                    }
                }
                if (i < nKeySetNum) {
                    if (return_value_length >= nValueLen) {
                        strncpy(pReturnData[index], return_value, nValueLen - 1);
                    }
                    else {
                        strncpy(pReturnData[index], return_value, return_value_length);
                    }
                    if (return_key_length >= CACHEKEYLEN) {
                        strncpy(pKeySet[index], return_key, CACHEKEYLEN - 1);
                    }
                    else {
                        strncpy(pKeySet[index], return_key, return_key_length);
                    }
                    memset(return_key, 0, sizeof(return_key));
                    index++;
                    free(return_value);
                }
                else {
                    return 0;
                }
            }
            else {
                return 0;
            }
        }
    }
    else {
        return 0;
    }
    return 1;
}

int ssMemcached::set(char *sKey, size_t nKeyLen, char *pValue, int pValueLen, int nTime) {
    uint32_t flag = 0;

    retCode_ = memcached_set(memc_, sKey, nKeyLen, pValue, pValueLen, nTime, flag);

    if (retCode_ == MEMCACHED_SUCCESS) {
        //fprintf(stderr,"Key stored successfully\n");
    }
    else {
        fprintf(stderr, "Couldn't store key: %s\n", memcached_strerror(memc_, retCode_));
        return 0;
    }
    return 1;
}

int ssMemcached::add(char *sKey, size_t nKeyLen, char *pValue, int pValueLen, int nTime) {
    uint32_t flag = 0;

    retCode_ = memcached_add(memc_, sKey, nKeyLen, pValue, pValueLen, nTime, flag);

    if (retCode_ == MEMCACHED_SUCCESS) {
        //fprintf(stderr,"Key stored successfully\n");
    }
    else {
        fprintf(stderr, "Couldn't store key: %s\n", memcached_strerror(memc_, retCode_));
        return 0;
    }
    return 1;
}

