
#include "PostBody/PostBodies/FormBody.h"


FormBody::FormBody(const std::string &name) {
    this->name = name;
    this->contentType = "multipart/form-data; charset=UTF-8";
}

FormBody::~FormBody() = default;


