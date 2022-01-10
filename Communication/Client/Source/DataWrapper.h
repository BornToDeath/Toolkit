//
// Created by 郑伟 on 2021/5/14.
//

#ifndef ANDROID_DATAWRAPPER_H
#define ANDROID_DATAWRAPPER_H


#include <cstdint>
#include <memory>
#include "Common/Define.h"


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
     *   注意: outputData 需要在外部手动释放！
     * @param dataType   待封装数据的类型。见 DataType
     * @param inputData  待封装数据
     * @param inputLen   待封装数据的长度
     * @param outputData 封装好的数据 (socket 实际传输的数据)
     * @param outputLen  已封装数据的长度
     * @return
     */
    static bool wrap(int dataType, const BYTE *inputData, int inputLen, BYTE **outputData, int &outputLen);

    /**
     * 解析数据的类型。
     *   注意: outputData 需要在外部手动释放!
     * @param inputData  socket 接收到的数据
     * @param inputLen   已接收数据的长度
     * @param dataType   已接收数据的类型
     * @param outputData 已解析的数据
     * @param outputLen  已解析数据的长度
     * @return
     */
    static bool parse(const BYTE *inputData, int inputLen, int &dataType, BYTE **outputData, int &outputLen);

    /**
     * 解析 Socket 数据类型
     * @param data  待解析的数据
     * @param start 待解析的起始字节位置
     * @return
     */
    static int parseDataType(const BYTE *data, int start = 0);

private:

    static const int UNIT_LENGTH = sizeof(signed int);

};


#endif //ANDROID_DATAWRAPPER_H
