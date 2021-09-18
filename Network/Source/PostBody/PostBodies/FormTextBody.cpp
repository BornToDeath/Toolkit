
#include "PostBody/PostBodies/FormTextBody.h"
#include "PostBody/PostBodyType.h"


FormTextBody::FormTextBody(const std::string &key, const std::string &value) : FormBody(key), value(value) {

}

PostBodyType FormTextBody::getType() {
    return POST_BODY_TYPE_FORM_TEXT;
}
