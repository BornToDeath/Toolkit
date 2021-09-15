//
// Created by lixiaoqing on 2021/9/9.
//

#ifndef DATABASE_ISQLEXECUTOR_H
#define DATABASE_ISQLEXECUTOR_H

#include <string>
#include <vector>
#include "SQLiteValue.h"
#include "SQLiteResultSet.h"


/**
 * 抽象的 SQL 执行器。
 *
 * execute、query 等虚函数是由子类去实现的。
 * 基于这两个方法，封装了 listAll、queryCount 等方法。
 */
class ISQLExecutor {

public:

    virtual ~ISQLExecutor() = default;

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    virtual bool execute(std::string sql) = 0;

    /**
     * 执行一个 SQL 语句。
     * 用于更新和删除等场景。
     */
    virtual bool execute(std::string sql, SQLiteValue values[], int length) = 0;

    /**
     * 批量执行一个 SQL 语句（相同的语句，不同的参数）
     * @param sql    SQL 语句
     * @param values SQL语句中「?」 对应的值
     * @param length 这个 SQL 有多少个「?」占位符（SQLiteValue[] 数组的长度，不是 vector 的长度）
     * @return
     */
    virtual bool executeBatch(const std::string &sql, const std::vector<SQLiteValue *> &values, int length) = 0;

    /**
     * 查询一个 SQL 语句
     */
    virtual SQLiteResultSet *query(const std::string &sql) = 0;

    /**
     * 执行一个查询语句
     * @param sql
     * @param values
     * @param length values 的长度（SQL中占位符「?」的个数）
     * @return
     */
    virtual SQLiteResultSet *query(std::string sql, SQLiteValue values[], int length) = 0;

    /**
     * 查询某个表里的总数
     */
    virtual long long queryCount(const std::string &tableName) = 0;

    /**
     * @param tableName   表
     * @param whereClause Where 从句, 不包含 where 关键字。 eg: name = 'zhp'
     */
    virtual long long queryCount(const std::string &tableName, const std::string &whereClause) = 0;

    /**
     * 查询表里所有数据
     */
    virtual SQLiteResultSet *listAll(const std::string &tableName) = 0;

    /**
     * 开启事务
     */
    virtual bool beginTransaction() = 0;

    /**
     * 结束事务
     */
    virtual bool endTransaction() = 0;

    /**
     * 回滚事务
     */
    virtual bool rollbackTransaction() = 0;
};


#endif //DATABASE_ISQLEXECUTOR_H
