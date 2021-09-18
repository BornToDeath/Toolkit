#ifndef SMARTEYE_MODULE_FORM_BODY_H
#define SMARTEYE_MODULE_FORM_BODY_H

#include <utility>
#include "PostBody/PostBody.h"


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
    std::string name;

    /**
     * 这个 Body 体的 Header
     */
    std::string header;

    /**
     * 这个 Body 体的 ContentType
     */
    std::string contentType;

};

#endif
