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

    const std::shared_ptr<LRUCache> &getLruCache() const;

    void setLruCache(const std::shared_ptr<LRUCache> &lruCache);

    const std::shared_ptr<LRUCache> &getLruCacheBackon() const;

    void setLruCacheBackon(const std::shared_ptr<LRUCache> &lruCacheBackon);

    const std::shared_ptr<ConsistentHash> &getKeyAddrs() const;

    void setKeyAddrs(const std::shared_ptr<ConsistentHash> &keyAddrs);

    int getClientSocketFd() const;

    void setClientSocketFd(int clientSocketFd);

    const char *getBuffer() const;

    int getEpollFd() const;

    void setEpollFd(int epollFd);

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
