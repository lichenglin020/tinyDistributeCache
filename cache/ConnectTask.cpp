//
// Created by Chenglinli on 2022/5/2.
//

#include "ConnectTask.h"

//ConnectTask::ConnectTask(ServerAcceptStruct *serverAcceptStruct) {
//    clientAddr = *(serverAcceptStruct -> clientAddr);
//    listendFd = serverAcceptStruct -> listendFd;
//    number = serverAcceptStruct -> number;
//    epollFd = serverAcceptStruct -> epollFd;
//}

ConnectTask::ConnectTask() {

}

ConnectTask::~ConnectTask() {

}

/**
 * accept监听连接请求, 并将客户端连接的套接字监听事件添加到epoll内核事件监听表中
 */
void ConnectTask::accept() {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int connectFd = acceptSocket(listendFd, (sockaddr*)&clientAddr, &clientLen);

    // 修改事件属性为非阻塞
    setNoblock(connectFd);
    // 添加
    epoll_event epollEvent;
    epollEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
    epollEvent.data.fd = connectFd;
    epollCtl(epollFd, EPOLL_CTL_ADD, connectFd, &epollEvent);

    char address[INET_ADDRSTRLEN];
    std::string tmp = "$$ 接收到新连接 $$：" +
                      std::string(inet_ntop(AF_INET, &clientAddr.sin_addr, address, sizeof(address))) +
                      ":" + std::to_string(ntohs(clientAddr.sin_port));
    COUT(tmp.c_str());
    tmp = "创建套接字并将其加入epoll红黑树：" + std::to_string(connectFd);
    COUT(tmp.c_str());
}

int ConnectTask::getListendFd() const {
    return listendFd;
}

void ConnectTask::setListendFd(int listendFd) {
    ConnectTask::listendFd = listendFd;
}

int ConnectTask::getEpollFd() const {
    return epollFd;
}

void ConnectTask::setEpollFd(int epollFd) {
    ConnectTask::epollFd = epollFd;
}

