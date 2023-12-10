#include "post_body/post_bodies/form_file_body.h"

PostBodyType FormFileBody::GetType() {
    return POST_BODY_TYPE_FORM_FILE;
}

FormFileBody::FormFileBody(const std::string &name, const std::string &local_path, const std::string &param_filename)
        : FormBody(name), local_path_(local_path), param_filename_(param_filename) {

}

FormFileBody::~FormFileBody() = default;



