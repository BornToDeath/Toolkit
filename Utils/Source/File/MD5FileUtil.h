//
// Created by lixiaoqing on 2021/6/22.
//

#ifndef UTILS_MD5FILEUTIL_H
#define UTILS_MD5FILEUTIL_H


/* Data structure for MD5 (Message Digest) computation */
typedef struct {
    unsigned int i[2];          /* number of _bits_ handled mod 2^64 */
    unsigned int buf[4];        /* scratch buffer */
    unsigned char in[64];       /* input buffer */
    unsigned char digest[16];   /* actual digest after MD5Final call */
} MD5_CTX;


/**
 * 文件 MD5 工具类
 */
class MD5FileUtil final {

public:

    MD5FileUtil() = default;

    ~MD5FileUtil() = default;

public:

    /**
     * 获取文件 MD5 值
     * @param filePath
     * @param md5Length
     */
    static char *getFileMD5(const char *filePath, int md5_len = 32);

private:

    static void MD5Init(MD5_CTX *mdContext);

    static void MD5Update(MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);

    static void MD5Final(MD5_CTX *mdContext);

    static void transform(unsigned int *buf, unsigned int *in);

};


#endif //UTILS_MD5FILEUTIL_H
