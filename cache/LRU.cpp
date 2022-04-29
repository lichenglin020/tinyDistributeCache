//
// Created by Chenglinli on 2022/4/26.
//

#include "LRU.h"

LRUCache::LRUCache(int _capacity): capacity(_capacity){
    head = new LRUNode();
    tail = new LRUNode();
    head->next = tail;
    tail->pre = head;
}

LRUCache::~LRUCache() {

    auto it = umap.begin();
    while(it != umap.end()){
        delete it -> second;
        umap.erase(it++);
    }

    delete head;
    delete tail;
}

std::string LRUCache::get(std::string key, bool flag) {
    if (umap.find(key) == umap.end()) {
        return "";
    }

    auto node = umap[key];
    if(flag) moveToHead(node);

    return node -> value;
}

void LRUCache::put(std::string key, std::string value) {
    if (umap.find(key) == umap.end()) {
        auto* node = new LRUNode(key, value);
        umap[key] = node;
        addToHead(node);

        if (umap.size() > capacity) {
            auto removedNode = removeTail();
            umap.erase(removedNode -> key);
            delete removedNode;
        }
    }
    else {
        auto node = umap[key];
        node -> value = value;
        moveToHead(node);
    }
}

void LRUCache::addToHead(LRUNode* node) {
    node -> pre = head;
    node -> next = head->next;
    head -> next -> pre = node;
    head -> next = node;
}

void LRUCache::removeNode(LRUNode* node) {
    node -> pre -> next = node -> next;
    node -> next -> pre = node -> pre;
}
void LRUCache::moveToHead(LRUNode* node) {
    removeNode(node);
    addToHead(node);
}

LRUNode* LRUCache::removeTail() {
    LRUNode* node = tail -> pre;
    removeNode(node);
    return node;
}
