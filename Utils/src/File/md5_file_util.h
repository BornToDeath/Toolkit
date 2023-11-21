//
// Created by lixiaoqing on 2021/6/22.
//

#ifndef UTILS_MD5_FILE_UTIL_H
#define UTILS_MD5_FILE_UTIL_H

namespace utils {
namespace file_util {

/**
 * 文件 MD5 工具类
 */
namespace md5_file_util {

/**
 * 获取文件 MD5 值
 * @param filepath
 * @param md5Length
 */
char *GetFileMD5(const char *filepath, int md5_len = 32);

// 仅限内部使用
namespace internal {

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
    unsigned int i[2];          /* number of _bits_ handled mod 2^64 */
    unsigned int buf[4];        /* scratch buffer */
    unsigned char in[64];       /* input buffer */
    unsigned char digest[16];   /* actual digest after MD5Final call */
} MD5_CTX;

void MD5Init(MD5_CTX *md_context);

void MD5Update(MD5_CTX *md_context, unsigned char *in_buf, unsigned int in_len);

void MD5Final(MD5_CTX *md_context);

void Transform(unsigned int *buf, unsigned int *in);

}  // namespace internal
}  // namespace md5_file_util
}  // namespace file_util
}  // namespace utils

#endif //UTILS_MD5_FILE_UTIL_H
