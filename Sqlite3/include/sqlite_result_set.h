//
// Created by lixiaoqing on 2021/9/8.
//

#ifndef SQLITE3_SQLITERESULTSET_H
#define SQLITE3_SQLITERESULTSET_H

#include "sqlite_value.h"


class SqliteDatabase;

class SqliteResultSet {

public:

    bool Next();

    void Close();

    // 用于执行有前面sqlite3_prepare创建的准备语句
    bool Step();

    int ColumnCount() const;

    double ColumnDouble(int idx) const;

    int ColumnInt(int idx) const;

    sqlite3_int64 ColumnInt64(int idx) const;

    const char *ColumnText(int idx) const;

    SQLiteBlobValue ColumnBlob(int idx) const;

private:

    SqliteResultSet();

    SqliteResultSet &operator=(const SqliteResultSet &rhs);

    SqliteResultSet(const SqliteResultSet &rhs);

    ~SqliteResultSet() = default;

    void CloseNeedNoticeDb(bool flag);

    bool IsOpen() const;

    void Open(SqliteDatabase *db) { db_ = db; }

private:

    friend class SqliteDatabase;

    SqliteDatabase *db_{nullptr};
    sqlite3_stmt *stmt_{nullptr};
    bool cached_;
};


inline bool SqliteResultSet::IsOpen() const {
    return db_ != nullptr;
}


inline int SqliteResultSet::ColumnCount() const {
    return sqlite3_column_count(stmt_);
}


inline double SqliteResultSet::ColumnDouble(int idx) const {
    return sqlite3_column_double(stmt_, idx);
}


inline int SqliteResultSet::ColumnInt(int idx) const {
    return sqlite3_column_int(stmt_, idx);
}


inline sqlite3_int64 SqliteResultSet::ColumnInt64(int idx) const {
    return sqlite3_column_int64(stmt_, idx);
}


inline const char *SqliteResultSet::ColumnText(int idx) const {
    return (const char *) sqlite3_column_text(stmt_, idx);
}


inline SQLiteBlobValue SqliteResultSet::ColumnBlob(int idx) const {
    int length = sqlite3_column_bytes(stmt_, idx);
    const void *bytes = sqlite3_column_blob(stmt_, idx);
    SQLiteBlobValue value = {bytes, length};
    return value;
}

#endif //SQLITE3_SQLITERESULTSET_H
