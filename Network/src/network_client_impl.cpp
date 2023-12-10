#ifndef NetworkClientNeedHeaders
#define NetworkClientNeedHeaders

#include "network_client_impl.h"

#include <iostream>

#include <sys/stat.h>

#include "curl/curl.h"

#include "post_body/post_bodies/binary_body.h"
#include "post_body/post_bodies/form_body.h"
#include "post_body/post_bodies/form_file_body.h"
#include "post_body/post_bodies/form_text_body.h"
#include "post_body/post_bodies/text_body.h"
#include "request/requests/get_file_request.h"
#include "request/requests/get_request.h"
#include "request/requests/get_string_request.h"
#include "request/requests/post_request.h"
#include "thread/thread.h"

#endif

/**
 * 私有的函数
 */
namespace Private {

#define ReturnFalseIfNotOK(x) if (x != CURLE_OK) { return false; }

// ********************************** 函数定义 ****************************************

/**
 * 根据 Request 的类型分别处理
 */
bool HandleByRequestType(CURL *curl, Request *request);

/**
 * get: 处理返回字符串
 */
bool HandleGetString(CURL *curl, GetStringRequest *request);

/**
* get: 处理返回文件
*/
bool HandleGetFile(CURL *curl, GetFileRequest *request);

/**
* Post: 处理返回字符串
*/
bool HandlePost(CURL *curl, PostRequest *request);

/**
 * Post: 处理表单类的数据
 */
bool HandleFormPost(CURL *curl, PostRequest *request);

/**
 * Post: 处理表单类的数据
 */
bool HandleRawPost(CURL *curl, PostRequest *request);

/**
 * 请求完成之后的操作（如关闭）
 */
void AfterRequest(CURL *curl, Request *request);

/**
 * 处理表单类型的 Post 请求，拼接body
 */
void AppendBody(CURL *curl, curl_httppost **first, curl_httppost **last, PostBody *body);

/**
 * 请求返回值
 * @param buffer 下载回来的数据
 * @param size 要读取的每个元素的大小（字节为单位）
 * @param nmemb 元素的数量，每个元素大小都是字节size
 * @param lpVoid 用户指针，用户通过这个指针传输自己的数据
 */
size_t OnResponse(void *buffer, size_t size, size_t nmemb, void *lpVoid);

/**
 * 保存下载文件
 */
size_t WriteFile(void *ptr, size_t size, size_t nmemb, void *stream);

/**
 * 对Url编码
 */
std::string EncodeURL(std::string str);

/**
 * get请求中对参数的拼接
 */
std::string SplicingParams(GetRequest *request);

/**
 * debug模式下调试参数
 */
int OnDebug(CURL *, curl_infotype info_type, char *data, size_t size, void *_Null_unspecified);


// ********************************** 具体函数实现 ****************************************

/**
 * 根据 Request 的类型分别处理
 */
bool HandleByRequestType(CURL *curl, Request *request) {
    if (request->type_ == REQUEST_TYPE_UNSPECIFIED) {
        return false;
    }

    // GET: 返回字符串
    if (request->type_ == REQUEST_TYPE_GET_STRING) {
        return HandleGetString(curl, (GetStringRequest *) request);
    }

    // GET: 返回文件
    if (request->type_ == REQUEST_TYPE_GET_FILE) {
        return HandleGetFile(curl, (GetFileRequest *) request);
    }

    // POST: 返回字符串
    if (request->type_ == REQUEST_TYPE_POST) {
        return HandlePost(curl, (PostRequest *) request);
    }

    return false;
}

/**
* get: 处理返回字符串
*/
bool HandleGetString(CURL *curl, GetStringRequest *request) {
    // 返回值的处理
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnResponse));
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &request->response_));

    // 拼接参数
    std::string url;
    std::string params = SplicingParams(request);

    // 如果参数是空的
    if (params.empty()) {
        url = request->url_;
    }
        // 参数不为空，拼接时需要考虑 URL 中是否已经自带了「?」
    else {
        auto idx = request->url_.find('?');
        // 没有找到 「?」，添加一个 「?」在 url 末尾
        if (idx == std::string::npos) {
            url = request->url_ + "?" + params;
        }
            // 找到了「?」
        else {
            url = request->url_ + "&" + params;
        }
    }

    // 编码
    std::string urlEncoded = EncodeURL(url);
    // 设置 URL
    return curl_easy_setopt(curl, CURLOPT_URL, urlEncoded.c_str()) == CURLE_OK;
}

