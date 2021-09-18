#ifndef SMARTEYE_MODULE_POST_FORM_BODY_H
#define SMARTEYE_MODULE_POST_FORM_BODY_H

#include <string>
#include "PostBodyType.h"


/**
 * Post 的 Body 体
 */
class PostBody {

protected:

    PostBody() = default;

public:

    virtual ~PostBody() = default;

public:

    /**
     * 获取 Body 的类型
     */
    virtual PostBodyType getType() = 0;
};


#endif
