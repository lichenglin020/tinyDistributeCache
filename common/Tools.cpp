//
// Created by Chenglinli on 2022/5/7.
//

#include "Tools.h"


static std::mutex cout_mtx;

/**
 * 线程安全的输出内容到控制台
 * @param filePath
 * @param funcName
 * @param line
 * @param c
 */
void safeThreadCout(std::string filePath, std::string funcName, int line, const char* c){
    std::lock_guard<std::mutex> lck(cout_mtx);
    std::string fileName = filePath.substr(filePath.find_last_of('/') + 1);
    std::cout << "[" << getTimeCrude() << "] "\
    << "线程" << std::this_thread::get_id() \
    << ":[" << fileName << "/" << funcName << "()/" << std::to_string(line) << "]: " \
    << c << std::endl;
}

/**
 * 获取当前时间的分与秒(线程安全)
 * @return
 */
std::string getTimeCrude(){
    time_t curtime = time(0);
    tm* nowtime = localtime(&curtime);
    std::string result = std::to_string(nowtime ->tm_hour) + ":" +
            std::to_string(nowtime -> tm_min) + ":" +
            std::to_string(nowtime -> tm_sec);
    return result;
}

/**
 * 从string中解读ip地址及端口信息
 * @param addr ip:port字符串
 * @return ipAndPort
 */
ipAndPort getIpAndPort(std::string addr){
    ipAndPort result;
    std::stringstream ss(addr);
    getline(ss, result.ip, ':');
    std::string port;
    getline(ss, port, ':');
    result.ip = std::stoi(port);
    return result;
}