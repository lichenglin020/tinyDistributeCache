//
// Created by Chenglinli on 2022/5/9.
//

#ifndef TINYDISTRIBUTECACHE_HEARTBEAT_H
#define TINYDISTRIBUTECACHE_HEARTBEAT_H
#include "../CacheServer.h"


/**
 * 缓存的心跳包模块，维持与cache master的沟通与通讯
 */
class Heartbeat {
public:
    Heartbeat(std::shared_ptr<ConsistentHash> _keyAddr,
              std::shared_ptr<ThreadPool> _threadPool,
              std::shared_ptr<LRUCache> _lruCache,
              std::shared_ptr<LRUCache> _lruCacheBackUp);


    virtual ~Heartbeat();

    void connect();

    void send();

    int recvCommond();

    void closeCmdHandler();

    void expansionCmdHandler();

    void reductionCmdHandler();

private:
    std::shared_ptr<ConsistentHash> keyAddr;
    std::shared_ptr<ThreadPool> threadPool;
    std::shared_ptr<LRUCache> lruCache;
    std::shared_ptr<LRUCache> lruCacheBackUp;

    std::string heartbeatJsonStr;  // 心跳包json字符串
    int connectFd; // 连接到master的套接字
    char commondBuffer[BUFSIZ];
    json commondJson;
};


#endif //TINYDISTRIBUTECACHE_HEARTBEAT_H
