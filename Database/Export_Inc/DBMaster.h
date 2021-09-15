//
// Created by lixiaoqing on 2021/9/14.
//

#ifndef DATABASE_DBMASTER_H
#define DATABASE_DBMASTER_H

#include "ISQLExecutor.h"


/**
 * 每个 Master 对应一个数据库文件。
 *
 * 这个类负责打开数据库、保持与数据库的链接、保证线程安全等。
 * 对外提供 ISQLExecutor。
 *
 * @see ISQLExecutor
 */
class DBMaster {

public:

    DBMaster() = default;

    ~DBMaster();

public:

    /**
     * 打开数据库
     * @param dbFilePath
     * @return
     */
    bool open(const std::string &dbFilePath);

    /**
     * 返回一个 SQL 语句执行器，用于执行 SQL 相关操作
     * @return
     */
    ISQLExecutor *getSQLExecutor();

    /**
     * 获取数据库文件的版本
     * @return
     */
    int getVersion() const;

private:

    /**
     * SQL 语句执行器
     */
    ISQLExecutor *sqlExecutor = nullptr;

    /**
     * SQLite3 的原始数据库对象
     */
    SQLiteDatabase *db = nullptr;

    /**
     * 标识数据库对象是否已经打开了
     */
    bool isOpened = false;

};


#endif //DATABASE_DBMASTER_H
