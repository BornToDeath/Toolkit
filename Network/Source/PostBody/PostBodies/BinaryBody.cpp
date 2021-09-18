
#include "PostBody/PostBodies/BinaryBody.h"


BinaryBody::BinaryBody(const char *data, unsigned long long dataLength) {
    this->data = data;
    this->dataLength = dataLength;
}

PostBodyType BinaryBody::getType() {
    return POST_BODY_TYPE_RAW_BINARY;
}

BinaryBody::~BinaryBody() = default;

