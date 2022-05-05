//
// Created by Chenglinli on 2022/5/2.
//

#include "ConnectTask.h"

ConnectTask::ConnectTask(ServerAcceptStruct *serverAcceptStruct) {
    clientAddr = *(serverAcceptStruct -> clientAddr);
    listendFd = serverAcceptStruct -> listendFd;
    number = serverAcceptStruct -> number;
    epollFd = serverAcceptStruct -> epollFd;
}

ConnectTask::~ConnectTask() {

}

/**
 * accept监听连接请求, 并将客户端连接的套接字监听事件添加到epoll内核事件监听表中
 */
void ConnectTask::accept() {
    socklen_t clientLen = sizeof(clientAddr);
    connectFd = acceptSocket(listendFd, (sockaddr*)&clientAddr, &clientLen);

    std::cout << "accept from" << \
    inet_ntop(AF_INET, &clientAddr.sin_addr, address, sizeof(address)) << \
    " at port " << ntohs(clientAddr.sin_port) << std::endl;

    // 修改事件属性为非阻塞
    setNoblock(connectFd);
    // 添加
    epollEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
    epollEvent.data.fd = connectFd;
    epollCtl(epollFd, EPOLL_CTL_ADD, connectFd, &epollEvent);
}

