#ifndef CONSISTENTHASH_H
#define CONSISTENTHASH_H

//#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <map>

#define RAND(min, max) ( rand() % ((int)(max+1) - (int)(min)) + (int)(min) )

class ConsistentHash
{
public:
    ConsistentHash(int num = 100);
    ~ConsistentHash();

    static uint32_t FNVHash(std::string key);
    static std::string RandStr(const int len);
    bool Initialize(const std::vector<std::string>& ipList);
    bool AddServer(const std::string& nodeIp);
    bool DeleteServer(const std::string& nodeIp);
    std::string GetServerIndex(const std::string& key);

    void TestBalance(int n);
    bool TestDelete();
    bool TestAdd();

private:
    std::map<uint32_t, std::string>virtualNodes;
    std::set<std::string> physicalNodes;
    int virtualNodeNum;

};

#endif // CONSISTENTHASH_H
