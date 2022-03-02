/**
 * @file sqlite3.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief sqlite3数据库管理
 * @version 0.2
 * @date 2021-12-12
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef _KAFUU_SQLITE3_HPP_
#define _KAFUU_SQLITE3_HPP_

#ifndef __cplusplus
#error Do not include the hpp header in a c project!
#endif //__cplusplus

extern "C" {
#include "sqlite3.h"
}

#include <cfloat>
#include <cinttypes>

#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <type_traits>

namespace sqlite {

enum class status {
    ok,
    terminal,
    error
};

struct database_connect {
    sqlite3* db;
    status db_status;
    std::recursive_mutex mutex;

    database_connect(sqlite3* db, status db_status)
        : db(db)
        , db_status(db_status)
    {
    }
};

class stmt_manager {
private:
    friend class database_manager;

    friend class transaction_manager;

    std::unique_ptr<sqlite3_stmt, std::function<void(sqlite3_stmt*)>> m_stmt;

    status m_status;

    stmt_manager() noexcept
        : m_stmt(nullptr, [](sqlite3_stmt* stmt) {
            if (stmt != nullptr && sqlite3_finalize(stmt) != SQLITE_OK) {
                std::terminate();
            }
        })
        , m_status(status::error)
    {
    }

    operator bool() const noexcept
    {
        return m_status == status::ok;
    }

    /**
     * @brief stmt构造器
     * 构造器将自动调用sqlite3_prepare_v2
     * @param con sqlite连接信息
     * @param sql prepare的sql
     */
    stmt_manager(const database_connect& connect, std::string_view sql, bool readonly) noexcept
        : stmt_manager()
    {
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(connect.db, sql.data(), sql.size(), &stmt, nullptr);
        m_stmt.reset(stmt);

        m_status = (rc == SQLITE_OK) ? status::ok : status::error;

        //当readonly为true时验证预准备的sql语句是否符合只读条件
        if (m_status == status::ok && readonly && !sqlite3_stmt_readonly(stmt)) {
            m_status = status::error;
        }
    }

    /* bind recur begin */
    template <class Value, class... Args, std::enable_if_t<std::is_integral_v<std::decay_t<Value>>, int32_t> = 0>
    bool bind_recur(int32_t which, const Value value, Args&&... args) const noexcept
    {
        return (sqlite3_bind_int64(m_stmt.get(), which, static_cast<sqlite_int64>(value)) == SQLITE_OK)
            ? bind_recur(which + 1, std::forward<decltype(args)>(args)...)
            : false;
    }

    template <class Value, class... Args, std::enable_if_t<std::is_floating_point_v<std::decay_t<Value>>, int32_t> = 0>
    bool bind_recur(int32_t which, const Value value, Args&&... args) const noexcept
    {
        return (sqlite3_bind_double(m_stmt.get(), which, static_cast<double>(value)) == SQLITE_OK)
            ? bind_recur(which + 1, std::forward<decltype(args)>(args)...)
            : false;
    }

    template <class... Args>
    bool bind_recur(int32_t which, const std::string_view& value, Args&&... args) const noexcept
    {
        return (sqlite3_bind_text(m_stmt.get(), which, value.data(), value.size(), SQLITE_TRANSIENT) == SQLITE_OK)
            ? bind_recur(which + 1, std::forward<decltype(args)>(args)...)
            : false;
    }

    template <class... Args>
    bool bind_recur(const int32_t) const noexcept { return true; }
    /* bind recur end */

