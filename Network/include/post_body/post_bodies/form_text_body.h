#ifndef NETWORK_FORM_TEXT_BODY_H
#define NETWORK_FORM_TEXT_BODY_H

#include "form_body.h"


class FormTextBody : public FormBody {

public:

    /**
     * @param name  [IN] 当前 Body 名称
     * @param value [IN] 当前 Body 的值
     */
    FormTextBody(const std::string &name, const std::string &value);

    PostBodyType GetType() override;

public:

    /**
     * 这个 Body 体的 值
     */
    std::string value_;
};


#endif
