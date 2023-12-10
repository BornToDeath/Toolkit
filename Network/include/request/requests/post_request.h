#ifndef NETWORK_POST_REQUEST_H
#define NETWORK_POST_REQUEST_H

#include "post_body/post_body.h"
#include "request/request.h"


class PostRequest : public Request {

public:

    /**
     * @param bodies    [IN]  Body体
     * @param response  [OUT] 返回值
     */
    PostRequest(std::vector<PostBody *> *bodies, std::string &response);

    /**
     * 自动析构包含的所有 post_body。
     */
    ~PostRequest() override;

public:

    /**
     * 是否为表单形式的 Post
     */
    bool IsFormPost() const;

public:

    /**
     * 返回值
     */
    std::string &response_;

    /**
     * Body 体
     */
    std::vector<PostBody *> *bodies_;

};


#endif
