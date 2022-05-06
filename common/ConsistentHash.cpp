//
// Created by Chenglinli on 2022/4/26.
//

#include <iostream>
#include "ConsistentHash.h"


ConsistentHash::ConsistentHash(int num) : virtualNodesNums(num){}

ConsistentHash::~ConsistentHash() {
    virtualNodesMap.clear();
    physicalNodesMap.clear();
}


/**
 * 32位Fowler-Noll-Vo算法。
 * 该算法于1991年由Glenn Fowler，Landon Curt Noll，Phong Vo三人提出。
 * 该算法能快速哈希大量数据并保持较小的冲突率，具有高度分散性
 * @param key 关键字字符串string
 * @return 哈希值
 */
unsigned int ConsistentHash::FNVHash(std::string key) {
    const int p = 16777619;
    uint32_t hash = 2166136261;
    for (int idx = 0; idx < key.size(); ++idx) {
        hash = (hash ^ key[idx]) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    if (hash < 0) {
        hash = -hash;
    }
    return hash;
}

/**
 * 一致性哈希表初始化配置函数，配置表中可用节点
 * @param ips ip地址信息列表("ip:端口"的string数组，例如["127.0.0.1:8000", ....])
 * @return
 */
void ConsistentHash::init(const std::vector<std::string>& ips) {
    virtualNodesMap.clear();
    physicalNodesMap.clear();

    for(int i = 0; i < ips.size(); i++){
        addServer(ips[i]);
    }
}

/**
 * 添加指定IP到物理对应节点以及虚拟对应节点的映射关系
 * @param nodeIp "ip:端口"，例如"127.0.0.1:8000"
 * @return
 */
void ConsistentHash::addServer(const std::string &nodeIp) {
    // 根据虚拟节点的个数，创建虚拟节点的映射关系
    for(int i = 0; i < virtualNodesNums; i++){
        std::string tmp = nodeIp + "#" + std::to_string(i);
        virtualNodesMap[FNVHash(tmp)] = nodeIp;
    }

    // 添加实际物理节点的映射
    physicalNodesMap[FNVHash(nodeIp)] = nodeIp;
}

/**
 * 删除指定IP到物理对应节点以及虚拟对应节点的映射关系
 * @param nodeIp "ip:端口"，例如"127.0.0.1:8000"
 * @return
 */
void ConsistentHash::deleteServer(const std::string &nodeIp) {
    // 删除虚拟节点的相关信息
    // 此处并没有完成相关节点信息的重新配置，会在后续的使用中处理
    for(int i = 0; i < virtualNodesNums; i++){
        std::string tmp = nodeIp + "#" + std::to_string(i);
        unsigned int delKey = FNVHash(tmp);
        if(virtualNodesMap.find(delKey) != virtualNodesMap.end()){
            virtualNodesMap.erase(delKey);
        }
    }

    // 删除实际的物理节点
    unsigned int delKey = FNVHash(nodeIp);
    auto it = physicalNodesMap.find(delKey);
    if(it != physicalNodesMap.end()) physicalNodesMap.erase(it);
}

/**
 * 根据关键字，获取其应该存入的IP地址信息
 * @param key 关键字字符串
 * @return
 */
std::string ConsistentHash::getServerIndex(const std::string &key) {
    unsigned int index = FNVHash(key);
    // 此处使用虚拟节点进行判断
    auto it = virtualNodesMap.lower_bound(index); // 向后寻找第一个大于等于index的可用的节点
    if(it == virtualNodesMap.end()){
        if(virtualNodesMap.empty()){
            std::cout << "there is no available server nodes" << std::endl;
            return "";
        }
        // 环状哈希表，抵达结尾，则返回头节点值
        return virtualNodesMap.begin() -> second;
    }else{
        return it -> second;
    }
}

/**
 * 获取备份节点IP地址。根据传入的当前IP地址，顺时针获取其下一个ip地址
 * @param key ip地址字符串，形如"ip:端口"，例如"127.0.0.1:8000"
 * @return
 */
std::string ConsistentHash::getNextServerIndex(const std::string &ip) {
    unsigned int index = FNVHash(ip);
    // 这里使用实际物理节点进行判断
    auto it = physicalNodesMap.upper_bound(index); // 向后寻找第一个大于index 的可用的节点
    if(it == physicalNodesMap.end()){
        if(physicalNodesMap.empty()){
            std::cout << "there is no available server nodes" << std::endl;
            return "";
        }
        // 环状哈希表，抵达结尾，则返回头节点值
        return physicalNodesMap.begin() -> second;
    }else{
        return it -> second;
    }
}





