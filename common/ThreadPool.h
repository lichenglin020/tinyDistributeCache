//
// Created by Chenglinli on 2022/4/26.
//

#ifndef TINYDISTRIBUTECACHE_THREADPOOL_H
#define TINYDISTRIBUTECACHE_THREADPOOL_H
#include "Task.h"
#include <thread>
#include <vector>
#include <pthread.h>


class ThreadPool {
public:
    // 设置线程允许运行的线程的数量的最大值及最小值
    ThreadPool(int minSize, int maxSize);
    virtual ~ThreadPool();

    // 添加新的任务
    void addTask(Task& task);
    void addTask(callback func, void* args);

    int getBusyNumber();
    int getAliveNumber();

private:
    // 线程的执行函数
    static void* workerFunc(void* args);
    static void* managerFunc(void* args);

    // 线程终止函数
    void threadExit();

    // 互斥的对数据进行访问
    pthread_mutex_t mtx;


    pthread_cond_t notEmpty;

    // 任务队列
    TaskQueue taskQueue;

    // 主线程及工作线程池
    pthread_t  managerId;
    std::vector<pthread_t> workerIds;


    int minNum;
    int maxNum;
    int busyNum;  // 正在处理逻辑的工作线程数量
    int aliveNum; // 总线程数量
    int exitNum;  // 需要执行自杀的线程数量

    // 判断线程池是否已经关闭
    bool shutdown;
};












/**
 * 以下版本使用了c++11的thread方法实现，待完成
 */

//class ThreadPool {
//public:
//    // 设置线程允许运行的线程的数量的最大值及最小值
//    ThreadPool(int minSize, int maxSize);
//    virtual ~ThreadPool();
//
//    // 添加新的任务
//    void addTask(Task& task);
//    void addTask(callback func, void* args);
//
//    int getBusyNumber();
//    int getAliveNumber();
//
//private:
//    // 线程的执行函数
//    static void* workerFunc(void* args);
//    static void* managerFunc(void* args);
//
//    // 线程终止函数
//    void threadExit();
//
//    // 互斥的对数据进行访问
//    std::mutex mtx;
//    std::condition_variable cv;
//
//    // 任务队列
//    TaskQueue taskQueue;
//    TaskQueue* taskQueue1;
//
//    // 主线程及工作线程池
//    std::thread manager;
//    std::vector<std::thread> workers;
//
//    int minNum;
//    int maxNum;
//    int busyNum;
//    int aliveNum;
//    int exitNum;
//
//    // 判断线程池是否已经关闭
//    bool shutdown;
//};


#endif //TINYDISTRIBUTECACHE_THREADPOOL_H
