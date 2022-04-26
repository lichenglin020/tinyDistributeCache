//
// Created by Chenglinli on 2022/4/26.
//

#ifndef TINYDISTRIBUTECACHE_CONSISTENTHASH_H
#define TINYDISTRIBUTECACHE_CONSISTENTHASH_H

#include <map>
#include <string>
#include <vector>
//using namespace std;

#define RAND(a, b) ( rand() % ((int)(b+1) - (int)(a) + 1) + (int)(a) )

class ConsistentHash {
public:
    ConsistentHash(int num = 100);
    virtual ~ConsistentHash();

    // 根据键值信息，进行哈希映射得到指定哈希值的函数, 根据string获得一个unsigned int 的哈希值。
    static unsigned int FNVHash(std::string key);

    // 初始化一致性哈希节点信息的函数
    // 参数是一个包含ip:端口的string数组，即["127.0.0.1:8000", ....]
    bool init(const std::vector<std::string>& ips);

    // 增加服务的节点信息，传入的是ip:端口信息，即"127.0.0.1:8000"
    bool addServer(const std::string& nodeIp);

    // 删除服务的节点的信息，传入的是ip:端口信息，即"127.0.0.1:8000"
    bool deleteServer(const std::string& nodeIp);

    // 根据待存储数据的键，算出该键值信息应该存储在哪一个节点上
    std::string getServerIndex(const std::string& key);

    // 根据待存储数据的键，算出该键值信息作为备份信息应该存储的下一个节点信息。
    std::string getNextServerIndex(const std::string& key);

    // 测试函数
    static std::string RandStr(const int len);
    void TestInit(std::vector<std::string>& iplist);
    void TestBalance(int n);
    bool TestDelete();
    bool TestAdd();
    void TestGetNextIndex();

private:
    // 这里存储的是哈希值 到 IP及端口 之间的映射。
    std::map<unsigned int, std::string> virtualNodesMap; // 一致性哈希的虚拟节点信息
    std::map<unsigned int, std::string> physicalNodesMap; // 一致性哈希的实际物理节点
    int virtualNodesNums;   // 设置每一个实际物理节点配套的虚拟节点的个数。
};


#endif //TINYDISTRIBUTECACHE_CONSISTENTHASH_H
