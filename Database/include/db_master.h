//
// Created by lixiaoqing on 2021/9/14.
//

#ifndef DATABASE_DB_MASTER_H
#define DATABASE_DB_MASTER_H

#include "sql_executor.h"


/**
 * 每个 Master 对应一个数据库文件。
 *
 * 这个类负责打开数据库、保持与数据库的链接、保证线程安全等。
 * 对外提供 SqlExecutor。
 *
 * @see SqlExecutor
 */
class DbMaster {

public:

    DbMaster() = default;

    ~DbMaster();

public:

    /**
     * 打开数据库
     * @param db_file_path
     * @return
     */
    bool Open(const std::string &db_file_path);

    /**
     * 返回一个 SQL 语句执行器，用于执行 SQL 相关操作
     * @return
     */
    SqlExecutor *GetSQLExecutor();

    /**
     * 获取数据库文件的版本
     * @return
     */
    int GetVersion() const;

private:

    /**
     * SQL 语句执行器
     */
    SqlExecutor *sql_executor_{nullptr};

    /**
     * Sqlite3 的原始数据库对象
     */
    SqliteDatabase *db_{nullptr};

    /**
     * 标识数据库对象是否已经打开了
     */
    bool is_opened_{false};

};


#endif //DATABASE_DB_MASTER_H
