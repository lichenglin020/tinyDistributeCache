//
// Created by Chenglinli on 2022/5/1.
//

#ifndef TINYDISTRIBUTECACHE_EPOLLWRAP_H
#define TINYDISTRIBUTECACHE_EPOLLWRAP_H
#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>
#include <string>

/**
 * epoll 的包装头文件
 * 在该文件中对epoll的操作添加必要的信息，并进行封装
 * 并不包含所有方法，仅对部分方法进行封装
 */

/**
 * 将error信息输出，并终止程序运行
 * @param str 等同于perror参数
 */
void perrorAndExit(const char* str){
    perror(str);
    exit(EXIT_FAILURE);
}

/**
 * 创建套接字
 * @param domain 地址类型
 * @param type socket类型
 * @param protocol 所使用的协议
 * @return
 */
int createSocket(int domain, int type, int protocol){
    int n = socket(domain, type, protocol);
    if(n < 0) perrorAndExit("create socket error: ");
    return n;
}

/**
 * 绑定socket到指定ip及端口
 * @param fd 绑定的套接字
 * @param _sockaddr
 * @param socklen
 * @return
 */
int bindSocket(int fd, const struct sockaddr* _sockaddr, socklen_t socklen){
    int n = bind(fd, _sockaddr, socklen);
    if(n < 0) perrorAndExit("bind error: ");
    return n;
}

/**
 * 进入监听套接字状态，等待用户发起请求
 * @param fd 被监听的套接字
 * @param size 请求队列长度
 * @return
 */
int listenSocket(int fd, int size){
    int n = listen(fd, size);
    if(n < 0) perrorAndExit("listen error:");
    return n;
}

/**
 * 通过accept接收来自客户端的请求
 * @param fd
 * @param _sockaddr
 * @param socklen
 * @return
 */
int acceptSocket(int fd, sockaddr* _sockaddr, socklen_t* socklen){
    int clnt_sock;
    //接收客户端请求
//    sockaddr_in clnt_addr;
//    socklen_t clnt_addr_size = sizeof(clnt_addr);
    do{
        clnt_sock = accept(fd, _sockaddr, socklen);
        if(clnt_sock > 0) return clnt_sock;
        // 出现错误时，如果是因为信号终端或者软件引起的连接终止需要重启accept连接。
    }while(errno == EINTR || errno == ECONNABORTED);
    perrorAndExit("accept error: ");
    return -1;
}

/**
 * 连接socket
 * @param fd 服务端socket
 * @param _sockaddr
 * @param socklen
 * @return
 */
int connectSocket(int fd, const struct sockaddr* _sockaddr, socklen_t socklen){
    int n = connect(fd, _sockaddr, socklen);
    if(n<0) perror("connect error: ");
    return n;
}

/**
 * 读取数据操作
 * @param clientFd 客户端连接的socket
 * @param buffer 接受数据的buffer
 * @param size buffer大小
 * @return
 */
ssize_t readInfo(int clientFd, char* buffer, int size){
    ssize_t n;
    do{
        n = read(clientFd, buffer, size);
        if(n >= 0) return n;
    }while(errno == EINTR || errno == EAGAIN);
    perror("read: ");
    return n;
}

/**
 * 循环的读取所有数据
 * @param clientFd
 * @param buffer
 * @param size
 * @return
 */
ssize_t readAllInfo(int clientFd, char* buffer, int size){
    std::string result = "";
    bool isEnd = false;
    ssize_t resultn;
    int n = 0;
    do{
        n = read(clientFd, buffer, size);
        if(n > 0 && !isEnd){
            for(int i = 0; i < n; i++){
                if(buffer[i] == '\0'){
                    isEnd = true;
                    break;
                }
                if(!isEnd){
                    result += buffer[i];
                    resultn++;
                }
            }
        }
    } while (n > 0 || errno == EINTR);
    if(resultn == 0) return n;
    for(int i = 0; i < result.size(); i++){
        buffer[i] = result[i];
    }
    buffer[result.size()] = '\0';
    return resultn;
}

/**
 * 写数据操作
 * @param clientFd 客户端连接的socket
 * @param buffer 发送的数据
 * @param size 数据大小
 * @return
 */
ssize_t writeInfo(int clientFd, char* buffer, int size){
    ssize_t n;
    do{
        n = write(clientFd, buffer, size);
        if(n >= 0) return n;
    }while(errno == EINTR || errno == EAGAIN);
    perror("write: ");
    return n;
}

/**
 * 创建epoll内核事件表的文件描述符
 * @param size
 * @return
 */
int epollCreate(int size){
    int epoll_fd = epoll_create(size);
    if(epoll_fd < 0) perrorAndExit("epoll create error: ");
    return epoll_fd;
}

/**
 * 操作内核事件表监控的文件描述符上的事件，添加监听事件
 * @param epoll_fd epoll内核事件表的文件描述符
 * @param op 操作
 * @param fd socket文件描述符
 * @param event epoll事件
 * @return
 */
int epollCtl(int epoll_fd, int op, int fd, struct epoll_event *event){
    int n = epoll_ctl(epoll_fd, op, fd, event);
    if(n < 0) perror("epoll control: ");
    return n;
}

/**
 * 等待所监控文件描述符上事件的产生，返回就绪的文件描述符个数
 * @param epoll_fd
 * @param events 用来存内核得到事件的集合
 * @param maxevents events的大小
 * @param timeout 超时时间
 * @return
 */
int epollWait(int epoll_fd, struct epoll_event *events, int maxevents, int timeout){
    int n = epoll_wait(epoll_fd, events, maxevents, timeout);
    if(n < 0) perrorAndExit("epoll_wait error: ");
//    if(n == 0) printf("time out \n");
    return n;
}

/**
 * 使用select实现的线程安全的毫秒级定时器
 * @param n ms的时停
 */
void selectSleep(int n){
    timeval timev;
    timev.tv_sec = 0;
    timev.tv_usec = n * 1000;
    select(0, nullptr, nullptr, nullptr, &timev);
}













#endif //TINYDISTRIBUTECACHE_EPOLLWRAP_H
