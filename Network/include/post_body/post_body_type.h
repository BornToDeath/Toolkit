#ifndef NETWORK_POST_BODY_TYPE_H
#define NETWORK_POST_BODY_TYPE_H


/**
 * Post 请求体 的类型
 */
enum PostBodyType {

    /**
     * 未指定
     */
    POST_BODY_TYPE_UNSPECIFIED = 0,

    /**
     * 表单  普通的键值对
     */
    POST_BODY_TYPE_FORM_TEXT = 1,

    /**
     * 表单  文件
     */
    POST_BODY_TYPE_FORM_FILE = 2,

    /**
     * 文本，例如 Json 串
     */
    POST_BODY_TYPE_RAW_TEXT = 3,

    /**
     * 二进制数据
     */
    POST_BODY_TYPE_RAW_BINARY = 4
};

inline bool IsFormBody(PostBodyType type) {
    return type == POST_BODY_TYPE_FORM_FILE || type == POST_BODY_TYPE_FORM_TEXT;
}


#endif
