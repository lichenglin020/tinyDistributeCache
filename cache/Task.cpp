//
// Created by Chenglinli on 2022/4/26.
//

#include "Task.h"


/**
 * 向队列中添加新的Task任务
 * @param task 指定的Task对象
 */
void TaskQueue::addTask(Task &task) {
    std::unique_lock<std::mutex> ulck(mtx);
    mQue.push(task);
}

/**
 * 向队列中添加新的Task任务
 * @param func 新任务的回调函数
 * @param args 回调函数配套的参数信息
 */
void TaskQueue::addTask(callback func, void *args) {
    std::unique_lock<std::mutex> ulck(mtx);
//    Task newTask(func, args);
//    mQue.push(newTask);
    mQue.emplace(func, args);
}

/**
 * 获取一个任务，并将该任务从队列中删除
 * @return 返回队列头部的Task任务对象
 */
Task TaskQueue::getTask() {
    Task tmpTask;
    std::unique_lock<std::mutex> ulck(mtx);
    if(!mQue.empty()){
        tmpTask = mQue.front();
        mQue.pop();
    }
    ulck.unlock();
    return tmpTask;
}
