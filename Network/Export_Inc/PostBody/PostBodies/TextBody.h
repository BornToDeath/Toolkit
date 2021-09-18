#ifndef SMARTEYE_MODULE_TEXT_BODY_H
#define SMARTEYE_MODULE_TEXT_BODY_H

#include "PostBody/PostBody.h"


/**
 * 纯文本的 PostBody。
 * 例如发送一个Json串。
 */
class TextBody : public PostBody {

public:

    explicit TextBody(const std::string &text);

    ~TextBody() override;

    PostBodyType getType() override;

public:

    /**
     * 需要放在 Body 体里的文本
     */
    std::string text;
};


#endif
