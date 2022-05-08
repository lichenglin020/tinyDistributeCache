//
// Created by Chenglinli on 2022/5/2.
//

#ifndef TINYDISTRIBUTECACHE_CACHESERVER_H
#define TINYDISTRIBUTECACHE_CACHESERVER_H
#include "../common/json.hpp"
#include <memory>
#include <fcntl.h>

#include "../common/EpollWrap.h"
#include "LRU.h"
#include "../common/ConsistentHash.h"
#include "../common/LogFile.h"
#include "../common/threadpool/ThreadPool.h"
#include "../common/Tools.h"

using json = nlohmann::json;

// 宏定义，定义一些可能用得上的参数信息
#define KEY_VALUE_REQUEST 0
#define KEY_VALUE_RESPOND 1
#define KEY_VALUE_RESPONDBK 11  // 表明这是一个主副节点数据同步请求
#define REFLESH_MASTER 9

// 本机配置信息
#define CACHESERV_IP "127.0.0.1"
#define CACHESERV_PORT 8003


extern std::string master_addr;
extern const std::string curen_addr;



extern LogFile logFile; // 全局日志

// 测试使用的一些参数
extern long long request_count;
extern long long rcv_cli;
extern long long rcv_bk;

// 读写记录所需要使用到的锁
// Client对Cache Server服务器进行写数据时，需要加写锁；进行读数据时，需要加读锁。
extern pthread_mutex_t transfertooth;

extern pthread_mutex_t task_lock;
extern pthread_mutex_t tasklisten_lock;
extern pthread_rwlock_t rw_lock;
extern pthread_mutex_t reflesh_master_lock;
extern pthread_rwlock_t shutdown_lock;

/**
 * 获取写入副本节点的json格式的数据
 * @param key
 * @param value
 * @return
 */
extern json getWriteBackupJson(const std::string& key, const std::string& value);



#endif //TINYDISTRIBUTECACHE_CACHESERVER_H
