#ifndef NETWORK_POST_BODY_H
#define NETWORK_POST_BODY_H

#include <string>

#include "post_body_type.h"


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
    virtual PostBodyType GetType() = 0;
};


#endif
