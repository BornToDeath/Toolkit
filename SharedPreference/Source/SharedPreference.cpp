//
// Created by lixiaoqing on 2021/9/15.
//

#include <algorithm>
#include <sstream>
#include "SharedPreference.h"


DBMaster *SharedPreference::database = nullptr;
std::recursive_mutex SharedPreference::staticMutex;
std::vector<SharedPreference *> SharedPreference::spList;


bool SharedPreference::init(DBMaster *master) {
    if (master == nullptr) {
        return false;
    }

    database = master;
    auto sqlExecutor = database->getSQLExecutor();
    if (sqlExecutor == nullptr) {
        return false;
    }

    // 创建 sp 数据库表
    const std::string sql = "CREATE TABLE IF NOT EXISTS shared_preference ("
                            "sp_name text NOT NULL, "
                            "key text, "
                            "value text, "
                            "timestamp int, "
                            "PRIMARY KEY(sp_name, key)"
                            ");";
    auto isOk = sqlExecutor->execute(sql);
    return isOk;
}

SharedPreference *SharedPreference::getSharedPreference(const std::string &name) {
    {
        std::unique_lock<std::recursive_mutex> lock(staticMutex);

        // 查找是否已经创建过了
        auto it = std::find_if(spList.begin(), spList.end(), [name](SharedPreference *sp) {
            return sp->name == name;
        });

        // 如果没有，新增
        if (it == spList.end()) {
            auto sp = new SharedPreference();
            sp->name = name;
            sp->refCount = 1;
            spList.push_back(sp);
            return sp;
        }

        // 返回找到的 SP
        return *it;
    }
}

void SharedPreference::with(const std::string &name, const std::function<void(SharedPreference *)>& block) {
    auto sp = SharedPreference::getSharedPreference(name);
    block(sp);
    sp->release();
}

std::string SharedPreference::getString(const std::string &key, const std::string &defValue) {
    if (SharedPreference::database == nullptr) {
        return defValue;
    }

    const std::string sql = "SELECT value FROM shared_preference "
                            "WHERE sp_name='" + name + "' AND key='" + key + "';";
    auto res = database->getSQLExecutor()->query(sql);
    if (res == nullptr) {
        return defValue;
    }

    if (res->next()) {
        std::string text = res->columnText(0);
        if (text.empty()) {
            return defValue;
        }
        res->close();
        return text;
    }

    res->close();
    return defValue;
}

bool SharedPreference::getBool(const std::string &key, bool defValue) {
    auto ret = this->getInt(key, 0);
    return (ret == 1);
}

int SharedPreference::getInt(const std::string &key, int defValue) {
    auto value = this->getString(key, "");
    if (value.empty()) {
        return defValue;
    }

    try {
        auto ret = std::stoi(value);
        return ret;
    } catch (...) {
        return defValue;
    };
}

float SharedPreference::getFloat(const std::string &key, float defValue) {
    auto value = this->getString(key, "");
    if (value.empty()) {
        return defValue;
    }

    try {
        auto ret = std::stof(value);
        return ret;
    } catch (...) {
        return defValue;
    };
}

double SharedPreference::getDouble(const std::string &key, double defValue) {
    auto value = this->getString(key, "");
    if (value.empty()) {
        return defValue;
    }

    try {
        auto ret = std::stod(value);
        return ret;
    } catch (...) {
        return defValue;
    }
}

long long SharedPreference::getLongLong(const std::string &key, long long defValue) {
    auto value = this->getString(key, "");
    if (value.empty()) {
        return defValue;
    }

    try {
        auto ret = std::stoll(value);
        return ret;
    } catch (...) {
        return defValue;
    }
}

bool SharedPreference::setString(const std::string &key, const std::string &value) {
    {
        std::unique_lock<std::recursive_mutex> lock(mutex);

        std::ostringstream oss;
        oss << "REPLACE INTO shared_preference (sp_name, key, value, timestamp) VALUES ("
            << "'" << name << "', "
            << "'" << key << "', "
            << "'" << value << "', "
            << currentTimeMills() << ");";
        if (database && database->getSQLExecutor()) {
            auto isOk = database->getSQLExecutor()->execute(oss.str());
            return isOk;
        }
        return false;
    }
}

bool SharedPreference::setBool(const std::string &key, bool value) {
    return setString(key, value ? "1" : "0");
}

bool SharedPreference::setInt(const std::string &key, int value) {
    std::ostringstream oss;
    oss << value;
    return setString(key, oss.str());
}

bool SharedPreference::setFloat(const std::string &key, float value) {
    std::ostringstream oss;
    oss << value;
    return setString(key, oss.str());
}

bool SharedPreference::setDouble(const std::string &key, double value) {
    std::ostringstream oss;
    oss << value;
    return setString(key, oss.str());
}

bool SharedPreference::setLongLong(const std::string &key, long long value) {
    std::ostringstream oss;
    oss << value;
    return setString(key, oss.str());
}

bool SharedPreference::release() {
    {
        std::unique_lock<std::recursive_mutex> lock(mutex);

        // 引用计数 -1
        --refCount;

        // 如果引用计数还大于 0 ，说明还有地方在引用
        if (refCount > 0) {
            return false;
        }

        {
            std::unique_lock<std::recursive_mutex> lockStatic(staticMutex);

            // 从 SP 列表中移除
            auto it = std::find_if(spList.begin(), spList.end(), [this](SharedPreference *sp) {
                return (this == sp);
            });
            spList.erase(it);
        }

        delete this;
        return true;
    }
}

void SharedPreference::retain() {
    {
        std::unique_lock<std::recursive_mutex> lock(mutex);
        ++refCount;
    }
}

unsigned long long SharedPreference::getRefCount() const {
    return refCount;
}

unsigned long long SharedPreference::currentTimeMills() {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}