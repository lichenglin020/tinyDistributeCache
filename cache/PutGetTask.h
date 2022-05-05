//
// Created by Chenglinli on 2022/5/2.
//

#ifndef TINYDISTRIBUTECACHE_PUTGETTASK_H
#define TINYDISTRIBUTECACHE_PUTGETTASK_H
#include "CacheServer.h"

/**
 * 当前节点作为服务端，epoll监听读写操作,所需要用到的配置信息。
 * 作为服务端，需要响应客户端或者来自其他服务端的请求。
 */
struct ServerRWStruct{
    int clientSocket;
    int epollFd;
    char* buffer;
    std::shared_ptr<LRUCache> lruCache;
    std::shared_ptr<LRUCache> lruCacheBackup;
    std::shared_ptr<ConsistentHash> keyAddrs;

    ~ServerRWStruct(){
        delete[] buffer;
    }
};

/**
 * 读取或者写入数据时的具体任务处理类
 */
class PutGetTask {
public:
    PutGetTask(ServerRWStruct* serverRWStruct);
    virtual ~PutGetTask();

    ssize_t readFromClient();
    void closeConnect();
    int getInfoType();
    //
    void kvReadHandler();
    void kvWriteHandler();
    std::string refleshMaster();

private:
    std::shared_ptr<LRUCache> lruCache;
    std::shared_ptr<LRUCache> lruCacheBackon;
    std::shared_ptr<ConsistentHash> keyAddrs;
    int clientSocketFd;
    char buffer[BUFSIZ]; //stdio.h 8192
    int epollFd;
    json info;
};


#endif //TINYDISTRIBUTECACHE_PUTGETTASK_H
