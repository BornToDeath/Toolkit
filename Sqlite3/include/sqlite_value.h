//
// Created by lixiaoqing on 2021/9/8.
//

#ifndef SQLITE3_SQLITEVALUE_H
#define SQLITE3_SQLITEVALUE_H

#include "sqlite3/sqlite3.h"


typedef enum {
    SQLValueType_Null,
    SQLValueType_Double,
    SQLValueType_Text,
    SQLValueType_Blob,
    SQLValueType_Int,
    SQLValueType_Int64,
} SQLValueType;


struct SQLiteBlobValue {
    const void *bytes;
    int length;
};


class SqliteValue {

public:

    bool IsNull() const { return type_ == SQLValueType_Null; }

    bool IsBlob() const { return type_ == SQLValueType_Blob; }

    bool IsDouble() const { return type_ == SQLValueType_Double; }

    bool IsText() const { return type_ == SQLValueType_Text; }

    bool IsInt() const { return type_ == SQLValueType_Int; }

    bool IsInt64() const { return type_ == SQLValueType_Int64; }

    SQLValueType Type() const { return type_; }

    const void *BlobBytes() const { return u_.blob.bytes; }

    int BlobLength() const { return u_.blob.length; }

    double DoubleValue() const { return u_.double_; }

    const char *TextValue() const { return u_.text; }

    int IntValue() const { return u_.int_; }

    sqlite3_int64 Int64Value() const { return u_.int64_; }

    SqliteValue() { type_ = SQLValueType_Null; }

    SqliteValue(SQLValueType type) : type_(type) {}

    SqliteValue &operator=(const SqliteValue &value) {
        type_ = value.type_;
        u_ = value.u_;
        return *this;
    }

private:

    // SqliteValue(SQLValueType type) : type_(type)   {}

    SQLValueType type_;
    union {
        double double_;
        const char *text;
        int int_;
        sqlite3_int64 int64_;
        SQLiteBlobValue blob;
    } u_;

    friend inline SqliteValue SqlDouble(double);

    friend inline SqliteValue SqlInt(int);

    friend inline SqliteValue SqlInt64(sqlite3_int64);

    friend inline SqliteValue SqlText(const char *);

    friend inline SqliteValue SqlBlob(const void *, int length);

    friend inline SqliteValue SqlBlob(const SQLiteBlobValue &);

    friend inline SqliteValue SqlNull();
};


inline SqliteValue SqlNull() {
    return SqliteValue(SQLValueType_Null);
}


inline SqliteValue SqlDouble(double v) {
    SqliteValue tmp(SQLValueType_Double);
    tmp.u_.double_ = v;
    return tmp;
}


inline SqliteValue SqlInt(int v) {
    SqliteValue tmp(SQLValueType_Int);
    tmp.u_.int_ = v;
    return tmp;
}


inline SqliteValue SqlInt64(sqlite3_int64 v) {
    SqliteValue tmp(SQLValueType_Int64);
    tmp.u_.int64_ = v;
    return tmp;
}


inline SqliteValue SqlText(const char *v) {
    SqliteValue tmp(SQLValueType_Text);
    tmp.u_.text = v;
    return tmp;
}


inline SqliteValue SqlBlob(const void *bytes, int length) {
    SqliteValue tmp(SQLValueType_Blob);
    tmp.u_.blob.bytes = bytes;
    tmp.u_.blob.length = length;
    return tmp;
}


inline SqliteValue SqlBlob(const SQLiteBlobValue &blob) {
    SqliteValue tmp(SQLValueType_Blob);
    tmp.u_.blob = blob;
    return tmp;
}


#endif //SQLITE3_SQLITEVALUE_H
