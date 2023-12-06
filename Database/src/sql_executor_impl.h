//
// Created by lixiaoqing on 2021/9/9.
//

#ifndef DATABASE_SQL_EXECUTOR_IMPL_H
#define DATABASE_SQL_EXECUTOR_IMPL_H

#include "sql_executor.h"
#include "sqlite_database.h"


/**
 * SqlExecutor 的实现类
 */
class SqlExecutorImpl : public SqlExecutor {

public:

    explicit SqlExecutorImpl(SqliteDatabase *db);

    ~SqlExecutorImpl() override = default;

public:

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    bool Execute(std::string sql) override;

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    bool Execute(std::string sql, SqliteValue values[], int length) override;

    /**
     * 批量执行一个 SQL 语句（相同的语句，不同的参数）
     * @param sql    SQL 语句
     * @param values SQL语句中「?」 对应的值
     * @param length 这个 SQL 有多少个「?」占位符（SqliteValue[] 数组的长度，不是 vector 的长度）
     * @return
     */
    bool ExecuteBatch(const std::string &sql, const std::vector<SqliteValue *> &values, int length) override;

    /**
     * 查询一个 SQL 语句
     */
    SqliteResultSet *Query(const std::string &sql) override;

    /**
     * 执行一个查询语句
     * @param sql
     * @param values
     * @param length values 的长度（SQL中占位符「?」的个数）
     * @return
     */
    SqliteResultSet *Query(std::string sql, SqliteValue values[], int length) override;

    /**
     * 查询某个表里的总数
     */
    long long QueryCount(const std::string &table_name) override;

    /**
     * @param table_name   表
     * @param where_clause Where 从句, 不包含 where 关键字
     */
    long long QueryCount(const std::string &table_name, const std::string &where_clause) override;

    /**
     * 查询表里所有数据
     */
    SqliteResultSet *ListAll(const std::string &table_name) override;

    /**
     * 开启事务
     */
    bool BeginTransaction() override;

    /**
     * 结束事务
     */
    bool EndTransaction() override;

    /**
     * 回滚事务
     */
    bool RollbackTransaction() override;

private:

    /**
     * 数据库句柄
     */
    SqliteDatabase *db_{nullptr};
};


#endif //DATABASE_SQL_EXECUTOR_IMPL_H
