#ifndef NETWORK_FORM_BODY_H
#define NETWORK_FORM_BODY_H

#include <utility>

#include "post_body/post_body.h"


/**
 * 表单 Body 体
 */
class FormBody : public PostBody {

public:

    explicit FormBody(const std::string &name);

    ~FormBody() override;

public:

    /**
     * 这个 Body 体的 名字
     */
    std::string name_;

    /**
     * 这个 Body 体的 Header
     */
    std::string header_;

    /**
     * 这个 Body 体的 ContentType
     */
    std::string content_type_;

};

#endif
