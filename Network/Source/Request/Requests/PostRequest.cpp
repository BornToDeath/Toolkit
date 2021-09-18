
#include "Request/Requests/PostRequest.h"


PostRequest::PostRequest(std::vector<PostBody *> *bodies, std::string &response) : response(response) {
    this->bodies = bodies;
    this->type = REQUEST_TYPE_POST;
}

PostRequest::~PostRequest() {
    if (!bodies) {
        return;
    }

    while (!bodies->empty()) {
        auto *first = bodies->at(0);
        bodies->erase(bodies->begin());
        delete first;
    }
}

bool PostRequest::isFormPost() const {
    if (!bodies) {
        return false;
    }

    size_t size = bodies->size();
    for (size_t i = 0; i < size; i++) {
        if (!isFormBody(bodies->at(i)->getType())) {
            return false;
        }
    }

    return true;
}