#include "post_body/post_bodies/binary_body.h"

BinaryBody::BinaryBody(const char *data, unsigned long long dataLength) {
    this->data_ = data;
    this->data_length_ = dataLength;
}

PostBodyType BinaryBody::GetType() {
    return POST_BODY_TYPE_RAW_BINARY;
}

BinaryBody::~BinaryBody() = default;

