#ifndef SMARTEYE_MODULE_POST_REQUEST_H
#define SMARTEYE_MODULE_POST_REQUEST_H

#include "Request/Request.h"
#include "PostBody/PostBody.h"


class PostRequest : public Request {

public:

    /**
     * @param bodies    [IN]  Body体
     * @param response  [OUT] 返回值
     */
    PostRequest(std::vector<PostBody *> *bodies, std::string &response);

    /**
     * 自动析构包含的所有 PostBody。
     */
    ~PostRequest() override;

public:

    /**
     * 是否为表单形式的 Post
     */
    bool isFormPost() const;

public:

    /**
     * 返回值
     */
    std::string &response;

    /**
     * Body 体
     */
    std::vector<PostBody *> *bodies;

};


#endif
