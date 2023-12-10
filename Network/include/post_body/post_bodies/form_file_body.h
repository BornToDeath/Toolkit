#ifndef NETWORK_FILE_BODY_H
#define NETWORK_FILE_BODY_H

#include "form_body.h"


/**
 * 文件 Body 体
 */
class FormFileBody : public FormBody {

public:

    /**
     * @param name          [IN] Body 对应的 name 字段
     * @param param_filename [IN] Body 对应的 filename 字段
     * @param local_path     [IN] 文件的本地路径
     */
    FormFileBody(const std::string &name, const std::string &local_path, const std::string &param_filename);

    ~FormFileBody() override;

    PostBodyType GetType() override;

public:

    /**
     * 本地文件路径
     */
    std::string local_path_;

    /**
     * 请求中的文件名
     */
    std::string param_filename_;

};

#endif
