#include "post_body/post_bodies/form_body.h"

FormBody::FormBody(const std::string &name) {
    this->name_ = name;
    this->content_type_ = "multipart/form-data; charset=UTF-8";
}

FormBody::~FormBody() = default;