public:
    stmt_manager(const stmt_manager&) = delete;
    stmt_manager(const stmt_manager&&) = delete;
    stmt_manager& operator=(const stmt_manager&) = delete;
    stmt_manager& operator=(const stmt_manager&&) = delete;

    ~stmt_manager() = default;

    /**
     * @brief 绑定参数
     * 按参数列表顺序依次绑定参数
     * @param args
     * @return true 绑定成功
     * @return false 绑定失败
     */
    template <class... Args>
    bool bind(Args&&... args) const noexcept
    {
        return (m_status != status::ok) ? false : this->bind_recur(1, std::forward<decltype(args)>(args)...);
    }

    /**
     * @brief 单步执行
     *
     * @return int32_t SQLITE_*
     */
    int32_t step() const noexcept
    {
        return (m_status != status::ok) ? SQLITE_ERROR : sqlite3_step(m_stmt.get());
    }

    /**
     * @brief 手动释放本次查询
     *
     * @return true 释放成功
     * @return false 释放失败
     */
    bool finalize() noexcept
    {
        auto ptr = m_stmt.release();
        if (sqlite3_finalize(ptr) == SQLITE_OK) {
            return true;
        }
        m_stmt.reset(ptr);
        return false;
    }

    int32_t column_int32(const int32_t iCol) const noexcept
    {
        return (m_status != status::ok) ? 0 : static_cast<int32_t>(sqlite3_column_int(m_stmt.get(), iCol));
    }

    int64_t column_int64(const int32_t iCol) const noexcept
    {
        return (m_status != status::ok) ? 0 : static_cast<int64_t>(sqlite3_column_int64(m_stmt.get(), iCol));
    }

    double column_double(const int32_t iCol) const noexcept
    {
        return (m_status != status::ok) ? 0 : sqlite3_column_double(m_stmt.get(), iCol);
    }

    std::string column_text(const int32_t iCol) const noexcept
    {
        return (m_status != status::ok) ? std::string() : std::string(reinterpret_cast<const char*>(sqlite3_column_text(m_stmt.get(), iCol)));
    }

    /**
     * @brief 取列总数
     *
     * @return int32_t
     */
    int32_t column_count() const noexcept
    {
        return (m_status != status::ok) ? 0 : sqlite3_column_count(m_stmt.get());
    }

    /**
     * @brief 取指定列名称
     *
     * @param iCol 列索引
     * @return std::string 名称
     */
    std::string column_name(const int32_t iCol) const noexcept
    {
        return (m_status != status::ok) ? std::string() : std::string(sqlite3_column_name(m_stmt.get(), iCol));
    }
};

/**
 * @brief SQLITE事务处理管理器
 * 通过database::begin_transaction取得管理器
 * 事务处理管理器将启用且有效则自动加锁，并在析构时释放
 * 事务处理期间其它线程的数据库有关执行的操作将堵塞
 */
class transaction_manager {
private:
    friend class database_manager;

    std::shared_ptr<database_connect> m_sqlite;

    explicit transaction_manager(std::shared_ptr<database_connect> connect)
        : m_sqlite(connect)
    {
        if (!m_sqlite || m_sqlite->db_status != status::ok) {
            m_sqlite = nullptr;
            return;
        }

        m_sqlite->mutex.lock();

        if (sqlite3_exec(m_sqlite->db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
            m_sqlite->mutex.unlock();
            m_sqlite = nullptr;
        }
    }

public:
    /**
     * @brief 准备一个sql
     *
     * @param sql
     * @return sqlite_stmt
     */
    stmt_manager prepare(std::string_view sql) const noexcept
    {
        if (!m_sqlite || m_sqlite->db_status != status::ok) {
            return stmt_manager();
        } else {
            return stmt_manager(*m_sqlite, sql, false);
        }
    }

    /**
     * @brief 执行一个或多个SQL语句
     *
     * @param sql sql语句
     * @param callback 回调函数
     * @param pArg callback的第一个参数
     * @param pzErrMsg 错误信息
     * @return true 执行成功
     * @return false 执行失败
     */
    bool exec(const std::string& sql, sqlite3_callback callback = nullptr, void* pArg = nullptr, char** pzErrMsg = nullptr) const noexcept
    {
        if (!m_sqlite) {
            return false;
        }

        return sqlite3_exec(m_sqlite->db, sql.c_str(), callback, pArg, pzErrMsg) == SQLITE_OK;
    }

    /**
     * @brief 带绑定参数执行sql语句
     * 按参数列表顺序依次绑定参数
     *
     * @return true 执行成功
     * @return false 执行失败
     */
    template <class... Args>
    bool singleStep(std::string_view sql, Args&&... args) const noexcept
    {
        if (!m_sqlite) {
            return false;
        }

        auto stmt = stmt_manager(*m_sqlite, sql, false);
        return stmt.bind(std::forward<decltype(args)>(args)...) ? (stmt.step() == SQLITE_DONE) : false;
    }

    /**
     * @brief 回滚事务，撤销所作的更改
     *
     * @return true 执行成功
     * @return false 执行失败
     */
    bool rollback() const noexcept
    {
        return this->exec("ROLLBACK");
    }

    /**
     * @brief 结束并提交事务
     * @return true 成功结束或已经处于结束状态
     * @return false 执行失败
     */
    bool end_transaction() noexcept
    {
        if (this->exec("END TRANSACTION")) {
            m_sqlite->mutex.unlock();
            m_sqlite = nullptr;
            return true;
        }

        return !m_sqlite;
    }

    ~transaction_manager() noexcept
    {
        if (!end_transaction()) {
            std::terminate();
        }
    }

    operator bool() const noexcept
    {
        return static_cast<bool>(m_sqlite);
    }

    transaction_manager(const transaction_manager&) = delete;
    transaction_manager& operator=(const transaction_manager&) = delete;
};

/**
 * @brief SQLITE数据库
 */
class database_manager {
private:
    std::shared_ptr<database_connect> m_sqlite;

public:
    database_manager() = default;

