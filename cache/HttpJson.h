//
// Created by Chenglinli on 2022/5/9.
//

#ifndef TINYDISTRIBUTECACHE_HTTPJSON_H
#define TINYDISTRIBUTECACHE_HTTPJSON_H
#include "../common/json.hpp"
using json = nlohmann::json;

#define KEY_VALUE_READ 0      // 客户端读取数据请求标志位
#define KEY_VALUE_WRITE 1     // 客户端写入数据请求标志位
#define KEY_VALUE_WRITE_BK 2  // 向备份节点同步数据的标志位
#define HEART_BEAT 3          // 心跳包标志位

/**
 * 定义通信的Json格式
 */

class HttpJson {
public:
    static bool getRequestJson(json& js, char* buffer);
    static json writeBackupJson(const std::string& key, const std::string& value);
    static json respondClientJson(const std::string &key, const std::string &value);
    static json heartbeatJson(const std::string& _ip);
};


#endif //TINYDISTRIBUTECACHE_HTTPJSON_H
