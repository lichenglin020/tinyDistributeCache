//
// Created by Chenglinli on 2022/4/26.
//

#ifndef TINYDISTRIBUTECACHE_TASK_H
#define TINYDISTRIBUTECACHE_TASK_H

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

private:
    callback func;  // 指定任务需要执行的回调函数
    void* arg;      // 回调函数的参数集合
};

/*
 * 任务队列
 */
class TaskQueue{
public:

};



#endif //TINYDISTRIBUTECACHE_TASK_H
