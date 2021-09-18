#ifndef NetworkClientNeedHeaders
#define NetworkClientNeedHeaders

#include <iostream>
#include <sys/stat.h>
#include "curl/curl.h"
#include "NetworkClient.h"
#include "Request/Requests/GetRequest.h"
#include "Request/Requests/GetStringRequest.h"
#include "Request/Requests/GetFileRequest.h"
#include "Request/Requests/PostRequest.h"
#include "PostBody/PostBodies/FormBody.h"
#include "PostBody/PostBodies/FormFileBody.h"
#include "PostBody/PostBodies/TextBody.h"
#include "PostBody/PostBodies/BinaryBody.h"
#include "PostBody/PostBodies/FormTextBody.h"
#include "Thread/Thread.h"

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
    bool handleByRequestType(CURL *curl, Request *request);

    /**
     * get: 处理返回字符串
     */
    bool handleGetString(CURL *curl, GetStringRequest *request);

    /**
    * get: 处理返回文件
    */
    bool handleGetFile(CURL *curl, GetFileRequest *request);

    /**
    * post: 处理返回字符串
    */
    bool handlePost(CURL *curl, PostRequest *request);

    /**
     * post: 处理表单类的数据
     */
    bool handleFormPost(CURL *curl, PostRequest *request);

    /**
     * post: 处理表单类的数据
     */
    bool handleRawPost(CURL *curl, PostRequest *request);

    /**
     * 请求完成之后的操作（如关闭）
     */
    void afterRequest(CURL *curl, Request *request);

    /**
     * 处理表单类型的 Post 请求，拼接body
     */
    void appendBody(CURL *curl, curl_httppost **first, curl_httppost **last,
                    PostBody *body);

    /**
     * 请求返回值
     * @param buffer 下载回来的数据
     * @param size 要读取的每个元素的大小（字节为单位）
     * @param nmemb 元素的数量，每个元素大小都是字节size
     * @param lpVoid 用户指针，用户通过这个指针传输自己的数据
     */
    size_t onResponse(void *buffer, size_t size, size_t nmemb, void *lpVoid);

    /**
     * 保存下载文件
     */
    size_t writeFile(void *ptr, size_t size, size_t nmemb, void *stream);

    /**
     * 对Url编码
     */
    std::string encodeURL(std::string str);

    /**
     * get请求中对参数的拼接
     */
    std::string splicingParams(GetRequest *request);

    /**
     * debug模式下调试参数
     */
    int OnDebug(CURL *, curl_infotype infoType, char *pData, size_t size, void *_Null_unspecified);


    // ********************************** 具体函数实现 ****************************************

    /**
     * 根据 Request 的类型分别处理
     */
    bool handleByRequestType(CURL *curl, Request *request) {
        if (request->type == REQUEST_TYPE_UNSPECIFIED) {
            return false;
        }

        // GET: 返回字符串
        if (request->type == REQUEST_TYPE_GET_STRING) {
            return handleGetString(curl, (GetStringRequest *) request);
        }

        // GET: 返回文件
        if (request->type == REQUEST_TYPE_GET_FILE) {
            return handleGetFile(curl, (GetFileRequest *) request);
        }

        // POST: 返回字符串
        if (request->type == REQUEST_TYPE_POST) {
            return handlePost(curl, (PostRequest *) request);
        }

        return false;
    }

    /**
    * get: 处理返回字符串
    */
    bool handleGetString(CURL *curl, GetStringRequest *request) {
        // 返回值的处理
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onResponse));
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &request->response));

        // 拼接参数
        std::string url;
        std::string params = splicingParams(request);

        // 如果参数是空的
        if (params.empty()) {
            url = request->url;
        }
            // 参数不为空，拼接时需要考虑 URL 中是否已经自带了「?」
        else {
            auto idx = request->url.find('?');
            // 没有找到 「?」，添加一个 「?」在 url 末尾
            if (idx == std::string::npos) {
                url = request->url + "?" + params;
            }
                // 找到了「?」
            else {
                url = request->url + "&" + params;
            }
        }

        // 编码
        std::string urlEncoded = encodeURL(url);
        // 设置 URL
        return curl_easy_setopt(curl, CURLOPT_URL, urlEncoded.c_str()) == CURLE_OK;
    }

    /**
    * get: 处理返回文件
    */
    bool handleGetFile(CURL *curl, GetFileRequest *request) {
        // 获取已经存在的文件的信息
        curl_off_t offset = 0;

        // 如果需要断点续传，读取已经存在的文件的大小
        if (request->needResume) {
            struct stat fileInfo{};
            int code = stat(request->filePath.c_str(), &fileInfo);
            if (code == 0) {
                offset = fileInfo.st_size;
            }
        }


        FILE *file = fopen(request->filePath.c_str(), "ab+");
        if (!file) {
            return false;
        }

        request->file = file;

        // 拼接参数
        std::string url = request->url + Private::splicingParams(request);
        // 编码
        std::string urlEncode = Private::encodeURL(url);
        // 设置 URL
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_URL, urlEncode.c_str()));

        // 续传位置
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, offset));

        // 写文件
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, file));
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile));

        return curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L) == CURLE_OK;
    }

    /**
    * post: 处理返回字符串
    */
    bool handlePost(CURL *curl, PostRequest *request) {

        // 表单类
        if (request->isFormPost()) {
            Private::handleFormPost(curl, request);
        }
            // 原始数据类
        else {
            Private::handleRawPost(curl, request);
        }

        // 返回值的处理
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onResponse));
        ReturnFalseIfNotOK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &request->response));

        // 设置 URL
        if (request->isPut) {
            return curl_easy_setopt(curl, CURLOPT_URL, request->url.c_str()) == CURLE_OK;
        }
        std::string urlEncode = Private::encodeURL(request->url);
        return curl_easy_setopt(curl, CURLOPT_URL, urlEncode.c_str()) == CURLE_OK;
    }

    /**
     * 处理表单类型的 Post 请求
     */
    bool handleFormPost(CURL *curl, PostRequest *request) {
        // 拼接各个 Body
        struct curl_httppost *firstBody = nullptr;
        struct curl_httppost *lastBody = nullptr;

        size_t size = request->bodies->size();
        for (size_t i = 0; i < size; i++) {
            PostBody *body = request->bodies->at(i);
            appendBody(curl, &firstBody, &lastBody, body);
        }

        // 设置 Body
        return curl_easy_setopt(curl, CURLOPT_HTTPPOST, firstBody) == CURLE_OK;
    }

    /**
     * 处理原始数据类型
     */
    bool handleRawPost(CURL *curl, PostRequest *request) {
        size_t size = request->bodies->size();
        if (size == 0) {
            return true;
        }

        PostBody *body = request->bodies->at(0);

        // 纯文本
        if (body->getType() == POST_BODY_TYPE_RAW_TEXT) {
            auto *textBody = (TextBody *) body;
            // 长度
            ReturnFalseIfNotOK(
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, textBody->text.length()));
            // 数据
            return curl_easy_setopt(curl, CURLOPT_POSTFIELDS, textBody->text.c_str()) ==
                   CURLE_OK;
        }

        // 二进制
        if (body->getType() == POST_BODY_TYPE_RAW_BINARY) {
            auto *binaryBody = (BinaryBody *) body;
            // 长度
            ReturnFalseIfNotOK(
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, binaryBody->dataLength));
            // 数据
            return curl_easy_setopt(curl, CURLOPT_POSTFIELDS, binaryBody->data) == CURLE_OK;
        }

        return false;
    }

    /**
     * 对一个 Post 请求追加 Body 体
     */
    void appendBody(CURL *curl, curl_httppost **first, curl_httppost **last,
                    PostBody *body) {
        // 表单
        if (body->getType() == POST_BODY_TYPE_FORM_TEXT) {
            auto *temp = (FormTextBody *) body;
            curl_formadd(first, last,
                         CURLFORM_COPYNAME, temp->name.c_str(),
                         CURLFORM_COPYCONTENTS, temp->value.c_str(),
                         CURLFORM_CONTENTTYPE, temp->contentType.c_str(),
                         CURLFORM_END);
            return;
        }

        // 文件
        if (body->getType() == POST_BODY_TYPE_FORM_FILE) {
            auto *temp = (FormFileBody *) body;
            curl_formadd(first, last,
                         CURLFORM_COPYNAME, temp->name.c_str(),
                         CURLFORM_FILE, temp->localPath.c_str(),
                         CURLFORM_FILENAME, temp->paramFileName.c_str(),
                         CURLFORM_CONTENTTYPE, temp->contentType.c_str(),
                         CURLFORM_END);
            return;
        }

        // 文本
        if (body->getType() == POST_BODY_TYPE_RAW_TEXT) {
            auto *temp = (TextBody *) body;
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, temp->text.c_str());
            return;
        }

        // 二进制
        if (body->getType() == POST_BODY_TYPE_RAW_BINARY) {
            auto *temp = (BinaryBody *) body;
            curl_formadd(first, last,
//                                         CURLFORM_COPYNAME, temp->name.c_str(),
                         CURLFORM_BUFFERPTR, temp->data,
                         CURLFORM_BUFFERLENGTH, temp->dataLength,
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

    unsigned char toHex(unsigned char x) {
        return static_cast<unsigned char>(x > 9 ? x + 55 : x + 48);
    }

    /**
     * 对 URL 编码
     */
    std::string encodeURL(std::string str) {
        std::string strTemp = std::string();
        size_t length = str.length();
        for (size_t i = 0; i < length; i++) {
            if (isalnum((unsigned char) str[i]) ||
                (str[i] == '-') || (str[i] == '_') ||
                (str[i] == '.') || (str[i] == '/') ||
                (str[i] == ':') || (str[i] == '?') ||
                (str[i] == '&') || (str[i] == '\\') ||
                (str[i] == '=') || (str[i] == '~')) {
                strTemp += str[i];
            } else if (str[i] == ' ') {
                strTemp += "+";
            } else {
                strTemp += '%';
                strTemp += toHex((unsigned char) str[i] >> 4);
                strTemp += toHex((unsigned char) ((unsigned char) str[i] % 16));
            }

        }

        return strTemp;
    }

    /**
     * 将 request 中的参数转为 key1=value1&key2=value2 的形式
     */
    std::string splicingParams(GetRequest *request) {
        if (!request || !request->params) {
            return "";
        }

        if (request->params->empty()) {
            return "";
        }

        std::string params;
        auto it = request->params->begin();
        while (it != request->params->end()) {
            params += it->first + "=" + it->second + "&";
            it++;
        }

        return params;
    }

    /**
     * 追加 Header
     */
    void appendHeaders(CURL *curl, std::vector<std::string> *headers) {
        if (!curl || !headers || headers->empty()) {
            return;
        }

        curl_slist *plist = nullptr;
        size_t size = headers->size();
        for (size_t i = 0; i < size; i++) {
            std::string header = headers->at(i);
            plist = curl_slist_append(plist, header.c_str());
        }

        if (plist) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);
        }
    }

    /**
     * 数据返回时的处理
     */
    size_t onResponse(void *buffer, size_t size, size_t nmemb, void *lpVoid) {
        auto *str = (std::string *) lpVoid;
        if (!str || !buffer) {
            return 0;
        }

        char *pData = (char *) buffer;
        str->append(pData, size * nmemb);

        return size * nmemb;
    }

    /**
     * 保存下载文件
     */
    size_t writeFile(void *ptr, size_t size, size_t nmemb, void *stream) {
        size_t result = fwrite(ptr, size, nmemb, (FILE *) stream);
        return result;
    }

    /**
     * 请求完成之后的操作（如关闭）
     */
    void afterRequest(CURL *curl, Request *request) {
        if (request->type == REQUEST_TYPE_GET_FILE) {
            auto *temp = (GetFileRequest *) request;
            fclose(temp->file);
        }
    }

    /**
     * debug模式下的调试信息
     */
    int OnDebug(CURL *, curl_infotype infoType, char *pData, size_t size, void *_Null_unspecified) {
        if (infoType == CURLINFO_TEXT) {
            std::cout << "[TEXT]" << pData << std::endl;
        } else if (infoType == CURLINFO_HEADER_IN) {
            std::cout << "[HEADER_IN]" << pData << std::endl;
        } else if (infoType == CURLINFO_HEADER_OUT) {
            std::cout << "[HEADER_OUT]" << pData << std::endl;
        } else if (infoType == CURLINFO_DATA_IN) {
            std::cout << "[DATA_IN]" << pData << std::endl;
        } else if (infoType == CURLINFO_DATA_OUT) {
            std::cout << "[DATA_OUT]" << pData << std::endl;
        }
        return 0;
    }
}


