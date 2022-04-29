//
// Created by Chenglinli on 2022/4/27.
//

#ifndef TINYDISTRIBUTECACHE_LOGFILE_H
#define TINYDISTRIBUTECACHE_LOGFILE_H

#include <string>
#include <sstream>
#include <list>
#include <queue>
#include <vector>
#include <mutex>
#include <pthread.h>
#include <thread>
#include <unistd.h>


#include <stdarg.h>

#include <iostream>


// 日志信息等级
enum LOG_LEVEL{
    VERBOSE = 0,
    TRACE = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    FATAL = 5
};

#define LOGVERBOSE(...) writeLog(VERBOSE,__FILE_NAME__,__func__,__LINE__,##__VA_ARGS__)
#define LOGTRACE(...) writeLog(TRACE,__FILE_NAME__,__func__,__LINE__,__VA_ARGS__)
#define LOGINFO(...) writeLog(INFO,__FILE_NAME__,__func__,__LINE__,__VA_ARGS__)
#define LOGWARNING(...) writeLog(WARNING,__FILE_NAME__,__func__,__LINE__,__VA_ARGS__)
#define LOGERROR(...) writeLog(ERROR,__FILE_NAME__,__func__,__LINE__,__VA_ARGS__)
#define LOGFATAL(...) writeLog(FATAL,__FILE_NAME__,__func__,__LINE__,__VA_ARGS__)
#define TIME_STRING_LEN 32


class LogFile {
public:
    LogFile(LOG_LEVEL log_level = INFO, int _sizeLimit = 10000):
    size(0), fileNum(0), logFilePtr(nullptr), level(log_level),
    sizeLimit(_sizeLimit), fileName("my.log"), writeToFileThread(writeToFile, this){};

    virtual ~LogFile();

    void openLogFile(std::string& fileName);

    // 将log信息添加到消息队列中
    void writeLog(LOG_LEVEL log_level, std::string file_name, std::string func, int line, const char* fmt, ...);

    /**
     * 阻塞线程
     */
    void join(){
        writeToFileThread.join();
    }

private:
    static void getTime(char* timeStr);

    // 消费者线程方法实际调用方法
    // 实现功能是不断从消息队列中获取数据并将其写入到磁盘文件中
    static void* writeToFile(void* arg);

    // 日志的LOG信息队列
    std::queue<std::string, std::list<std::string>> cached_log;

    // 使用一个消费者线程，完成写入文件的操作
    std::thread writeToFileThread;
    // 对消息队列的互斥请求和同步机制实现所需要的变量
    std::mutex mtx;
    std::condition_variable cv;

    FILE* logFilePtr;
    int level;  // 用于指定可以添加的日志的level起始等级
    int size;
    int fileNum;
    int sizeLimit;  // 用于限制单个日志文件的大小（这个限制是指达到或者超过该值后，便创建新的文件）
    std::string fileName;
};








#endif //TINYDISTRIBUTECACHE_LOGFILE_H
