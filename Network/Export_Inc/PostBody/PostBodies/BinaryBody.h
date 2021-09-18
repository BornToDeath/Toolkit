#ifndef SMARTEYE_MODULE_BINARY_BODY_H
#define SMARTEYE_MODULE_BINARY_BODY_H

#include "PostBody/PostBody.h"


/**
 * 二进制形式的 PostBody。
 *
 * 析构时，不会主动析构 data 数组。
 */
class BinaryBody : public PostBody {

public:

    BinaryBody(const char *data, unsigned long long dataLength);

    ~BinaryBody() override;

public:

    /**
     * 需要 Post 的二进制数据
     */
    const char *data;

    /**
     * 二进制数据的长度
     */
    unsigned long long dataLength;


public:
    PostBodyType getType() override;

};


#endif
