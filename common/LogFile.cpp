//
// Created by Chenglinli on 2022/4/27.
//

#include "LogFile.h"

LogFile::~LogFile() {
    if(logFilePtr != nullptr) fclose(logFilePtr);
    logFilePtr = nullptr;
}

void LogFile::openLogFile(std::string& fileName) {
    std::unique_lock<std::mutex> lck(mtx);
    this -> fileName = fileName;
    if(logFilePtr != nullptr) fclose(logFilePtr);
    // 创建初始的log文件
    fileName.insert(fileName.find_last_of('/') + 1, "0_");
    logFilePtr = fopen(fileName.c_str(), "a+");
    fileNum = 1;
}

/**
 * 向log文件中写入数据的线程调用函数
 * @param arg LogFile*
 * @return nullptr
 */
void* LogFile::writeToFile(void *arg){
    LogFile* logFile = static_cast<LogFile*>(arg);
    std::string line = "";
    while(true){
        std::unique_lock<std::mutex> lck(logFile -> mtx);
        logFile -> cv.wait(lck, [&]() -> bool {
            return !(logFile -> cached_log.empty());
        });

        line = logFile -> cached_log.front();
        logFile -> cached_log.pop();

        if(line.empty()){
            lck.unlock();
            continue;
        }

        // 完成数据写入磁盘操作
        if(logFile -> logFilePtr == nullptr) continue;
        // 向输出流logFilePtr中写入信息，并判断是否写入成功（C库的fwrite是线程安全函数）
        if(fwrite((void*)line.c_str(), 1, line.length(), logFile -> logFilePtr) != line.length()) continue;
        // 刷新输出流(必须要刷新，否则输出可能为空)
        fflush(logFile -> logFilePtr);
        // 如果文件的数据量已经大于了文件限制的最大长度，则创建新的文件
        if(logFile -> size + line.size() >= logFile -> sizeLimit){
            std::string newFileName = logFile -> fileName;
            newFileName.insert(newFileName.find_last_of('/') + 1, std::to_string(logFile -> fileNum) + "_");
            logFile -> logFilePtr = fopen(newFileName.c_str(), "a+");
            logFile -> fileNum++;
            logFile -> size = 0;
        }else{
            logFile -> size += line.length();
        }
        line.clear();
    }
    return nullptr;
}

/**
 * 获取当前时间
 * @param timeStr
 */
void LogFile::getTime(char* timeStr)
{
    time_t rawtime;
    struct tm *ptminfo;
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    snprintf(timeStr,64,"[%04d-%02d-%02d %02d:%02d:%02d]",
             ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
             ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
}

/**
 * 将信息添加到日志队列的函数
 * @param log_level 日志等级
 * @param file_name 调用该函数的文件的名称
 * @param func_name 调用该函数的函数的名称
 * @param code_line 调用点在所在文件的行数
 * @param fmt 格式化的不定参数，用于vsnprintf
 * @param ...
 */
void LogFile::writeLog(LOG_LEVEL log_level, std::string file_name, std::string func_name, int code_line, const char* fmt, ...) {
    if(log_level < level) return;

    std::ostringstream ss;
    // 时间信息
    char strTime[TIME_STRING_LEN] = {};
    getTime(strTime);
    ss << strTime;

    // level信息
    std::string LogLevelNames[] = {"VERBOSE","TRACE","INFO","WARNING","ERROR","FATAL"};
    ss << "[" << LogLevelNames[log_level] << "]";
    // 线程id
    ss << "[tid:" << std::this_thread::get_id() <<"]";
    // 文件名，文件代码行及所在函数名
//    ss << "[" << file_name << "(" << code_line << "):" << func_name <<"]";
    std::string fname1 = file_name.substr(file_name.find_last_of("//")+1);
    ss << "[" << fname1 << "(" << code_line << "):" << func_name <<"]";

    //content
    // 定义变参列表
    va_list ap;
    // 获取变参首地址，并赋给变参列表
    va_start(ap, fmt);
    int len = vsnprintf(nullptr,0,fmt, ap); // 将可变参数格式化输出到一个字符数组（此处未输出，仅获取变参长度）
    va_end(ap);

    va_start(ap,fmt);
    char* content = new char[len+1];
    vsnprintf(content,len+1,fmt,ap); // 将可变参数格式化输出到content中
    va_end(ap);
    ss << content;
    ss << "\n";
    delete[] content;

    std::unique_lock<std::mutex> lck(mtx);
    cached_log.push(ss.str());
    lck.unlock();
    cv.notify_all();
}

/**
 * 创建将队列中的消息写入Log日志文件的线程，并执行
 */
void LogFile::run() {
    writeToFileThread = std::thread(writeToFile, this);
}

/**
 * 阻塞调用该方法的线程，等待写入Log日志文件的线程结束
 */
void LogFile::join() {
    writeToFileThread.join();
}
