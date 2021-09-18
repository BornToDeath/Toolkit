#ifndef SMARTEYE_MODULE_FORM_TEXT_BODY_H
#define SMARTEYE_MODULE_FORM_TEXT_BODY_H

#include "FormBody.h"


class FormTextBody : public FormBody {

public:

    /**
     * @param name  [IN] 当前 Body 名称
     * @param value [IN] 当前 Body 的值
     */
    FormTextBody(const std::string &name, const std::string &value);

    PostBodyType getType() override;

public:

    /**
     * 这个 Body 体的 值
     */
    std::string value;
};


#endif
