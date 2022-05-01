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

/**
 * epoll 的包装头文件
 * 在该文件中对epoll的操作添加必要的信息，并进行封装
 */

/**
 * 将error信息输出，并终止程序运行
 * @param str 等同于perror参数
 */
void perrorAndExit(const char* str){
    perror(str);
    exit(EXIT_FAILURE);
}








#endif //TINYDISTRIBUTECACHE_EPOLLWRAP_H
