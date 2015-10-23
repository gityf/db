/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class LogWriter.
*/
#include "log.h"
#include <sys/file.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/uio.h>

// iovec init
#define INIT_IOV(i)        \
struct iovec iovs[i];      \
int    iovs_count = 0;

#define SET_IOV_LEN(s, l)          \
{                                  \
    iovs[iovs_count].iov_base = const_cast<char *>(s); \
    iovs[iovs_count].iov_len = l; \
    iovs_count++;                  \
}

bool LogWriter::loginit(ELogLevel l, const string& logfile) {
    fd_ = ::open(logfile.c_str(), O_RDWR|O_APPEND|O_CREAT|O_LARGEFILE, 0644);
    if (0 > fd_) {
        //open failed
        return false;
    }
    level_ = l;
    logFile_ = logfile;
    inode_ = fileNode();
    return true;
}

bool LogWriter::log(ELogLevel l, const char* format, ...) {
    if (l > level_) {
        return false;
    }
    char* bPtr;
    va_list ap;
    va_start(ap, format);
    int blen = ::vasprintf(&bPtr, format, ap);
    if (blen < 0) {
        va_end(ap);
        return false;
    }
    va_end(ap);
    char header[100] = { 0 };
    int hlen = 0;
    logHeader(header, hlen, l);    
    INIT_IOV(2);
    SET_IOV_LEN(header, hlen);
    SET_IOV_LEN(bPtr, blen);
    if (inode_ != fileNode() || !inode_){
        rwlock_.WLock();
        if (inode_ != fileNode() || !inode_){
            logclose();
            loginit(level_, logFile_);
        }
        rwlock_.UnLock();
    }
    bool ret = true;
    rwlock_.RLock();
    if (::writev(fd_, iovs, 2) <= 0) {
        ret = false;
    }
    rwlock_.UnLock();
    free(bPtr);
    return ret;
}

bool LogWriter::logclose() {
    if (fd_ > 0) {
        ::close(fd_);
        fd_ = -1;
        inode_ = 0;
    }
    
    return true;
}

void LogWriter::logHeader(char* header, int& len, ELogLevel level) {
    struct timeval tmv;
    struct tm tme;
    memset(&tmv, 0, sizeof(tmv));
    memset(&tme, 0, sizeof(tme));
    ::gettimeofday(&tmv, NULL);
    ::localtime_r(&tmv.tv_sec, &tme);
    
    int off = ::strftime(header, 50, "%D %H:%M:%S.", &tme);
    len = off;
    off = ::sprintf(header + len, "%03d <%-8s>",
        (int)tmv.tv_usec / 1000, levelDesc(level));
    len += off;
}

uint64_t LogWriter::fileNode() {	
	uint64_t node = 0;
	struct stat statbuff;	
	if(stat(logFile_.c_str(), &statbuff) == 0) {			
		node = statbuff.st_ino;
	}	
	
	return node;
}

const char* LogWriter::levelDesc(ELogLevel level) {
    const char* levelstr = "UNKNOWN";

    switch (level) {
    case LL_ERROR:
        levelstr = "ERROR";
        break;
    case LL_WARN:
        levelstr = "WARNING";
        break;
    case LL_NOTICE:
        levelstr = "NOTICE";
        break;
    case LL_INFO:
        levelstr = "INFO";
        break;
    case LL_DBG:
        levelstr = "DEBUG";
        break;
    case LL_VARS:
        levelstr = "VARS";
        break;
    case LL_ALL:
        levelstr = "ALL";
        break;
    };

    return levelstr;
}

LogWriter WARN_W;
LogWriter INFO_W;
bool log_init(ELogLevel l, const string& moduleName, const string& logDir) {
    string fullPath = logDir + "/" + moduleName + ".access";
    INFO_W.loginit(l, fullPath);
    fullPath = logDir + "/" + moduleName + ".error";
    WARN_W.loginit(l > LL_WARN ? l : LL_WARN, fullPath);
    return true;
}

bool log_close(){
    WARN_W.logclose();
    INFO_W.logclose();
    return true;
}