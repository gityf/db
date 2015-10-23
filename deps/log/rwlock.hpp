/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class RWLock.
*/
#pragma once
#include <pthread.h>

class RWLock {
public:
    explicit RWLock(const pthread_rwlockattr_t* attr = NULL) {
        ::pthread_rwlock_init(&m_lv, attr);
    }
    ~RWLock() {
        ::pthread_rwlock_destroy(&m_lv);
    }
    int RLock(){
        return pthread_rwlock_rdlock(&m_lv);
    }
    int WLock(){
        return pthread_rwlock_wrlock(&m_lv);

    }
    int UnLock(){
        return pthread_rwlock_unlock(&m_lv);
    }

private:
    pthread_rwlock_t m_lv;
};