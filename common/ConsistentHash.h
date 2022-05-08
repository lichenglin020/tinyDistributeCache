//
// Created by Chenglinli on 2022/4/26.
//

#ifndef TINYDISTRIBUTECACHE_CONSISTENTHASH_H
#define TINYDISTRIBUTECACHE_CONSISTENTHASH_H

#include <map>
#include <string>
#include <cstring>
#include <vector>
#include <shared_mutex>
#include <mutex>

/**
 * 一致性哈希表
 * 1. 配置ip地址到哈希表上节点的映射关系，包括实际物理节点和虚拟节点
 * 2. 实现key值到指定ip地址的映射
 */
class ConsistentHash {
public:
    ConsistentHash(int num = 100);
    virtual ~ConsistentHash();

    static unsigned int FNVHash(std::string key);
    void init(const std::vector<std::string>& ips);
    void addServer(const std::string& nodeIp);
    void deleteServer(const std::string& nodeIp);
    std::string getServerIndex(const std::string& key);
    std::string getNextServerIndex(const std::string& ip);



private:
    // 这里存储的是哈希值 到 IP及端口 之间的映射。
    std::map<unsigned int, std::string> virtualNodesMap; // 一致性哈希的虚拟节点信息
    std::map<unsigned int, std::string> physicalNodesMap; // 一致性哈希的实际物理节点
    int virtualNodesNums;   // 设置每一个实际物理节点配套的虚拟节点的个数。

    std::shared_mutex sharedMutex; // 读写锁
};


#endif //TINYDISTRIBUTECACHE_CONSISTENTHASH_H
