#ifndef COMMUNICATION_DATA_WRAPPER_H
#define COMMUNICATION_DATA_WRAPPER_H


#include <cstdint>
#include <memory>

#include "common/define.h"


/**
 * 数据封装类。封装(或解析)发送(或接收)的 Socket 数据
 */
class DataWrapper {

public:

    DataWrapper() = delete;

    ~DataWrapper() = delete;

    DataWrapper(const DataWrapper &obj) = delete;

    DataWrapper &operator=(const DataWrapper &obj) = delete;

public:

    /**
     * 封装数据，用于 Socket 传输。在数据首部添加 4 字节的数据类型。
     *   注意: output_data 需要在外部手动释放！
     * @param data_type   待封装数据的类型。见 DataType
     * @param input_data  待封装数据
     * @param input_len   待封装数据的长度
     * @param output_data 封装好的数据 (socket 实际传输的数据)
     * @param output_len  已封装数据的长度
     * @return
     */
    static bool Wrap(int data_type, const BYTE *input_data, int input_len, BYTE **output_data, int &output_len);

    /**
     * 解析数据的类型。
     *   注意: output_data 需要在外部手动释放!
     * @param input_data  socket 接收到的数据
     * @param input_len   已接收数据的长度
     * @param data_type   已接收数据的类型
     * @param output_data 已解析的数据
     * @param output_len  已解析数据的长度
     * @return
     */
    static bool Parse(const BYTE *input_data, int input_len, int &data_type, BYTE **output_data, int &output_len);

    /**
     * 解析 Socket 数据类型
     * @param data  待解析的数据
     * @param start 待解析的起始字节位置
     * @return
     */
    static int ParseDataType(const BYTE *data, int start = 0);

private:

    static const int kIntLength = sizeof(signed int);

};


#endif //COMMUNICATION_DATA_WRAPPER_H
