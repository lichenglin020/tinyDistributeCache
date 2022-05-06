//
// Created by Chenglinli on 2022/5/6.
//

#include "../common/ConsistentHash.h"
#include <iostream>
#include <string>

using namespace std;
#define RAND(a, b) ( rand() % ((int)(b+1) - (int)(a) + 1) + (int)(a) )

// 测试函数
std::string RandStr(const int len)
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

void TestInit(std::vector<std::string>& iplist, ConsistentHash& consistentHash)
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
    consistentHash.init(iplist);
}

void TestBalance(int n, ConsistentHash& consistentHash)
{
    std::vector<std::string> iplist;
    TestInit(iplist, consistentHash);
    std::map<std::string,int> ipmap;
    for(int i=0; i<3; i++) ipmap.insert({iplist[i],i});

    int cnt[3];
    memset(cnt, 0, sizeof(cnt));
    for (int i=0; i<n; i++)
    {
        std::string s0 = RandStr(20);
        std::string s = consistentHash.getServerIndex(s0);
        cnt[ipmap[s]]++;
    }

    // 判断n个数据在3个节点的数据量是否大致相当。
    std::cout << cnt[0] << " " << cnt[1] << " " << cnt[2] << " " << cnt[0]+cnt[1]+cnt[2] << std::endl;
}

bool TestDelete(ConsistentHash& consistentHash)
{
    std::vector<std::string> iplist;
    TestInit(iplist, consistentHash);

    for (int i=0; i<1000; i++)
    {
        std::string s0 = RandStr(20);
        std::string s1 = consistentHash.getServerIndex(s0);
        for (auto& ip:iplist)
        {
            consistentHash.deleteServer(ip);
            std::string s2 = consistentHash.getServerIndex(s0);
            // 如果一个值没有映射到要被删除的节点，但是当该节点被删除后，这个值映射的地址发生了变化，那么则认为删除出错了
            if (s2 != s1 && ip != s1) {
                std::cout << ip << " " << s1 << " " << s2 << std::endl;
                return false;
            }
            consistentHash.addServer(ip);
        }
    }
    return true;
}

bool TestAdd(ConsistentHash& consistentHash)
{
    std::vector<std::string> iplist;
    TestInit(iplist, consistentHash);

    for (int i=0; i<1000; i++)
    {
        std::string newip = std::to_string(rand()%256)+"."
                            +std::to_string(rand()%256)+"."
                            +std::to_string(rand()%256)+"."
                            +std::to_string(rand()%256)+":8080";

        std::string s0 = RandStr(20);
        std::string s1 = consistentHash.getServerIndex(s0);
        consistentHash.addServer(newip);
        std::string s2 = consistentHash.getServerIndex(s0);
        // 如果添加新节点后，数据所存放的节点发生了变化，但是又没有映射到新添加的节点中，那么便认为出错了。
        if (s2 != s1 && newip != s2) {
            std::cout << newip << " " << s1 << " " << s2 << std::endl;
            return false;
        }
        consistentHash.deleteServer(newip);

    }
    return true;
}

void TestGetNextIndex(ConsistentHash& consistentHash)
{
    std::vector<std::string> iplist;
    TestInit(iplist, consistentHash);

    for (auto& ip:iplist) {
        std::cout << ip << "->" << consistentHash.getNextServerIndex(ip) << std::endl;
    }
}


int main(){

    std::cout << "ok" << std::endl;
    ConsistentHash consistentHash(100);
    std::cout << "TestBalance: " << std::endl;
    TestBalance(30000, consistentHash);
    std::cout << std::endl;
    std::cout << "TestAdd" << std::endl;
    TestAdd(consistentHash);
    std::cout << std::endl;
    std::cout << "TestDelete" << std::endl;
    TestDelete(consistentHash);
    std::cout << std::endl;
    std::cout << "TestBackUp" << std::endl;
    TestGetNextIndex(consistentHash);
    return 0;
}