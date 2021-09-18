
#include "PostBody/PostBodies/FormFileBody.h"


PostBodyType FormFileBody::getType() {
    return POST_BODY_TYPE_FORM_FILE;
}

FormFileBody::FormFileBody(const std::string &name, const std::string &path, const std::string &paramFileName)
        : FormBody(name), localPath(path), paramFileName(paramFileName) {

}

FormFileBody::~FormFileBody() = default;



