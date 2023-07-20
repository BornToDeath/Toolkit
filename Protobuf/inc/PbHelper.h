//
// Created by lixiaoqing on 2023/7/17.
//

#ifndef PROTOBUF_PBHELPER_H
#define PROTOBUF_PBHELPER_H

#include <string>
#include "google/protobuf/message.h"

class PbHelper final {

public:

    PbHelper() = default;

    ~PbHelper() = default;

public:

    /**
     * pb 转 二进制数据
     * @param message Protobuf 对象。Message 是基类，自定义的 pb 类都继承自此类
     * @param output  序列化之后的结果。string 只是作为序列化之后的数据的容器，并非意味着可见字符串
     * @return
     */
    static bool toBinary(const google::protobuf::Message &message, std::string &output);

    /**
     * pb 转 string (可见字符串)
     * @param message
     * @param output
     * @return
     */
    static bool toString(const google::protobuf::Message &message, std::string &output);

    /**
     * pb 转 json
     * @param message
     * @param output
     * @return
     */
    static bool toJson(const google::protobuf::Message &message, std::string &output);

    /**
     * 二进制 转 pb
     * Protobuf 序列化和反序列化都是二进制数据，所以 `SerializeToString()/ParseFromString()` 方法输出/输入的 string 也是二进制数据，
     * 不可以使用 `DebugString()` 或可见的文件字符串。
     * @param input   二进制流
     * @param message
     * @return
     */
    static bool parseFromBinary(const std::string &input, google::protobuf::Message &message);

    /**
     * json 转 pb
     * @param input
     * @param message
     * @return
     */
    static bool parseFromJson(const std::string &input, google::protobuf::Message &message);
};

#endif //PROTOBUF_PBHELPER_H