/**
* get: 处理返回文件
*/
bool HandleGetFile(CURL *curl, GetFileRequest *request) {
    // 获取已经存在的文件的信息
    curl_off_t offset = 0;

    // 如果需要断点续传，读取已经存在的文件的大小
    if (request->need_resume_) {
        struct stat fileInfo{};
        int code = stat(request->filepath_.c_str(), &fileInfo);
        if (code == 0) {
            offset = fileInfo.st_size;
        }
    }


    FILE *file = fopen(request->filepath_.c_str(), "ab+");
    if (!file) {
        return false;
    }

    request->file_ = file;

    // 拼接参数
    std::string url = request->url_ + Private::SplicingParams(request);
    // 编码
    std::string urlEncode = Private::EncodeURL(url);
    // 设置 URL
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_URL, urlEncode.c_str()));

    // 续传位置
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, offset));

    // 写文件
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, file));
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFile));

    return curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L) == CURLE_OK;
}

/**
* Post: 处理返回字符串
*/
bool HandlePost(CURL *curl, PostRequest *request) {

    // 表单类
    if (request->IsFormPost()) {
        Private::HandleFormPost(curl, request);
    }
        // 原始数据类
    else {
        Private::HandleRawPost(curl, request);
    }

    // 返回值的处理
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnResponse));
    ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &request->response_));

    // 设置 URL
    if (request->is_put_) {
        return curl_easy_setopt(curl, CURLOPT_URL, request->url_.c_str()) == CURLE_OK;
    }
    std::string urlEncode = Private::EncodeURL(request->url_);
    return curl_easy_setopt(curl, CURLOPT_URL, urlEncode.c_str()) == CURLE_OK;
}

/**
 * 处理表单类型的 Post 请求
 */
bool HandleFormPost(CURL *curl, PostRequest *request) {
    // 拼接各个 Body
    struct curl_httppost *firstBody = nullptr;
    struct curl_httppost *lastBody = nullptr;

    size_t size = request->bodies_->size();
    for (size_t i = 0; i < size; i++) {
        PostBody *body = request->bodies_->at(i);
        AppendBody(curl, &firstBody, &lastBody, body);
    }

    // 设置 Body
    return curl_easy_setopt(curl, CURLOPT_HTTPPOST, firstBody) == CURLE_OK;
}

/**
 * 处理原始数据类型
 */
bool HandleRawPost(CURL *curl, PostRequest *request) {
    size_t size = request->bodies_->size();
    if (size == 0) {
        return true;
    }

    PostBody *body = request->bodies_->at(0);

    // 纯文本
    if (body->GetType() == POST_BODY_TYPE_RAW_TEXT) {
        auto *textBody = (TextBody *) body;
        // 长度
        ReturnFalseIfNotOK(
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, textBody->text_.length()));
        // 数据
        return curl_easy_setopt(curl, CURLOPT_POSTFIELDS, textBody->text_.c_str()) ==
               CURLE_OK;
    }

    // 二进制
    if (body->GetType() == POST_BODY_TYPE_RAW_BINARY) {
        auto *binaryBody = (BinaryBody *) body;
        // 长度
        ReturnFalseIfNotOK(
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, binaryBody->data_length_));
        // 数据
        return curl_easy_setopt(curl, CURLOPT_POSTFIELDS, binaryBody->data_) == CURLE_OK;
    }

    return false;
}

/**
 * 对一个 Post 请求追加 Body 体
 */
