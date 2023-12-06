//
// Created by lixiaoqing on 2021/9/8.
//

#ifndef SQLITE3_SQLITEDATABASE_H
#define SQLITE3_SQLITEDATABASE_H

#include <map>
#include <string>
#include <vector>

#include <unistd.h>

#include "sqlite_result_set.h"
#include "sqlite_value.h"


class SqliteDatabase {

public:

    SqliteDatabase();

    ~SqliteDatabase();

    static bool IsThreadSafe();

    bool Open(const char *db_path);

    bool Close();

    bool CacheSql(const char *sql);

    bool UncacheSql(const char *sql);

    bool Exec(const char *sql);

    bool Exec(const char *sql, const SqliteValue &v0);

    bool Exec(const char *sql, const SqliteValue &v0, const SqliteValue &v1);

    bool Exec(const char *sql, const SqliteValue *values, int length);

    bool ExecBatch(const char *sql, const std::vector<SqliteValue *> &listValues, int length);

    SqliteResultSet *Query(const char *sql);

    SqliteResultSet *Query(const char *sql, const SqliteValue &v0);

    SqliteResultSet *Query(const char *sql, const SqliteValue &v0, const SqliteValue &v1);

    SqliteResultSet *Query(const char *sql, const SqliteValue *values, int length);

    bool IsTableExists(const char *name) const;

    int GetTableRows(const char *name) const;

    // 获取数据库文件的版本
    int GetVersion() const;

    // 设置数据库文件的版本
    void SetVersion(const int version) const;

private:

    SqliteDatabase(const SqliteDatabase &rhs);

    SqliteDatabase &operator=(const SqliteDatabase &rhs);

    bool DatabaseExists() const;

    void CloseOpenedResultSets();

    void UncacheCachesResultSets();

    void ResultSetDidfinalize(SqliteResultSet *result_set);

    int NumberOfRetries() const;

    void WaitForRetry();

    SqliteResultSet *FindClosedCache(const char *sql);

    sqlite3_stmt *StmtPrepare(const char *sql);

    void BindValue(const SqliteValue &value, int idx, sqlite3_stmt *stmt);

    void BindValues(const SqliteValue *values, int length, sqlite3_stmt *stmt);


private:

    friend class SqliteResultSet;

    sqlite3 *db_;
    std::string dbPath_;
    int number_of_retries_;
    std::vector<SqliteResultSet *> open_result_sets_;
    std::map<std::string, SqliteResultSet *> cache_result_sets_;
    SqliteResultSet reuse_result_set_;
};


inline bool SqliteDatabase::IsThreadSafe() {
    return sqlite3_threadsafe();
}


inline bool SqliteDatabase::DatabaseExists() const {
    return db_ != nullptr;
}


inline int SqliteDatabase::NumberOfRetries() const {
    return number_of_retries_;
}


inline void SqliteDatabase::WaitForRetry() {
    usleep(20);
}


#endif //SQLITE3_SQLITEDATABASE_H
