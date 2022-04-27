//
// Created by Chenglinli on 2022/4/26.
//

#include "ThreadPool.h"
#include <iostream>

/**
 * 创建线程池，初始时包含一个主线程和minSize大小的工作线程集合
 * @param minSize 线程池的最小线程数量
 * @param maxSize 线程池的最大线程数量
 */
ThreadPool::ThreadPool(int minSize, int maxSize):
minNum(minSize), maxNum(maxSize), busyNum(0), aliveNum(minSize), shutdown(false), workerIds(maxSize, 0){

    if(pthread_mutex_init(&mtx, nullptr) != 0 || pthread_cond_init(&notEmpty, nullptr) != 0){
        std::cout << "init mutex or condition fail..." << std::endl;
    }else{
        for(int i = 0; i < minNum; i++){
            pthread_create(&workerIds[i], nullptr, workerFunc, this);
        }
        pthread_create(&managerId, nullptr, managerFunc, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown = true;
    pthread_join(managerId, nullptr);
    for(int i = 0; i < aliveNum; i++){
        pthread_cond_signal(&notEmpty);
    }

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&notEmpty);
}

void ThreadPool::addTask(Task& task){
    if (shutdown)
        return;
    taskQueue.addTask(task);
    pthread_cond_signal(&notEmpty);
}

int ThreadPool::getAliveNumber(){
    int threadNum = 0;
    pthread_mutex_lock(&mtx);
    threadNum = aliveNum;
    pthread_mutex_unlock(&mtx);
    return threadNum;
}

int ThreadPool::getBusyNumber(){
    int busyNum = 0;
    pthread_mutex_lock(&mtx);
    busyNum = busyNum;
    pthread_mutex_unlock(&mtx);
    return busyNum;
}

void* ThreadPool::workerFunc(void *args) {
    ThreadPool* pool = static_cast<ThreadPool*>(args);
    while(true){
        pthread_mutex_lock(&pool -> mtx);
        // 如果任务队列为空，那么就持续等待，直到不为空的信号到来
        while(pool -> taskQueue.size() == 0 && !pool -> shutdown){
            pthread_cond_wait(&pool -> notEmpty, &pool -> mtx);
            if(pool -> exitNum > 0){
                pool -> exitNum--;
                if(pool -> aliveNum > pool -> minNum){
                    pool -> aliveNum--;
                    pthread_mutex_unlock(&pool -> mtx);
                    pool -> threadExit();
                }
            }
        }

        // 如果线程结束位已确定，那么就结束当前线程的执行
        if(pool -> shutdown){
            pthread_mutex_unlock(&pool -> mtx);
            pool -> threadExit();
        }

        // 从任务队列中获取一个任务，并执行
        Task task = pool -> taskQueue.getTask();
        pool -> busyNum++;
        pthread_mutex_unlock(&pool -> mtx);
        task.run();

        pthread_mutex_lock(&pool -> mtx);
        pool -> busyNum--;
        pthread_mutex_unlock(&pool -> mtx);

    }

    return nullptr;
}

void* ThreadPool::managerFunc(void *args) {
    ThreadPool* pool = static_cast<ThreadPool*>(args);
    while(!pool -> shutdown){
        pthread_mutex_lock(&pool -> mtx);
        int queueSize = pool->taskQueue.size();
        int liveNum = pool->aliveNum;
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mtx);

        int number = 2; // 单次增加或者减少的线程数量

        // 如果任务数量多于存活的线程并且当前存活的线程数小于线程最大数量，那么就增加线程。
        if(queueSize > liveNum && liveNum < pool -> maxNum){
            pthread_mutex_lock(&pool -> mtx);
            for(int i = 0, num = 0; i < pool -> maxNum && num < number && pool -> aliveNum < pool -> maxNum; i++){
                if(pool -> workerIds[i] == 0){
                    pthread_create(&pool -> workerIds[i], nullptr, workerFunc, pool);
                    num++;
                    pool -> aliveNum++;
                }
            }
            pthread_mutex_unlock(&pool -> mtx);
        }

        // 如果存活线程数量大于工作线程的两倍，并且存活线程数量大于了最小线程数量，那么就进行线程自销操作（通过exitNum）
        if(busyNum * 2 < liveNum && liveNum > pool -> minNum){
            pthread_mutex_lock(&pool -> mtx);
            pool->exitNum = number;
            pthread_mutex_unlock(&pool -> mtx);
            for (int i = 0; i < number; ++i){
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return nullptr;
}

/**
 * 将线程数组中的当前线程置为空，并停止当前线程的执行。
 */
void ThreadPool::threadExit() {
    auto exitThread = pthread_self();
    for(int i = 0; i < maxNum; i++){
        if(workerIds[i] == exitThread){
            workerIds[i] = 0;
            break;
        }
    }
    pthread_exit(nullptr);
}



/**
 * 以下方法使用了c++11 的thread方法，待完成
 */

///**
// *
// * @param minSize 线程池的最小线程数量
// * @param maxSize 线程池的最大线程数量
// */
//ThreadPool::ThreadPool(int minSize, int maxSize):
//minNum(minSize), maxNum(maxSize), busyNum(0), aliveNum(minSize), shutdown(false),
//manager(managerFunc, this), workers(maxSize){
//    for(int i = 0; i < minNum; i++){
//        workers[i] = std::thread(workerFunc, this);
//    }
//}
//
//ThreadPool::~ThreadPool() {
//    shutdown = true;
//    manager.join();
//    cv.notify_all();
//    workers.clear();
//}
//
///**
// * 添加新的任务到任务队列中
// * @param task
// */
//void ThreadPool::addTask(Task &task) {
//    if(shutdown) return;
//    taskQueue.addTask(task);
//    cv.notify_one();
//}
//
//void ThreadPool::addTask(callback func, void *args) {
//    if(shutdown) return;
//    taskQueue.addTask(func, args);
//    cv.notify_one();
//}
//
//int ThreadPool::getAliveNumber() {
//    int num = 0;
//    std::unique_lock<std::mutex> lck(mtx);
//    num = aliveNum;
//    lck.unlock();
//    return num;
//}
//
//int ThreadPool::getBusyNumber() {
//    int num = 0;
//    std::unique_lock<std::mutex> lck(mtx);
//    num = busyNum;
//    lck.unlock();
//    return num;
//}
//
//void* ThreadPool::workerFunc(void* args) {
//    ThreadPool* pool = static_cast<ThreadPool*>(args);
//    while(true){
//        std::unique_lock<std::mutex> lck(pool -> mtx);
//        while(!pool -> shutdown){
//            pool -> cv.wait(lck, [&]() -> bool{
//                return pool -> taskQueue.size() > 0;
//            });
//
//
//        }
//
//
//
//
//
//    }
//
//    return nullptr;
//}
//
//
//void ThreadPool::threadExit() {
//
//}