void AppendBody(CURL *curl, curl_httppost **first, curl_httppost **last,
                PostBody *body) {
    // 表单
    if (body->GetType() == POST_BODY_TYPE_FORM_TEXT) {
        auto *temp = (FormTextBody *) body;
        curl_formadd(first, last,
                     CURLFORM_COPYNAME, temp->name_.c_str(),
                     CURLFORM_COPYCONTENTS, temp->value_.c_str(),
                     CURLFORM_CONTENTTYPE, temp->content_type_.c_str(),
                     CURLFORM_END);
        return;
    }

    // 文件
    if (body->GetType() == POST_BODY_TYPE_FORM_FILE) {
        auto *temp = (FormFileBody *) body;
        curl_formadd(first, last,
                     CURLFORM_COPYNAME, temp->name_.c_str(),
                     CURLFORM_FILE, temp->local_path_.c_str(),
                     CURLFORM_FILENAME, temp->param_filename_.c_str(),
                     CURLFORM_CONTENTTYPE, temp->content_type_.c_str(),
                     CURLFORM_END);
        return;
    }

    // 文本
    if (body->GetType() == POST_BODY_TYPE_RAW_TEXT) {
        auto *temp = (TextBody *) body;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, temp->text_.c_str());
        return;
    }

    // 二进制
    if (body->GetType() == POST_BODY_TYPE_RAW_BINARY) {
        auto *temp = (BinaryBody *) body;
        curl_formadd(first, last,
//                                         CURLFORM_COPYNAME, temp->name.c_str(),
                     CURLFORM_BUFFERPTR, temp->data_,
                     CURLFORM_BUFFERLENGTH, temp->data_length_,
//                                         CURLFORM_CONTENTTYPE, temp->contentType.c_str(),
                     CURLFORM_END);


//                            curl_formadd(&post, &last,
//                                     CURLFORM_COPYNAME, "file",
//                                     CURLFORM_BUFFER, "unnamed.png",
//                                     CURLFORM_BUFFERPTR, memblock,
//                                     CURLFORM_BUFFERLENGTH, memblock_length,
//                                     CURLFORM_CONTENTTYPE, "image/png",
//                                     CURLFORM_END);
    }

}

unsigned char ToHex(unsigned char x) {
    return static_cast<unsigned char>(x > 9 ? x + 55 : x + 48);
}

/**
 * 对 URL 编码
 */
std::string EncodeURL(std::string str) {
    std::string temp_str = std::string();
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (isalnum((unsigned char) str[i]) ||
            (str[i] == '-') || (str[i] == '_') ||
            (str[i] == '.') || (str[i] == '/') ||
            (str[i] == ':') || (str[i] == '?') ||
            (str[i] == '&') || (str[i] == '\\') ||
            (str[i] == '=') || (str[i] == '~')) {
            temp_str += str[i];
        } else if (str[i] == ' ') {
            temp_str += "+";
        } else {
            temp_str += '%';
            temp_str += ToHex((unsigned char) str[i] >> 4);
            temp_str += ToHex((unsigned char) ((unsigned char) str[i] % 16));
        }

    }

    return temp_str;
}

/**
 * 将 Request 中的参数转为 key1=value1&key2=value2 的形式
 */
std::string SplicingParams(GetRequest *request) {
    if (!request || !request->params_) {
        return "";
    }

    if (request->params_->empty()) {
        return "";
    }

    std::string params;
    auto it = request->params_->begin();
    while (it != request->params_->end()) {
        params += it->first + "=" + it->second + "&";
        it++;
    }

    return params;
}

/**
 * 追加 Header
 */
void AppendHeaders(CURL *curl, std::vector<std::string> *headers) {
    if (!curl || !headers || headers->empty()) {
        return;
    }

    curl_slist *curl_list = nullptr;
    size_t size = headers->size();
    for (size_t i = 0; i < size; i++) {
        std::string header = headers->at(i);
        curl_list = curl_slist_append(curl_list, header.c_str());
    }

    if (curl_list) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_list);
    }
}

/**
 * 数据返回时的处理
 */
size_t OnResponse(void *buffer, size_t size, size_t nmemb, void *lpVoid) {
    auto *str = (std::string *) lpVoid;
    if (!str || !buffer) {
        return 0;
    }

    char *data = (char *) buffer;
    str->append(data, size * nmemb);

    return size * nmemb;
}

/**
 * 保存下载文件
 */
size_t WriteFile(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t result = fwrite(ptr, size, nmemb, (FILE *) stream);
    return result;
}

/**
 * 请求完成之后的操作（如关闭）
 */
void AfterRequest(CURL *curl, Request *request) {
    if (request->type_ == REQUEST_TYPE_GET_FILE) {
        auto *temp = (GetFileRequest *) request;
        fclose(temp->file_);
    }
}

/**
 * debug模式下的调试信息
 */
