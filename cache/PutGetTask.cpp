//
// Created by Chenglinli on 2022/5/2.
//

#include "PutGetTask.h"

PutGetTask::PutGetTask() {

}

PutGetTask::~PutGetTask() {

}

/**
 * 从输入缓冲区中获取数据
 * @return
 */
ssize_t PutGetTask::readFromClient() {
    ssize_t n = readInfo(clientSocketFd, buffer, BUFSIZ);
    return n;
}

/**
 * 关闭连接
 */
void PutGetTask::closeConnect() {
    epollCtl(epollFd, EPOLL_CTL_DEL, clientSocketFd, nullptr);
    close(clientSocketFd);
    // 输出关闭连接日志信息
    std::string loginfo = "client[" + std::to_string(clientSocketFd) + "] close connection.";
    logFile.LOGINFO(loginfo.c_str());
    std::string tmp = "断开连接，清理epoll，删除套接字：" + std::to_string(clientSocketFd);
    COUT(tmp.c_str());
}

/**
 * 解析json并从中获取数据的类型
 * @return
 */
int PutGetTask::getInfoType() {
    if(HttpJson::getRequestJson(info, buffer)){
        return (int)info["type"];
    }else{
        return -1;
    }
}

/**
 * 客户端读取数据时的处理方法
 * 尚未完成：读取数据的时候也需要同步副本
 */
void PutGetTask::kvReadHandler() {
    std::string value = lruCache -> get(info["data"]["key"], false);
    json respondClient = HttpJson::respondClientJson(info["data"]["key"], value);
    std::string str = respondClient.dump() + "\0";
    std::string tmp = "收到了读取数据的请求,回传给客户端的数据：" + str;
    COUT(tmp.c_str());

    // 将数据传回给客户端
    writeInfo(clientSocketFd, (char*)str.c_str(), str.size() + 1);
}

/**
 * 客户端写入数据时的处理方法
 * 如果收到的信息是KEY_VALUE_RESPONDBK，则表明是Primary节点发送而来的同步数据，，当前节点为副本，仅需完成副本数据的写入
 * 如果是KEY_VALUE_RESPOND，则表明是client请求的Primary节点，Primary节点需要完成数据的更新，并且需要将相关信息通知给副本节点
 */
void PutGetTask::kvWriteHandler() {
    // 测试: 使用logfile记录一些数据
    std::string tmp = "收到的写入数据的请求：" + info.dump();
    COUT(tmp.c_str());
    std::string loginfo_count = "the count of request is: "+std::to_string(++request_count);
    logFile.LOGINFO((char *)loginfo_count.data());

    // 如果收到的信息是KEY_VALUE_RESPONDBK，则表明是Primary节点发送而来的同步数据
    if(info["type"] == KEY_VALUE_WRITE_BK){
        logFile.LOGINFO("receive KEY_VALUE_RESPONDBK from other cache: ");
        logFile.LOGINFO((char *)info.dump().data());
        tmp = "从服务端收到的备份数据总数：" + std::to_string(++rcv_bk);
        COUT(tmp.c_str());
        lruCacheBackon -> put(info["data"]["key"], info["data"]["value"]);
        return;
    }

    // 如果是KEY_VALUE_RESPOND，则表明是client请求的Primary节点
    // Primary节点需要完成数据的更新，并且需要将相关信息通知给副本节点
    logFile.LOGINFO("receive KEY_VALUE_RESPOND from client");
    logFile.LOGINFO((char *)info.dump().data());
    tmp = "从客户端收到的数据总数：" + std::to_string(++rcv_cli);
    COUT(tmp.c_str());
    lruCache -> put(info["data"]["key"], info["data"]["value"]);

    // 获取副本节点地址信息
    std::string backUpServeraddr = keyAddrs -> getNextServerIndex(curen_addr);
    // 将数据传送给副本节点
    if(!backUpServeraddr.empty() && backUpServeraddr != curen_addr){
        logFile.LOGINFO("send info to backup.");
        auto ip_and_port = getIpAndPort(backUpServeraddr);

        int backUpSocket = createSocket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in backUpSocketAddr;
        memset(&backUpSocketAddr, 0, sizeof(backUpSocketAddr));
        backUpSocketAddr.sin_family = AF_INET;
        backUpSocketAddr.sin_addr.s_addr = inet_addr(ip_and_port.ip.c_str());
        backUpSocketAddr.sin_port = htons(ip_and_port.port);

        int n = connectSocket(backUpSocket, (sockaddr*)&backUpSocketAddr, sizeof(backUpSocketAddr));
        if(n >= 0){
            setNoblock(backUpSocket);
            json backupJson = HttpJson::writeBackupJson(
                    (const std::string)info["data"]["key"],
                    (const std::string)info["data"]["value"]);
            std::string backupStr = backupJson.dump() + "\0";
            writeInfo(backUpSocket, (char *)backupStr.data(), backupStr.length() + 1);
        }

        close(backUpSocket);
    }
}

/**
 * 刷新IP地址信息
 * @return
 */
std::string PutGetTask::refleshMaster() {
    return (std::string)info["data"]["iplist"];
}


const std::shared_ptr<LRUCache> &PutGetTask::getLruCache() const {
    return lruCache;
}

void PutGetTask::setLruCache(const std::shared_ptr<LRUCache> &lruCache) {
    PutGetTask::lruCache = lruCache;
}

const std::shared_ptr<LRUCache> &PutGetTask::getLruCacheBackon() const {
    return lruCacheBackon;
}

void PutGetTask::setLruCacheBackon(const std::shared_ptr<LRUCache> &lruCacheBackon) {
    PutGetTask::lruCacheBackon = lruCacheBackon;
}

const std::shared_ptr<ConsistentHash> &PutGetTask::getKeyAddrs() const {
    return keyAddrs;
}

void PutGetTask::setKeyAddrs(const std::shared_ptr<ConsistentHash> &keyAddrs) {
    PutGetTask::keyAddrs = keyAddrs;
}

int PutGetTask::getClientSocketFd() const {
    return clientSocketFd;
}

void PutGetTask::setClientSocketFd(int clientSocketFd) {
    PutGetTask::clientSocketFd = clientSocketFd;
}

int PutGetTask::getEpollFd() const {
    return epollFd;
}

void PutGetTask::setEpollFd(int epollFd) {
    PutGetTask::epollFd = epollFd;
}




