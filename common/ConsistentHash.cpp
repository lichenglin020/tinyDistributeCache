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


// 32位Fowler-Noll-Vo算法。
// 该算法于1991年有Glenn Fowler，Landon Curt Noll，Phong Vo三人提出。
// 该算法能快速哈希大量数据并保持较小的冲突率，具有高度分散性
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

bool ConsistentHash::init(const std::vector<std::string>& ips) {
    virtualNodesMap.clear();
    physicalNodesMap.clear();

    for(int i = 0; i < ips.size(); i++){
//        if(!addServer(ips[i])){
//            cout << "the serve node: " << ips[i] << " insert failed";
//            return false;
//        }
        addServer(ips[i]);
    }
    return true;
}

bool ConsistentHash::addServer(const std::string &nodeIp) {
    // 根据虚拟节点的个数，创建虚拟节点的映射关系
    for(int i = 0; i < virtualNodesNums; i++){
        std::string tmp = nodeIp + "#" + std::to_string(i);
        virtualNodesMap[FNVHash(tmp)] = nodeIp;
    }

    // 添加实际物理节点的映射
    physicalNodesMap[FNVHash(nodeIp)] = nodeIp;
    return true;
}

bool ConsistentHash::deleteServer(const std::string &nodeIp) {
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
    return true;
}

std::string ConsistentHash::getServerIndex(const std::string &key) {
    unsigned int index = FNVHash(key);
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

std::string ConsistentHash::getNextServerIndex(const std::string &key) {
    unsigned int index = FNVHash(key);
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


/****
 * 以下为测试函数
 */

std::string ConsistentHash::RandStr(const int len)
{
    std::string ans;
    int i;
    for (i = 0; i < len; i++)
    {
        char c;
        switch ((rand() % 3))
        {
            case 1:
                c = RAND('A', 'Z');
                break;
            case 2:
                c = RAND('a', 'z');
                break;
            default:
                c = RAND('0', '9');
        }
        ans += c;
    }
    ans[++i] = '\0';
    return ans;
}

void ConsistentHash::TestInit(std::vector<std::string>& iplist)
{
    srand((unsigned)time(NULL));
    for (int i=0; i<3; i++)
    {
        std::string ip = std::to_string(rand()%256)+"."
                         +std::to_string(rand()%256)+"."
                         +std::to_string(rand()%256)+"."
                         +std::to_string(rand()%256)+":8080";
        iplist.push_back(ip);
    }
    init(iplist);
}

void ConsistentHash::TestBalance(int n)
{
    std::vector<std::string> iplist;
    TestInit(iplist);
    std::map<std::string,int> ipmap;
    for(int i=0; i<3; i++) ipmap.insert({iplist[i],i});

    int cnt[3];
    memset(cnt, 0, sizeof(cnt));
    for (int i=0; i<n; i++)
    {
        std::string s0 = ConsistentHash::RandStr(20);
        std::string s = getServerIndex(s0);
        cnt[ipmap[s]]++;
    }

    // 判断n个数据在3个节点的数据量是否大致相当。
    std::cout << cnt[0] << " " << cnt[1] << " " << cnt[2] << " " << cnt[0]+cnt[1]+cnt[2] << std::endl;
}

bool ConsistentHash::TestDelete()
{
    std::vector<std::string> iplist;
    TestInit(iplist);

    for (int i=0; i<1000; i++)
    {
        std::string s0 = ConsistentHash::RandStr(20);
        std::string s1 = getServerIndex(s0);
        for (auto& ip:iplist)
        {
            deleteServer(ip);
            std::string s2 = getServerIndex(s0);
            // 如果一个值没有映射到要被删除的节点，但是当该节点被删除后，这个值映射的地址发生了变化，那么则认为删除出错了
            if (s2 != s1 && ip != s1) {
                std::cout << ip << " " << s1 << " " << s2 << std::endl;
                return false;
            }
            addServer(ip);
        }
    }
    return true;
}

bool ConsistentHash::TestAdd()
{
    std::vector<std::string> iplist;
    TestInit(iplist);

    for (int i=0; i<1000; i++)
    {
        std::string newip = std::to_string(rand()%256)+"."
                            +std::to_string(rand()%256)+"."
                            +std::to_string(rand()%256)+"."
                            +std::to_string(rand()%256)+":8080";

        std::string s0 = ConsistentHash::RandStr(20);
        std::string s1 = getServerIndex(s0);
        addServer(newip);
        std::string s2 = getServerIndex(s0);
        // 如果添加新节点后，数据所存放的节点发生了变化，但是又没有映射到新添加的节点中，那么便认为出错了。
        if (s2 != s1 && newip != s2) {
            std::cout << newip << " " << s1 << " " << s2 << std::endl;
            return false;
        }
        deleteServer(newip);

    }
    return true;
}

void ConsistentHash::TestGetNextIndex()
{
    std::vector<std::string> iplist;
    TestInit(iplist);

    for (auto& ip:iplist) std::cout<< ip << "->" << getNextServerIndex(ip) << std::endl;
}






