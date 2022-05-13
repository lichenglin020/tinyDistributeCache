//
// Created by Chenglinli on 2022/5/2.
//

#ifndef TINYDISTRIBUTECACHE_CACHESERVER_H
#define TINYDISTRIBUTECACHE_CACHESERVER_H
#include <memory>
#include <fcntl.h>
#include <atomic>

#include "../common/EpollWrap.h"
#include "LRU.h"
#include "../common/ConsistentHash.h"
#include "../common/LogFile.h"
#include "../common/threadpool/ThreadPool.h"
#include "../common/Tools.h"
#include "HttpJson.h"

#define REFLESH_MASTER 9

// 本机配置信息
#define CACHESERV_IP "127.0.0.1"
#define CACHESERV_PORT 8003


extern std::string master_addr;
extern const std::string curen_addr;

extern LogFile logFile; // 全局日志

// 测试使用的一些参数
extern std::atomic<long long> request_count;
extern std::atomic<long long> rcv_cli;
extern std::atomic<long long> rcv_bk;


extern pthread_mutex_t transfertooth;
extern pthread_mutex_t reflesh_master_lock;
extern pthread_rwlock_t shutdown_lock;




#endif //TINYDISTRIBUTECACHE_CACHESERVER_H
