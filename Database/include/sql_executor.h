//
// Created by lixiaoqing on 2021/9/9.
//

#ifndef DATABASE_SQL_EXECUTOR_H
#define DATABASE_SQL_EXECUTOR_H

#include <string>
#include <vector>
#include "sqlite_value.h"
#include "sqlite_result_set.h"


/**
 * 抽象的 SQL 执行器。
 *
 * execute、Query 等虚函数是由子类去实现的。
 * 基于这两个方法，封装了 ListAll、QueryCount 等方法。
 */
class SqlExecutor {

public:

    virtual ~SqlExecutor() = default;

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    virtual bool Execute(std::string sql) = 0;

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    virtual bool Execute(std::string sql, SqliteValue values[], int length) = 0;

    /**
     * 批量执行一个 SQL 语句（相同的语句，不同的参数）
     * @param sql    SQL 语句
     * @param values SQL语句中「?」 对应的值
     * @param length 这个 SQL 有多少个「?」占位符（SqliteValue[] 数组的长度，不是 vector 的长度）
     * @return
     */
    virtual bool ExecuteBatch(const std::string &sql, const std::vector<SqliteValue *> &values, int length) = 0;

    /**
     * 查询一个 SQL 语句
     */
    virtual SqliteResultSet *Query(const std::string &sql) = 0;

    /**
     * 执行一个查询语句
     * @param sql
     * @param values
     * @param length values 的长度（SQL中占位符「?」的个数）
     * @return
     */
    virtual SqliteResultSet *Query(std::string sql, SqliteValue values[], int length) = 0;

    /**
     * 查询某个表里的总数
     */
    virtual long long QueryCount(const std::string &table_name) = 0;

    /**
     * @param table_name   表
     * @param where_clause Where 从句, 不包含 where 关键字。 eg: name = 'zhangsan'
     */
    virtual long long QueryCount(const std::string &table_name, const std::string &where_clause) = 0;

    /**
     * 查询表里所有数据
     */
    virtual SqliteResultSet *ListAll(const std::string &table_name) = 0;

    /**
     * 开启事务
     */
    virtual bool BeginTransaction() = 0;

    /**
     * 结束事务
     */
    virtual bool EndTransaction() = 0;

    /**
     * 回滚事务
     */
    virtual bool RollbackTransaction() = 0;
};


#endif //DATABASE_SQL_EXECUTOR_H
