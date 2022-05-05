//
// Created by Chenglinli on 2022/5/2.
//

#ifndef TINYDISTRIBUTECACHE_CONNECTTASK_H
#define TINYDISTRIBUTECACHE_CONNECTTASK_H

#include "CacheServer.h"

/**
 * 作为服务端，epoll监听连接操作时的配置信息。
 */
struct ServerAcceptStruct{
    sockaddr_in *clientAddr;
    int listendFd;
    int number;
    int epollFd;

    ~ServerAcceptStruct(){
        delete clientAddr;
    }
};

/**
 * 提供给线程池的Task
 * 该类型Task主要指代连接任务
 */
class ConnectTask {
public:
    ConnectTask(ServerAcceptStruct * serverAcceptStruct);
    virtual ~ConnectTask();
    void accept();

private:
    sockaddr_in clientAddr;
    int listendFd;
    int number;
    int epollFd;
    epoll_event epollEvent;
    char address[INET_ADDRSTRLEN]; // 16个字节, IPv4地址信息
    int connectFd;
};


#endif //TINYDISTRIBUTECACHE_CONNECTTASK_H
