#include "request/requests/post_request.h"

PostRequest::PostRequest(std::vector<PostBody *> *bodies, std::string &response) : response_(response) {
    this->bodies_ = bodies;
    this->type_ = REQUEST_TYPE_POST;
}

PostRequest::~PostRequest() {
    if (!bodies_) {
        return;
    }

    while (!bodies_->empty()) {
        auto *first = bodies_->at(0);
        bodies_->erase(bodies_->begin());
        delete first;
    }
}

bool PostRequest::IsFormPost() const {
    if (!bodies_) {
        return false;
    }

    size_t size = bodies_->size();
    for (size_t i = 0; i < size; i++) {
        if (!IsFormBody(bodies_->at(i)->GetType())) {
            return false;
        }
    }

    return true;
}