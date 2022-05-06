//
// Created by Chenglinli on 2022/4/26.
//

#ifndef TINYDISTRIBUTECACHE_TASK_H
#define TINYDISTRIBUTECACHE_TASK_H
#include <mutex>
#include <queue>

using callback = void(*)(void*);
//typedef void (*callback)(void*);

/*
 * 任务类：
 * 包含任务的回调函数，以及回调函数的参数信息
 */
class Task {
public:
    Task(): func(nullptr), arg(nullptr){}
    Task(callback _func, void* _arg):func(_func), arg(_arg){}
    virtual ~Task(){}
    void run(){
        func(arg);
        arg = nullptr;
    }
private:
    callback func;  // 指定任务需要执行的回调函数
    void* arg;      // 回调函数的参数集合
};

/*
 * 任务队列(线程安全的)
 */
class TaskQueue{
public:
    TaskQueue(){};
    virtual ~TaskQueue(){};

    // 添加任务方法(已经自带上锁)
    void addTask(Task& task);
    void addTask(callback func, void* args);

    // 获取任务方法
    Task getTask();

    // 获取队列的大小
    int size(){
        return mQue.size();
    }

private:
    std::mutex mtx;
    std::queue<Task> mQue;
};



#endif //TINYDISTRIBUTECACHE_TASK_H
