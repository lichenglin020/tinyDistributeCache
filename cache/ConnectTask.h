//
// Created by Chenglinli on 2022/5/2.
//

#ifndef TINYDISTRIBUTECACHE_CONNECTTASK_H
#define TINYDISTRIBUTECACHE_CONNECTTASK_H

#include "CacheServer.h"

/**
 * 提供给线程池的Task
 * 该类型Task主要指代连接任务
 */
class ConnectTask {
public:
//    ConnectTask(ServerAcceptStruct * serverAcceptStruct);
    ConnectTask();

    virtual ~ConnectTask();

    void accept();

    void setListendFd(int listendFd);

    void setEpollFd(int epollFd);

private:
    int listendFd;
    int epollFd;    // 16个字节, IPv4地址信息
};


#endif //TINYDISTRIBUTECACHE_CONNECTTASK_H
