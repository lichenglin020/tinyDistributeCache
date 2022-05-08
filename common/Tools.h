//
// Created by Chenglinli on 2022/5/7.
//

#ifndef TINYDISTRIBUTECACHE_TOOLS_H
#define TINYDISTRIBUTECACHE_TOOLS_H
#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <ctime>
#include <sstream>

#define COUT(str) safeThreadCout(__FILE__,__func__,__LINE__,str)

extern void safeThreadCout(std::string filePath, std::string funcName, int line, const char* c);
extern std::string getTimeCrude();

// ip地址及端口信息
struct ipAndPort{
    int port;
    std::string ip;
};

extern ipAndPort getIpAndPort(std::string addr);



#endif //TINYDISTRIBUTECACHE_TOOLS_H
