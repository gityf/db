/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class LogWriter.
*/
#pragma once
#include <stdint.h>
#include <string>
#include "rwlock.hpp"

using std::string;
/*
* log level comments.
* LL_ERROR: error
* LL_WARN:  warning
* LL_NOTICE:  notice
* LL_INFO:  important information
* LL_VARS:  variable info
* LL_DBG:  debug info
* LL_ALL:   all info, lowest level
*/
enum ELogLevel {
    LL_NONE = 0,
    LL_ERROR = 1,
    LL_WARN,
    LL_NOTICE,
    LL_INFO,
    LL_VARS,
    LL_DBG,
    LL_ALL
};

class LogWriter {
public:
    LogWriter() {}
    ~LogWriter(){
        logclose();
    }
    bool loginit(ELogLevel l, const string& logfile);
    bool log(ELogLevel l, const char *format, ...);
    bool logclose();
private:
    void logHeader(char* header, int& len, ELogLevel level);
    uint64_t fileNode();
    const char* levelDesc(ELogLevel level);
private:
    int fd_;
    ELogLevel level_;
    uint64_t inode_;
    string logFile_;
    RWLock rwlock_;
};

extern LogWriter WARN_W;
extern LogWriter INFO_W;

#define LOG_FATAL(fmt, arg...) do { \
    WARN_W.log(LL_ERROR, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

#define LOG_ERROR(fmt, arg...) do { \
    WARN_W.log(LL_ERROR, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

#define LOG_WARN(fmt, arg...) do { \
    WARN_W.log(LL_WARN, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

#define LOG_NOTICE(fmt, arg...) do { \
    INFO_W.log(LL_NOTICE, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

#define LOG_INFO(fmt, arg...) do { \
    INFO_W.log(LL_INFO, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

#define LOG_VARS(fmt, arg...) do { \
    INFO_W.log(LL_VARS, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

#define LOG_DEBUG(fmt, arg...) do { \
    WARN_W.log(LL_DBG, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

#define LOG_ALL(fmt, arg...) do { \
    INFO_W.log(LL_ALL, "[%s:%d][%s] " fmt "\n", \
        __FILE__, __LINE__, __FUNCTION__, ##arg); \
} while (0)

bool log_init(ELogLevel l, const string& moduleName, const string& logDir);
bool log_close();