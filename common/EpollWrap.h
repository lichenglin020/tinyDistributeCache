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
#include<fcntl.h>

/**
 * epoll 的包装头文件
 * 在该文件中对epoll的操作添加必要的信息，并进行封装
 * 并不包含所有方法，仅对部分方法进行封装
 */

/**
 * 将error信息输出，并终止程序运行
 * @param str 等同于perror参数
 */
extern void perrorAndExit(const char* str);

/**
 * 创建套接字
 * @param domain 地址类型
 * @param type socket类型
 * @param protocol 所使用的协议
 * @return
 */
extern int createSocket(int domain, int type, int protocol);

/**
 * 绑定socket到指定ip及端口
 * @param fd 绑定的套接字
 * @param _sockaddr
 * @param socklen
 * @return
 */
extern int bindSocket(int fd, const struct sockaddr* _sockaddr, socklen_t socklen);

/**
 * 进入监听套接字状态，等待用户发起请求
 * @param fd 被监听的套接字
 * @param size 请求队列长度
 * @return
 */
extern int listenSocket(int fd, int size);

/**
 * 通过accept接收来自客户端的请求
 * @param fd
 * @param _sockaddr
 * @param socklen
 * @return
 */
extern int acceptSocket(int fd, sockaddr* _sockaddr, socklen_t* socklen);

/**
 * 连接socket
 * @param fd 服务端socket
 * @param _sockaddr
 * @param socklen
 * @return
 */
extern int connectSocket(int fd, const struct sockaddr* _sockaddr, socklen_t socklen);

/**
 * 读取数据操作
 * @param clientFd 客户端连接的socket
 * @param buffer 接受数据的buffer
 * @param size buffer大小
 * @return
 */
extern ssize_t readInfo(int clientFd, char* buffer, int size);

/**
 * 循环的读取所有数据
 * @param clientFd
 * @param buffer
 * @param size
 * @return
 */
extern ssize_t readAllInfo(int clientFd, char* buffer, int size);

/**
 * 写数据操作
 * @param clientFd 客户端连接的socket
 * @param buffer 发送的数据
 * @param size 数据大小
 * @return
 */
extern ssize_t writeInfo(int clientFd, char* buffer, int size);

/**
 * 创建epoll内核事件表的文件描述符
 * @param size
 * @return
 */
extern int epollCreate(int size);

/**
 * 操作内核事件表监控的文件描述符上的事件，添加监听事件
 * @param epoll_fd epoll内核事件表的文件描述符
 * @param op 操作
 * @param fd socket文件描述符
 * @param event epoll事件
 * @return
 */
extern int epollCtl(int epoll_fd, int op, int fd, struct epoll_event *event);

/**
 * 等待所监控文件描述符上事件的产生，返回就绪的文件描述符个数
 * @param epoll_fd
 * @param events 用来存内核得到事件的集合
 * @param maxevents events的大小
 * @param timeout 超时时间
 * @return
 */
extern int epollWait(int epoll_fd, struct epoll_event *events, int maxevents, int timeout);

/**
 * 使用select实现的线程安全的毫秒级定时器
 * @param n ms的时停
 */
extern void selectSleep(int n);
/**
 * 设置文件描述符为非阻塞模式
 * @param sock
 * @return
 */
extern int setNoblock(int sock);

/**
 * 设置服务端先断开连接时，可立即复用端口，避免进入到TIME_WAIT状态
 * @param serv_sock
 */
extern void setReuseAddr(int serv_sock);


#endif //TINYDISTRIBUTECACHE_EPOLLWRAP_H
