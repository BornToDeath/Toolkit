#ifndef NETWORK_BINARY_BODY_H
#define NETWORK_BINARY_BODY_H

#include "post_body/post_body.h"


/**
 * 二进制形式的 post_body。
 *
 * 析构时，不会主动析构 data 数组。
 */
class BinaryBody : public PostBody {
public:
    BinaryBody(const char *data, unsigned long long dataLength);

    ~BinaryBody() override;

public:
    PostBodyType GetType() override;

public:
    /**
     * 需要 Post 的二进制数据
     */
    const char *data_;

    /**
     * 二进制数据的长度
     */
    unsigned long long data_length_;
};


#endif
