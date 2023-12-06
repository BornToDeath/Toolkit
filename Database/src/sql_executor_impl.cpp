//
// Created by lixiaoqing on 2021/9/9.
//

#include "sql_executor_impl.h"


SqlExecutorImpl::SqlExecutorImpl(SqliteDatabase *db) : SqlExecutor() {
    this->db_ = db;
}

bool SqlExecutorImpl::Execute(std::string sql) {
    return this->db_->Exec(sql.c_str());
}

bool SqlExecutorImpl::Execute(std::string sql, SqliteValue values[], int length) {
    return this->db_->Exec(sql.c_str(), values, length);
}

bool SqlExecutorImpl::ExecuteBatch(const std::string &sql, const std::vector<SqliteValue *> &values, int length) {
    return this->db_->ExecBatch(sql.c_str(), values, length);
}

SqliteResultSet *SqlExecutorImpl::Query(const std::string &sql) {
    return this->db_->Query(sql.c_str());
}

SqliteResultSet *SqlExecutorImpl::Query(std::string sql, SqliteValue values[], int length) {
    return this->db_->Query(sql.c_str(), values, length);
}

long long SqlExecutorImpl::QueryCount(const std::string &table_name) {
    auto result_set = this->Query("SELECT count(1) FROM " + table_name + " ;");
    if (result_set && result_set->Next()) {
        auto count = result_set->ColumnInt64(0);
        result_set->Close();
        return count;
    }
    return 0;
}

long long SqlExecutorImpl::QueryCount(const std::string &table_name, const std::string &where_clause) {
    std::string sql = "SELECT count(1) FROM " + table_name;
    if (where_clause.empty()) {
        sql += " ;";
    } else {
        sql += " WHERE " + where_clause + " ;";
    }
    auto result_set = this->Query(sql);
    if (result_set && result_set->Next()) {
        auto count = result_set->ColumnInt64(0);
        result_set->Close();
        return count;
    }
    return 0;
}

SqliteResultSet *SqlExecutorImpl::ListAll(const std::string &table_name) {
    return this->Query("SELECT * FROM " + table_name + " ;");
}

bool SqlExecutorImpl::BeginTransaction() {
    return this->Execute("BEGIN TRANSACTION;");
}

bool SqlExecutorImpl::EndTransaction() {
    return this->Execute("END TRANSACTION;");
}

bool SqlExecutorImpl::RollbackTransaction() {
    return this->Execute("ROLLBACK;");
}