int OnDebug(CURL *, curl_infotype info_type, char *data, size_t size, void *_Null_unspecified) {
    if (info_type == CURLINFO_TEXT) {
        std::cout << "[TEXT]" << data << std::endl;
    } else if (info_type == CURLINFO_HEADER_IN) {
        std::cout << "[HEADER_IN]" << data << std::endl;
    } else if (info_type == CURLINFO_HEADER_OUT) {
        std::cout << "[HEADER_OUT]" << data << std::endl;
    } else if (info_type == CURLINFO_DATA_IN) {
        std::cout << "[DATA_IN]" << data << std::endl;
    } else if (info_type == CURLINFO_DATA_OUT) {
        std::cout << "[DATA_OUT]" << data << std::endl;
    }
    return 0;
}
}


/*************************** Network 类函数实现 ***************************/

// 单例对象
static NetworkClient *g_network_client = nullptr;

// 单例互斥锁
static std::mutex g_singleton_mutex;


/**
 * 单例实现
 * @return
 */
NetworkClient *NetworkClient::GetSingleton() {
    if (g_network_client == nullptr) {
        std::unique_lock<std::mutex> lock(g_singleton_mutex);
        if (g_network_client == nullptr) {
            auto temp = new NetworkClientImpl();
            g_network_client = temp;
        }
    }
    return g_network_client;
}

NetworkClientImpl::NetworkClientImpl() {
    thread_pool_ = ThreadPool::NewInstance("Network_Thread_Pool", 3);
}

NetworkClientImpl::~NetworkClientImpl() {
    if (thread_pool_) {
        thread_pool_->ShutDownAndDeleteSelf();
        thread_pool_ = nullptr;
    }
}

/**
 * 同步请求
 * @param request get/post请求
 * @param is_debug 是否显示日志
 * @return 请求结果 0：成功 1：失败
 */
ResultCode NetworkClientImpl::Request(class Request *request, bool is_debug) {
    ResultCode result_code{};
    if (!request) {
        result_code.curl_code_ = CURLE_FAILED_INIT;
        result_code.request_result_ = SN_RESULT_CODE_FAILED;
        return result_code;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        result_code.curl_code_ = CURLE_FAILED_INIT;
        result_code.request_result_ = SN_RESULT_CODE_FAILED;
        return result_code;
    }

    // 测试时显示日志
    if (is_debug) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, Private::OnDebug);
    }


    if (request->is_put_) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    }

    // 超时时间
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, request->connect_timeout_sec_);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request->timeout_sec_);

    // 添加 Header 信息
    Private::AppendHeaders(curl, request->GetHeaders());

    // 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
    // 如果不设置这个选项，libCurl将会发信号打断这个wait从而导致程序退出。
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    // 根据不同的请求类型，处理各自不同的部分
    bool is_success = Private::HandleByRequestType(curl, request);
    if (!is_success) {
        result_code.curl_code_ = CURLE_FAILED_INIT;
        result_code.request_result_ = SN_RESULT_CODE_FAILED;
        return result_code;
    }

    // 本地抓包时，可以在这里设置代理服务器。流量监控是不是也可以用这种方式？？
    // curl_easy_setopt(curl, CURLOPT_PROXY, "http://30.28.163.235");
    // curl_easy_setopt(curl, CURLOPT_PROXYPORT, 8888);

    // 发起请求
    CURLcode res = curl_easy_perform(curl);
    std::cout << "url:" << request->url_ << ", 网络请求返回值: res=" << res << std::endl;

    // 如果返回的网络状态有错误，加入到异常列表中
    if (res != CURLE_OK) {
        std::cout << "返回的网络状态异常！" << std::endl;
    }

    curl_easy_cleanup(curl);

    Private::AfterRequest(curl, request);

    result_code.request_result_ = (res == CURLE_OK ? SN_RESULT_CODE_SUCCESS : SN_RESULT_CODE_FAILED);
    result_code.curl_code_ = res;

    return result_code;
}

/**
 * 异步请求
 * @param request get/post请求
 * @param callback 回调函数
 * @param is_debug 是否显示日志
 */
void NetworkClientImpl::RequestAsync(class Request *request, NetworkCallback callback, bool is_debug) {
    if (!request) {
        return;
    }

    if (!thread_pool_) {
        return;
    }

    // 在线程池中执行
    thread_pool_->Execute("NetworkClient::RequestAsync", [this, request, callback] {
        ResultCode code = this->Request(request);
        if (callback) {
            callback(request, code);
        }
    });

}


