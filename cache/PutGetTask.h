//
// Created by Chenglinli on 2022/5/2.
//

#ifndef TINYDISTRIBUTECACHE_PUTGETTASK_H
#define TINYDISTRIBUTECACHE_PUTGETTASK_H
#include "CacheServer.h"

/**
 * 读取或者写入数据时的具体任务处理类
 */
class PutGetTask {
public:
    PutGetTask();

    virtual ~PutGetTask();

    ssize_t readFromClient();

    void closeConnect();

    int getInfoType();

    void kvReadHandler();

    void kvWriteHandler();

    std::string refleshMaster();

    void setLruCache(const std::shared_ptr<LRUCache> &lruCache);

    void setLruCacheBackon(const std::shared_ptr<LRUCache> &lruCacheBackon);

    void setKeyAddrs(const std::shared_ptr<ConsistentHash> &keyAddrs);

    void setClientSocketFd(int clientSocketFd);

    void setEpollFd(int epollFd);

private:
    std::shared_ptr<LRUCache> lruCache;
    std::shared_ptr<LRUCache> lruCacheBackon;
    std::shared_ptr<ConsistentHash> keyAddrs;
    int epollFd;
    int clientSocketFd;

    char buffer[BUFSIZ]; //stdio.h 8192
    json info;
};


#endif //TINYDISTRIBUTECACHE_PUTGETTASK_H
