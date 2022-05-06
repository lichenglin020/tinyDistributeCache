//
// Created by Chenglinli on 2022/5/2.
//
/**
 * 描述：在该文件中包含cache server的相关配置信息等
 */

//#include "CacheServer.h"
#include "CacheData.h"
#include <sstream>

std::string master_addr = "127.0.0.1:7000";
const std::string curen_addr = CACHESERV_IP+(std::string)":"+std::to_string(CACHESERV_PORT);

LogFile logFile; // 全局日志

// 测试使用的一些参数
long long request_count = 0;
long long rcv_cli = 0;
long long rcv_bk = 0;

// 读写记录所需要使用到的锁
// Client对Cache Server服务器进行写数据时，需要加写锁；进行读数据时，需要加读锁。
pthread_mutex_t transfertooth = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t task_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tasklisten_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rw_lock = PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t reflesh_master_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t shutdown_lock = PTHREAD_RWLOCK_INITIALIZER;



/**
 * 获取写入副本节点的json格式的数据
 * @param key
 * @param value
 * @return
 */
json getWriteBackupJson(const std::string& key, const std::string& value){
    json writeBackupJson, data;
    data["flag"] = true;
    data["key"] = key;
    data["value"] = value;
    writeBackupJson["type"] = KEY_VALUE_RESPONDBK;
    writeBackupJson["data"] = data;
    return writeBackupJson;
}

/**
 * 从string中解读ip地址及端口信息
 * @param addr ip:port字符串
 * @return ipAndPort
 */
ipAndPort getIpAndPort(std::string addr){
    ipAndPort result;
    std::stringstream ss(addr);
    getline(ss, result.ip, ':');
    std::string port;
    getline(ss, port, ':');
    result.ip = std::stoi(port);
    return result;
}


int main(){
    //create log thread
    std::string filename = "../log/CacheServer.log";
    logFile.openLogFile(filename);
    logFile.run();
//    pthread_t tid_log;
//    Pthread_create(&tid_log, nullptr, write_log, &logfile);

    //create heartbeat thread
    auto key_addr = std::make_shared<ConsistentHash>(100);
    key_addr -> init({curen_addr});
    auto ipport_list = std::make_shared<std::vector<std::string>>();
    auto LC = std::make_shared<LRUCache>(100);
    auto LC_BK = std::make_shared<LRUCache>(100);
    auto ThrPl = std::make_shared<ThreadPool>(10,20);
//    pthread_t tid_heartbeat;
//    struct heartbeat_struct heartbeat_arg;
//    heartbeat_arg.ThrPl = ThrPl;
//    heartbeat_arg.LC = LC;
//    heartbeat_arg.LC_BK = LC_BK;
//    heartbeat_arg.key_addr = key_addr;
//    heartbeat_arg.ipport_list = ipport_list;
//    Pthread_create(&tid_heartbeat, nullptr, heart_beat, &heartbeat_arg);

    //receive as server
    CacheData cacheData(ThrPl, LC, LC_BK, key_addr);
    cacheData.listen();
    logFile.LOGINFO("the cache server is running.");
    logFile.LOGINFO(curen_addr.c_str());

    for(;;){
        int nready = cacheData.wait(500);
        if(nready <= 0)
            continue;
        pthread_rwlock_rdlock(&shutdown_lock);
        for(int i = 0; i < nready; i++){
            if(cacheData.isListenedFd(i)){
                logFile.LOGINFO("receive new connect");
                cacheData.acceptHandler();
            }
            else{
                logFile.LOGINFO("receive new read or write request");
                cacheData.readAndWriteHandler(i);
            }

        }
        pthread_rwlock_unlock(&shutdown_lock);
    }
    return 0;
}