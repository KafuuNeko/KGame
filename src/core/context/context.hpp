#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "context_fwd.hpp"
#include "logger/logger.hpp"
#include "manager_base.hpp"
#include "sqlite/sqlite3.hpp"

namespace core {

enum class GameStatus {
    uninitialized,
    running,
    shutoff,
    error
};

/**
 * @brief 游戏环境
 *
 */
class Context
    : public std::enable_shared_from_this<Context> {
private:
    //游戏根目录
    std::string mRootDir;

    //运行状态
    GameStatus mGameStatus;

    //游戏数据库
    sqlite::database_manager mGameDatabase;

    LoggerBase::SharedPtr mLogger;

    explicit Context(const LoggerBase::SharedPtr &logger, std::string_view rootDir);

    bool initDirectories() noexcept;
    bool initDBStruct() noexcept;

public:
    static std::shared_ptr<Context> instantiate(const LoggerBase::SharedPtr &logger, std::string_view rootDir);

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    bool init();
    bool close();

    /**
     * 使得当前游戏处于不可恢复的错误状态
     */
    template <typename... Args>
    void fatalError(std::string_view source, std::string_view message, Args&&... messageArgs) noexcept
    {
        mGameDatabase.close();
        mGameStatus = GameStatus::error;
        mLogger->error(source, message, std::forward<Args>(messageArgs)...);
    }

    /**
     * @brief 取得游戏运行状态
     * @see enum class GameStatus
     */
    GameStatus getStatus() const noexcept
    {
        return mGameStatus;
    }

    /**
     * @brief 判断是否是正在运行状态
     * @see GameRunningStatus
     */
    bool isRunning() const noexcept
    {
        return getStatus() == GameStatus::running;
    }

    /**
     * @brief 取得游戏数据库
     */
    const sqlite::database_manager getGameDB() const noexcept
    {
        return mGameDatabase;
    }
    sqlite::database_manager getGameDB() noexcept
    {
        return mGameDatabase;
    }

    /**
     * @brief 取得对应管理器
     */
    template <class Manager, std::enable_if_t<std::is_convertible_v<Manager*, ManagerBase*>, int> = 0>
    std::unique_ptr<Manager> getManager()
    {
        return std::make_unique<Manager>(shared_from_this());
    }
};

}
