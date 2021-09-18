#ifndef SMARTEYE_MODULE_FILE_BODY_H
#define SMARTEYE_MODULE_FILE_BODY_H

#include "FormBody.h"


/**
 * 文件 Body 体
 */
class FormFileBody : public FormBody {

public:

    /**
     * @param name          [IN] Body 对应的 name 字段
     * @param paramFileName [IN] Body 对应的 filename 字段
     * @param localPath     [IN] 文件的本地路径
     */
    FormFileBody(const std::string &name, const std::string &localPath, const std::string &paramFileName);

    ~FormFileBody() override;

    PostBodyType getType() override;

public:

    /**
     * 本地文件路径
     */
    std::string localPath;

    /**
     * 请求中的文件名
     */
    std::string paramFileName;

};

#endif
