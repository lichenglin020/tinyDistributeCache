//
// Created by Chenglinli on 2022/5/9.
//

#include "HttpJson.h"

/**
 * 对收到的数据构建JSON数据
 * @param js
 * @param buffer
 * @return 返回是否构建成功
 */
bool HttpJson::getRequestJson(json &js, char *buffer) {
    if(json::accept(buffer)){
        js = json::parse(buffer);
        return true;
    }else{
        return false;
    }
}

/**
 * 获取发送至备份服务器的JSON数据
 * @param key
 * @param value
 * @return
 */
json HttpJson::writeBackupJson(const std::string &key, const std::string &value) {
    json writeBackupJson, data;
    writeBackupJson["type"] = KEY_VALUE_WRITE_BK;
    data["flag"] = true;
    data["key"] = key;
    data["value"] = value;
    writeBackupJson["data"] = data;
    return writeBackupJson;
}

/**
 * 构建返回给客户端查询结果的JSON数据
 * @param key
 * @param value
 * @return
 */
json HttpJson::respondClientJson(const std::string &key, const std::string &value) {
    json respondClientJson, data;
    respondClientJson["type"] = KEY_VALUE_READ;
    data["flag"] = !value.empty();
    data["key"] = key;
    data["value"] = value;
    respondClientJson["data"] = data;
    return respondClientJson;
}

/**
 * 构建心跳包Json数据格式
 * @param _ip
 * @return
 */
json HttpJson::heartbeatJson(const std::string& _ip){
    json data, heartbeat_json;
    heartbeat_json["type"] = HEART_BEAT;
    data["ip"] = _ip;
    data["state"] = true;
    heartbeat_json["data"] = data;
    return heartbeat_json;
}

