//
// Created by lixiaoqing on 2021/9/8.
//

#ifndef SQLITE3_SQLITERESULTSET_H
#define SQLITE3_SQLITERESULTSET_H

#include "SQLiteValue.h"


class SQLiteDatabase;

class SQLiteResultSet {

public:

    bool next();

    void close();

    // 用于执行有前面sqlite3_prepare创建的准备语句
    bool step();

    int columnCount() const;

    double columnDouble(int idx) const;

    int columnInt(int idx) const;

    sqlite3_int64 columnInt64(int idx) const;

    const char *columnText(int idx) const;

    SQLiteBlobValue columnBlob(int idx) const;

private:

    SQLiteResultSet();

    SQLiteResultSet &operator=(const SQLiteResultSet &rhs);

    SQLiteResultSet(const SQLiteResultSet &rhs);

    ~SQLiteResultSet() = default;

    void _closeNeedNoticeDb(bool flag);

    bool isOpen() const;

    void open(SQLiteDatabase *db) { _db = db; }

private:

    friend class SQLiteDatabase;

    SQLiteDatabase *_db;
    sqlite3_stmt *_pStmt;
    bool _cached;
};


inline bool SQLiteResultSet::isOpen() const {
    return (_db);
}


inline int SQLiteResultSet::columnCount() const {
    return sqlite3_column_count(_pStmt);
}


inline double SQLiteResultSet::columnDouble(int idx) const {
    return sqlite3_column_double(_pStmt, idx);
}


inline int SQLiteResultSet::columnInt(int idx) const {
    return sqlite3_column_int(_pStmt, idx);
}


inline sqlite3_int64 SQLiteResultSet::columnInt64(int idx) const {
    return sqlite3_column_int64(_pStmt, idx);
}


inline const char *SQLiteResultSet::columnText(int idx) const {
    return (const char *) sqlite3_column_text(_pStmt, idx);
}


inline SQLiteBlobValue SQLiteResultSet::columnBlob(int idx) const {
    int length = sqlite3_column_bytes(_pStmt, idx);
    const void *bytes = sqlite3_column_blob(_pStmt, idx);
    SQLiteBlobValue value = {bytes, length};
    return value;
}

#endif //SQLITE3_SQLITERESULTSET_H