/*************************** Network 类函数实现 ***************************/

// 单例对象
static INetworkClient *networkClient = nullptr;

// 单例互斥锁
static std::mutex singletonMutex;


/**
 * 单例实现
 * @return
 */
INetworkClient *INetworkClient::getSingleton() {
    if (networkClient == nullptr) {
        std::unique_lock<std::mutex> lock(singletonMutex);
        if (networkClient == nullptr) {
            auto temp = new NetworkClient();
            networkClient = temp;
        }
    }
    return networkClient;
}

NetworkClient::NetworkClient() {
    mThreadPool = ThreadPool::newThreadPool("Network_Thread_Pool", 3);
}

NetworkClient::~NetworkClient() {
    if (mThreadPool) {
        mThreadPool->shutDownAndDeleteSelf();
        mThreadPool = nullptr;
    }
}

/**
 * 同步请求
 * @param request get/post请求
 * @param isDebug 是否显示日志
 * @return 请求结果 0：成功 1：失败
 */
ResultCode NetworkClient::request(Request *request, bool isDebug) {
    ResultCode resultCode;
    if (!request) {
        resultCode.curlCode = CURLE_FAILED_INIT;
        resultCode.requestResult = SN_RESULT_CODE_FAILED;
        return resultCode;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        resultCode.curlCode = CURLE_FAILED_INIT;
        resultCode.requestResult = SN_RESULT_CODE_FAILED;
        return resultCode;
    }

    // 测试时显示日志
    if (isDebug) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, Private::OnDebug);
    }


    if (request->isPut) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    }

    // 超时时间
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, request->connectTimeoutS);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request->timeOutS);

    // 添加 Header 信息
    Private::appendHeaders(curl, request->getHeaders());

    // 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
    // 如果不设置这个选项，libCurl将会发信号打断这个wait从而导致程序退出。
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    // 根据不同的请求类型，处理各自不同的部分
    bool isSuccess = Private::handleByRequestType(curl, request);
    if (!isSuccess) {
        resultCode.curlCode = CURLE_FAILED_INIT;
        resultCode.requestResult = SN_RESULT_CODE_FAILED;
        return resultCode;
    }

    // 本地抓包时，可以在这里设置代理服务器。流量监控是不是也可以用这种方式？？
    // curl_easy_setopt(curl, CURLOPT_PROXY, "http://30.28.163.235");
    // curl_easy_setopt(curl, CURLOPT_PROXYPORT, 8888);

    // 发起请求
    CURLcode res = curl_easy_perform(curl);
    std::cout << "url:" << request->url << ", 网络请求返回值: res=" << res << std::endl;

    // 如果返回的网络状态有错误，加入到异常列表中
    if (res != CURLE_OK) {
        std::cout << "返回的网络状态异常！" << std::endl;
    }

    curl_easy_cleanup(curl);

    Private::afterRequest(curl, request);

    resultCode.requestResult = (res == CURLE_OK ? SN_RESULT_CODE_SUCCESS : SN_RESULT_CODE_FAILED);
    resultCode.curlCode = res;

    return resultCode;
}

/**
 * 异步请求
 * @param requestObj get/post请求
 * @param callback 回调函数
 * @param isDebug 是否显示日志
 */
void NetworkClient::requestAsyn(Request *requestObj, NetworkCallback callback, bool isDebug) {
    if (!requestObj) {
        return;
    }

    if (!mThreadPool) {
        return;
    }

    // 在线程池中执行
    mThreadPool->execute("NetworkClient::requestAsyn", [this, requestObj, callback] {
        ResultCode code = this->request(requestObj);
        if (callback) {
            callback(requestObj, code);
        }
    });

}


