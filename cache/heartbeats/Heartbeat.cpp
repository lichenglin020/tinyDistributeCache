//
// Created by Chenglinli on 2022/5/9.
//

#include "Heartbeat.h"

Heartbeat::Heartbeat(std::shared_ptr<ConsistentHash> _keyAddr,
                     std::shared_ptr<ThreadPool> _threadPool,
                     std::shared_ptr<LRUCache> _lruCache,
                     std::shared_ptr<LRUCache> _lruCacheBackUp):
                     keyAddr(_keyAddr),
                     threadPool(_threadPool),
                     lruCache(_lruCache),
                     lruCacheBackUp(_lruCacheBackUp),
                     connectFd(-1){
    heartbeatJsonStr = HttpJson::heartbeatJson(curen_addr).dump() + '\0';
}

Heartbeat::~Heartbeat() {
    close(connectFd);
    logFile.LOGINFO("close and release the heartbeats module.");
}

/**
 * 连接master节点
 */
void Heartbeat::connect() {
    // 如果已存在使用的套接字，则先进行关闭
    if(connectFd != -1) close(connectFd);
    connectFd = createSocket(AF_INET, SOCK_STREAM, 0);
    int isConnect = -1;
    while(isConnect < 0){

        pthread_mutex_lock(&reflesh_master_lock);
        auto ip_and_port = getIpAndPort(master_addr);
        pthread_mutex_unlock(&reflesh_master_lock);

        sockaddr_in socketAddr;
        memset(&socketAddr, 0, sizeof(socketAddr));
        socketAddr.sin_family = AF_INET;
        socketAddr.sin_addr.s_addr = inet_addr(ip_and_port.ip.c_str());
        socketAddr.sin_port = htons(ip_and_port.port);

        isConnect = connectSocket(connectFd, (sockaddr *) &socketAddr, sizeof(socketAddr));
    }
    setNoblock(connectFd);
}

/**
 * 发送心跳包给Master节点
 */
void Heartbeat::send() {
    // 尝试发送心跳包。
    if(writeInfo(connectFd, heartbeatJsonStr.data(), heartbeatJsonStr.size() + 1) < 0){
        // 如果写入失败则尝试重新连接。
        connect();
    }
}

/**
 * 从Master接受数据，并判断类型
 * @return 接受成功则返回类型，否则返回-1
 */
int Heartbeat::recvCommond() {
    int n = readAllInfo(connectFd, commondBuffer, BUFSIZ);
    if(n > 0){
        if(HttpJson::getRequestJson(commondJson, commondBuffer)){
            return (int)commondJson["type"];
        }else{
            std::string tmp = "心跳包Commond格式有错: " + std::string(commondBuffer);
            COUT(tmp.c_str());
        }
    }else if(n == 0){
        // 收到了finish报文则尝试重新连接
        connect();
        COUT("心跳包连接中断");
    }
    return -1;
}

/**
 * 从Master收到关闭当前cache节点的命令时的处理方法
 */
void Heartbeat::closeCmdHandler() {
    COUT("收到关闭服务指令");
}

/**
 * 从Master收到扩容指令时的处理方法（有新增cache节点）
 */
void Heartbeat::expansionCmdHandler() {
    COUT("收到扩容指令");
}

/**
 * 从Master收到缩容指令时的处理方法（有cache节点被删除）
 */
void Heartbeat::reductionCmdHandler() {
    COUT("收到缩容指令");
}



