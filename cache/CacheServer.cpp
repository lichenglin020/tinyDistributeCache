//
// Created by Chenglinli on 2022/5/2.
//

#include "CacheData.h"
#include <sstream>

std::string master_addr = "127.0.0.1:7000";
const std::string curen_addr = CACHESERV_IP+(std::string)":"+std::to_string(CACHESERV_PORT);

LogFile logFile; // 全局日志

// 测试使用的一些参数
std::atomic<long long> request_count = 0;
std::atomic<long long> rcv_cli = 0;
std::atomic<long long> rcv_bk = 0;

// 读写记录所需要使用到的锁
// Client对Cache Server服务器进行写数据时，需要加写锁；进行读数据时，需要加读锁。
pthread_mutex_t transfertooth = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reflesh_master_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t shutdown_lock = PTHREAD_RWLOCK_INITIALIZER;


int main(){
    // 创建log日志，并启动
    std::string filename = "../log/CacheServer.log";
    logFile.openLogFile(filename);
    logFile.run();

    // cache节点需要使用一致性哈希是因为其需要根据key值找到自己的备份节点
    auto key_addr = std::make_shared<ConsistentHash>(100);
    key_addr -> init({curen_addr});
    auto ipport_list = std::make_shared<std::vector<std::string>>();
    auto LC = std::make_shared<LRUCache>(100);
    auto LC_BK = std::make_shared<LRUCache>(100);
    auto ThrPl = std::make_shared<ThreadPool>(10,20);

    CacheData cacheData(ThrPl, LC, LC_BK, key_addr);
    logFile.LOGINFO("run the cache server.");
    logFile.LOGINFO(curen_addr.c_str());
    COUT("服务启动");
    cacheData.run();

    return 0;
}