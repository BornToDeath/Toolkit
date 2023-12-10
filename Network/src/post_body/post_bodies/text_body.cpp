#include "post_body/post_bodies/text_body.h"

TextBody::TextBody(const std::string &text) {
    this->text_ = text;
}

PostBodyType TextBody::GetType() {
    return POST_BODY_TYPE_RAW_TEXT;
}

TextBody::~TextBody() = default;

