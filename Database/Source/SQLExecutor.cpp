//
// Created by lixiaoqing on 2021/9/9.
//

#include "SQLExecutor.h"


SQLExecutor::SQLExecutor(SQLiteDatabase *db) : ISQLExecutor() {
    this->db = db;
}

bool SQLExecutor::execute(std::string sql) {
    return this->db->exec(sql.c_str());
}

bool SQLExecutor::execute(std::string sql, SQLiteValue *values, int length) {
    return this->db->exec(sql.c_str(), values, length);
}

bool SQLExecutor::executeBatch(const std::string &sql, const std::vector<SQLiteValue *> &values, int length) {
    return this->db->execBatch(sql.c_str(), values, length);
}

SQLiteResultSet *SQLExecutor::query(const std::string &sql) {
    return this->db->query(sql.c_str());
}

SQLiteResultSet *SQLExecutor::query(std::string sql, SQLiteValue *values, int length) {
    return this->db->query(sql.c_str(), values, length);
}

long long SQLExecutor::queryCount(const std::string &tableName) {
    auto resultSet = this->query("SELECT count(1) FROM " + tableName + " ;");
    if (resultSet && resultSet->next()) {
        auto count = resultSet->columnInt64(0);
        resultSet->close();
        return count;
    }
    return 0;
}

long long SQLExecutor::queryCount(const std::string &tableName, const std::string &whereClause) {
    std::string sql = "SELECT count(1) FROM " + tableName;
    if (whereClause.empty()) {
        sql += " ;";
    } else {
        sql += " WHERE " + whereClause + " ;";
    }
    auto resultSet = this->query(sql);
    if (resultSet && resultSet->next()) {
        auto count = resultSet->columnInt64(0);
        resultSet->close();
        return count;
    }
    return 0;
}

SQLiteResultSet *SQLExecutor::listAll(const std::string &tableName) {
    return this->query("SELECT * FROM " + tableName + " ;");
}

bool SQLExecutor::beginTransaction() {
    return this->execute("BEGIN TRANSACTION;");
}

bool SQLExecutor::endTransaction() {
    return this->execute("END TRANSACTION;");
}

bool SQLExecutor::rollbackTransaction() {
    return this->execute("ROLLBACK;");
}