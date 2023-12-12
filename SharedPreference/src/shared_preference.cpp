//
// Created by lixiaoqing on 2021/9/15.
//

#include "shared_preference.h"

#include <algorithm>
#include <sstream>


DbMaster *SharedPreference::database_ = nullptr;
std::recursive_mutex SharedPreference::sp_list_mutex_;
std::vector<SharedPreference *> SharedPreference::sp_list_;


bool SharedPreference::Init(DbMaster *master) {
    if (master == nullptr) {
        return false;
    }

    database_ = master;
    auto sqlExecutor = database_->GetSQLExecutor();
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
    auto isOk = sqlExecutor->Execute(sql);
    return isOk;
}

SharedPreference *SharedPreference::GetSharedPreference(const std::string &name) {
    {
        std::unique_lock<std::recursive_mutex> lock(sp_list_mutex_);

        // 查找是否已经创建过了
        auto it = std::find_if(sp_list_.begin(), sp_list_.end(), [name](SharedPreference *sp) {
            return sp->name_ == name;
        });

        // 如果没有，新增
        if (it == sp_list_.end()) {
            auto sp = new SharedPreference();
            sp->name_ = name;
            sp->ref_count_ = 1;
            sp_list_.push_back(sp);
            return sp;
        }

        // 返回找到的 SP
        return *it;
    }
}

void SharedPreference::With(const std::string &name, const std::function<void(SharedPreference *)> &block) {
    auto sp = SharedPreference::GetSharedPreference(name);
    block(sp);
    sp->Release();
}

std::string SharedPreference::GetString(const std::string &key, const std::string &default_value) {
    if (SharedPreference::database_ == nullptr) {
        return default_value;
    }

    const std::string sql = "SELECT value FROM shared_preference "
                            "WHERE sp_name='" + name_ + "' AND key='" + key + "';";
    auto res = database_->GetSQLExecutor()->Query(sql);
    if (res == nullptr) {
        return default_value;
    }

    if (res->Next()) {
        std::string text = res->ColumnText(0);
        if (text.empty()) {
            return default_value;
        }
        res->Close();
        return text;
    }

    res->Close();
    return default_value;
}

bool SharedPreference::GetBool(const std::string &key, bool default_value) {
    auto ret = this->GetInt(key, 0);
    return (ret == 1);
}

int SharedPreference::GetInt(const std::string &key, int default_value) {
    auto value = this->GetString(key, "");
    if (value.empty()) {
        return default_value;
    }

    try {
        auto ret = std::stoi(value);
        return ret;
    } catch (...) {
        return default_value;
    };
}

float SharedPreference::GetFloat(const std::string &key, float default_value) {
    auto value = this->GetString(key, "");
    if (value.empty()) {
        return default_value;
    }

    try {
        auto ret = std::stof(value);
        return ret;
    } catch (...) {
        return default_value;
    };
}

double SharedPreference::GetDouble(const std::string &key, double default_value) {
    auto value = this->GetString(key, "");
    if (value.empty()) {
        return default_value;
    }

    try {
        auto ret = std::stod(value);
        return ret;
    } catch (...) {
        return default_value;
    }
}

long long SharedPreference::GetLongLong(const std::string &key, long long default_value) {
    auto value = this->GetString(key, "");
    if (value.empty()) {
        return default_value;
    }

    try {
        auto ret = std::stoll(value);
        return ret;
    } catch (...) {
        return default_value;
    }
}

bool SharedPreference::SetString(const std::string &key, const std::string &value) {
    {
        std::unique_lock<std::recursive_mutex> lock(ref_count_mutex_);

        std::ostringstream oss;
        oss << "REPLACE INTO shared_preference (sp_name, key, value, timestamp) VALUES ("
            << "'" << name_ << "', "
            << "'" << key << "', "
            << "'" << value << "', "
            << CurrentTimeMills() << ");";
        if (database_ && database_->GetSQLExecutor()) {
            auto isOk = database_->GetSQLExecutor()->Execute(oss.str());
            return isOk;
        }
        return false;
    }
}

bool SharedPreference::SetBool(const std::string &key, bool value) {
    return SetString(key, value ? "1" : "0");
}

bool SharedPreference::SetInt(const std::string &key, int value) {
    std::ostringstream oss;
    oss << value;
    return SetString(key, oss.str());
}

bool SharedPreference::SetFloat(const std::string &key, float value) {
    std::ostringstream oss;
    oss << value;
    return SetString(key, oss.str());
}

bool SharedPreference::SetDouble(const std::string &key, double value) {
    std::ostringstream oss;
    oss << value;
    return SetString(key, oss.str());
}

bool SharedPreference::SetLongLong(const std::string &key, long long value) {
    std::ostringstream oss;
    oss << value;
    return SetString(key, oss.str());
}

bool SharedPreference::Release() {
    {
        std::unique_lock<std::recursive_mutex> lock(ref_count_mutex_);

        // 引用计数 -1
        --ref_count_;

        // 如果引用计数还大于 0 ，说明还有地方在引用
        if (ref_count_ > 0) {
            return false;
        }

        {
            std::unique_lock<std::recursive_mutex> lockStatic(sp_list_mutex_);

            // 从 SP 列表中移除
            auto it = std::find_if(sp_list_.begin(), sp_list_.end(), [this](SharedPreference *sp) {
                return (this == sp);
            });
            sp_list_.erase(it);
        }

        delete this;
        return true;
    }
}

void SharedPreference::Retain() {
    {
        std::unique_lock<std::recursive_mutex> lock(ref_count_mutex_);
        ++ref_count_;
    }
}

unsigned long long SharedPreference::GetRefCount() const {
    return ref_count_;
}

unsigned long long SharedPreference::CurrentTimeMills() {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    );
    return ms.count();
}