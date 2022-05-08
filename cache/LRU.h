//
// Created by Chenglinli on 2022/4/26.
//

#ifndef TINYDISTRIBUTECACHE_LRU_H
#define TINYDISTRIBUTECACHE_LRU_H

#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>


struct LRUNode {
    std::string key, value;
    LRUNode* pre;
    LRUNode* next;
    LRUNode(): key(20,' '), value(200,' '), pre(nullptr), next(nullptr) {}
    LRUNode(std::string _key, std::string _value): key(_key), value(_value), pre(nullptr), next(nullptr) {}
};

class LRUCache {
public:
    LRUCache(int _capacity);
    virtual ~LRUCache();

    std::string get(std::string key, bool flag = true);
    void put(std::string key, std::string value);
    int size();

private:
    std::unordered_map<std::string, LRUNode*> umap;
    LRUNode* head;
    LRUNode* tail;
    int capacity;
    std::shared_mutex sharedMutex; // 读写锁

    // 以下操作均只针对双向链表，不涉及umap操作，降低耦合
    void addToHead(LRUNode* node);
    void removeNode(LRUNode* node);
    void moveToHead(LRUNode* node);
    LRUNode* removeTail();
};


#endif //TINYDISTRIBUTECACHE_LRU_H
