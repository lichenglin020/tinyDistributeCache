//
// Created by Chenglinli on 2022/5/2.
//

#ifndef TINYDISTRIBUTECACHE_CACHEDATA_H
#define TINYDISTRIBUTECACHE_CACHEDATA_H
#include "ConnectTask.h"
#include "PutGetTask.h"

/**
 * CacheData主要承接整个缓存服务器的数据缓存能力，
 * 涉及LRU数据缓存设定，数据的读写，线程池等
 */

class CacheData {
public:
    CacheData(std::shared_ptr<ThreadPool> threadPool,
              std::shared_ptr<LRUCache> lruCache,
              std::shared_ptr<LRUCache> lruCacheBackup,
              std::shared_ptr<ConsistentHash> keyAddrs);

    virtual ~CacheData();
    void listen();              // 创建绑定并监听套接字，并加入到epoll内核红黑树中
    int wait(int microsecond);  // 监听内核epoll响应

    void acceptHandler();       // 当epoll触发事件是accept客户端连接时的处理逻辑
    void readAndWriteHandler(int index); // 当epoll触发事件是读写事件时的处理逻辑

    bool isListenedFd(int index);


private:
//    sockaddr_in clientSocketAddr; // 客户端连接时所创建的地址信息
    sockaddr_in serverSocketAddr; // 服务端的相关的地址配置信息
    int listenedServerSocket;   // 需要监听的套接字
    int listenNum;  // listen时，请求队列的长度设置
    int epollFd;    // epoll的文件描述符
    int epollFdSize; // epoll_create所使用的size参数大小
    epoll_event epollEvent; // epoll事件
    int readyEventsNum;
    epoll_event *readyEvents;
    char buffer[BUFSIZ];

    ServerRWStruct serverRwStruct;             // epoll监听，触发读写事件时需要使用的数据
    ServerAcceptStruct serverAcceptStruct;     // epoll监听，触发连接事件时需要使用的数据

    std::shared_ptr<ThreadPool> cacheDataThreadPool; // 缓存数据模块所需要使用到的线程池


};


#endif //TINYDISTRIBUTECACHE_CACHEDATA_H