    /**
     * @brief 此构造函数将在构造时调用open打开数据库
     */
    database_manager(const std::string& path, const int flag) noexcept
    {
        this->open(path, flag);
    }

    /**
     * @brief 数据库状态正常时返回true
     * 如果还未打开数据库，或数据库被关闭或错误则返回false
     */
    operator bool() const noexcept
    {
        return m_sqlite ? m_sqlite->db_status == status::ok : false;
    }

    /**
     * @brief 打开数据库
     *
     * @param path 数据库路径
     *
     * @param flag SQLITE_*
     */
    bool open(const std::string& path, const int flag)
    {
        if (m_sqlite && m_sqlite->db_status == status::ok) {
            if (!this->close()) {
                return false;
            }
        }

        sqlite3* db = nullptr;
        int rc = sqlite3_open_v2(path.c_str(), &db, flag, nullptr);

        auto deleter = [](database_connect* connect) {
            if (connect) {
                if (connect->db_status == status::ok) {
                    if (sqlite3_close_v2(connect->db) != SQLITE_OK) {
                        std::terminate();
                    }
                }
                delete connect;
            }
        };
        m_sqlite = std::shared_ptr<database_connect>(new database_connect(db, (rc != SQLITE_OK) ? status::error : status::ok), deleter);

        return m_sqlite->db_status == status::ok;
    }

    /**
     * @brief 开启事务处理
     * 成功开启事务后将开启锁，其它地方的exec将被堵塞
     */
    transaction_manager begin_transaction() const noexcept
    {
        return transaction_manager(m_sqlite);
    }

    /**
     * @brief 执行一个或多个sql
     * 参数以及返回值参照：transaction::exec
    */
    bool exec(const std::string& sql, sqlite3_callback callback = nullptr, void* pArg = nullptr, char** pzErrMsg = nullptr) {
        if (!m_sqlite || m_sqlite->db_status != status::ok) {
            return false;
        }
        std::lock_guard<std::recursive_mutex> lock(m_sqlite->mutex);
        
        return sqlite3_exec(m_sqlite->db, sql.c_str(), callback, pArg, pzErrMsg) == SQLITE_OK;
    }

    /**
     * @brief 执行一个sql
     * 参数以及返回值参照：transaction::singleStep
     */
    template <class... Args>
    bool singleStep(std::string_view sql, Args&&... args) const noexcept
    {
        if (!m_sqlite || m_sqlite->db_status != status::ok) {
            return false;
        }
        std::lock_guard<std::recursive_mutex> lock(m_sqlite->mutex);

        auto stmt = stmt_manager(*m_sqlite, sql, false);
        return stmt.bind(std::forward<decltype(args)>(args)...) ? (stmt.step() == SQLITE_DONE) : false;
    }

    /**
     * @brief 最近完成的SQL语句更改或插入\删除\更新的数据库行数
     * @return 若此数据库状态异常则返回-1
     */
    int32_t changes() const noexcept
    {
        if (!m_sqlite || m_sqlite->db_status != status::ok) {
            return -1;
        }
        return sqlite3_changes(m_sqlite->db);
    }

    /**
     * @brief 返回最后一次插入的rowid
     * @return 若此数据库状态异常则返回-1
     */
    int64_t last_insert_rowid() const noexcept
    {
        if (!m_sqlite || m_sqlite->db_status != status::ok) {
            return -1;
        }

        return sqlite3_last_insert_rowid(m_sqlite->db);
    }

    /**
     * @brief 查询记录
     */
    stmt_manager query(std::string_view table, std::string_view columns = "*", std::string_view other = std::string()) const noexcept
    {
        std::string sql = std::string("SELECT ").append(columns).append(" FROM ").append(table).append(" ").append(other);

        if (!m_sqlite || m_sqlite->db_status != status::ok) {
            return stmt_manager();
        } else {
            return stmt_manager(*m_sqlite, sql, true);
        }
    }

    /**
     * @brief 手动关闭数据库连接
     *
     * @return true 成功关闭
     * @return false 关闭失败
     */
    bool close() noexcept
    {
        if (!m_sqlite) {
            return false;
        }

        std::lock_guard<std::recursive_mutex> _lock_guard(m_sqlite->mutex);
        //若数据库未关闭，则关闭它
        if (m_sqlite->db_status == status::ok) {
            m_sqlite->db_status = (sqlite3_close_v2(m_sqlite->db) == SQLITE_OK) ? status::terminal : m_sqlite->db_status;
        }

        return m_sqlite->db_status == status::terminal;
    }
};
}

#endif /* _KAFUU_SQLITE3_HPP_ */
