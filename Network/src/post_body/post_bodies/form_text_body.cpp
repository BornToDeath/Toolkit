#include "post_body/post_bodies/form_text_body.h"
#include "post_body/post_body_type.h"

FormTextBody::FormTextBody(const std::string &key, const std::string &value) : FormBody(key), value_(value) {

}

PostBodyType FormTextBody::GetType() {
    return POST_BODY_TYPE_FORM_TEXT;
}
