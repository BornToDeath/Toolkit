#ifndef NETWORK_TEXT_BODY_H
#define NETWORK_TEXT_BODY_H

#include "post_body/post_body.h"


/**
 * 纯文本的 post_body。
 * 例如发送一个Json串。
 */
class TextBody : public PostBody {

public:

    explicit TextBody(const std::string &text);

    ~TextBody() override;

    PostBodyType GetType() override;

public:

    /**
     * 需要放在 Body 体里的文本
     */
    std::string text_;
};


#endif
