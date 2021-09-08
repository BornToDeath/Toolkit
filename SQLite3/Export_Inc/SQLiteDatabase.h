//
// Created by lixiaoqing on 2021/9/8.
//

#ifndef SQLITE3_SQLITEDATABASE_H
#define SQLITE3_SQLITEDATABASE_H

#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include "SQLiteResultSet.h"
#include "SQLiteValue.h"


class SQLiteDatabase {

public:

    SQLiteDatabase();

    ~SQLiteDatabase();

    static bool isThreadSafe();

    bool open(const char *dbPath);

    bool close();

    bool cacheSQL(const char *sql);

    bool uncacheSQL(const char *sql);

    bool exec(const char *sql);

    bool exec(const char *sql, const SQLiteValue &v0);

    bool exec(const char *sql, const SQLiteValue &v0, const SQLiteValue &v1);

    bool exec(const char *sql, const SQLiteValue *values, int length);

    bool execBatch(const char *sql, const std::vector<SQLiteValue *> &listValues, int length);

    SQLiteResultSet *query(const char *sql);

    SQLiteResultSet *query(const char *sql, const SQLiteValue &v0);

    SQLiteResultSet *query(const char *sql, const SQLiteValue &v0, const SQLiteValue &v1);

    SQLiteResultSet *query(const char *sql, const SQLiteValue *values, int length);

    bool isTableExists(const char *name) const;

    int getTableRows(const char *name) const;

    // 获取数据库文件的版本
    int getVersion() const;

    // 设置数据库文件的版本
    void setVersion(const int version) const;

private:

    SQLiteDatabase(const SQLiteDatabase &rhs);

    SQLiteDatabase &operator=(const SQLiteDatabase &rhs);

    bool databaseExists() const;

    void closeOpenedResultSets();

    void uncacheCachesResultSets();

    void resultSetDidfinalize(SQLiteResultSet *resultSet);

    int numberOfRetries() const;

    void waitForRetry();

    SQLiteResultSet *findClosedCache(const char *sql);

    sqlite3_stmt *stmtPrepare(const char *sql);

    void bindValue(const SQLiteValue &value, int idx, sqlite3_stmt *stmt);

    void bindValues(const SQLiteValue *values, int length, sqlite3_stmt *stmt);


private:

    friend class SQLiteResultSet;

    sqlite3 *_db;
    std::string _dbPath;
    int _numberOfRetries;
    std::vector<SQLiteResultSet *> _openResultSets;
    std::map<std::string, SQLiteResultSet *> _cacheResultSets;
    SQLiteResultSet _reuseResultSet;
};


inline bool SQLiteDatabase::isThreadSafe() {
    return sqlite3_threadsafe();
}


inline bool SQLiteDatabase::databaseExists() const {
    return _db != nullptr;
}


inline int SQLiteDatabase::numberOfRetries() const {
    return _numberOfRetries;
}


inline void SQLiteDatabase::waitForRetry() {
    usleep(20);
}


#endif //SQLITE3_SQLITEDATABASE_H
