//
// Created by lixiaoqing on 2021/9/9.
//

#ifndef DATABASE_SQLEXECUTOR_H
#define DATABASE_SQLEXECUTOR_H

#include "ISQLExecutor.h"
#include "SQLiteDatabase.h"


/**
 * ISQLExecutor 的实现类
 */
class SQLExecutor : public ISQLExecutor {

public:

    explicit SQLExecutor(SQLiteDatabase *db);

    ~SQLExecutor() override = default;

public:

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    bool execute(std::string sql) override;

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    bool execute(std::string sql, SQLiteValue *values, int length) override;

    /**
     * 批量执行一个 SQL 语句（相同的语句，不同的参数）
     * @param sql    SQL 语句
     * @param values SQL语句中「?」 对应的值
     * @param length 这个 SQL 有多少个「?」占位符（SQLiteValue[] 数组的长度，不是 vector 的长度）
     * @return
     */
    bool executeBatch(const std::string &sql, const std::vector<SQLiteValue *> &values, int length) override;

    /**
     * 查询一个 SQL 语句
     */
    SQLiteResultSet *query(const std::string& sql) override;

    /**
     * 执行一个查询语句
     * @param sql
     * @param values
     * @param length values 的长度（SQL中占位符「?」的个数）
     * @return
     */
    SQLiteResultSet * query(std::string sql, SQLiteValue *values, int length) override;

    /**
     * 查询某个表里的总数
     */
    long long queryCount(const std::string &tableName) override;

    /**
     * @param tableName   表
     * @param whereClause Where 从句, 不包含 where 关键字
     */
    long long queryCount(const std::string &tableName, const std::string &whereClause) override;

    /**
     * 查询表里所有数据
     */
    SQLiteResultSet *listAll(const std::string &tableName) override;

    /**
     * 开启事务
     */
    bool beginTransaction() override;

    /**
     * 结束事务
     */
    bool endTransaction() override;

    /**
     * 回滚事务
     */
    bool rollbackTransaction() override;

private:

    /**
     * 数据库句柄
     */
    SQLiteDatabase *db;
};


#endif //DATABASE_SQLEXECUTOR_H
