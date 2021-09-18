
#include "PostBody/PostBodies/TextBody.h"


TextBody::TextBody(const std::string &text) {
    this->text = text;
}

PostBodyType TextBody::getType() {
    return POST_BODY_TYPE_RAW_TEXT;
}

TextBody::~TextBody() = default;

