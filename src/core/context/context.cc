#include "context.hpp"

#include <cinttypes>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>
#include <filesystem>
#include <vector>
#include <fstream>

#include "cppcrc/encoder.hh"
#include "cppcrc/code_crc32.hh"

using namespace core;

Context::Context(const LoggerBase::SharedPtr &logger, std::string_view rootDir)
    : mLogger(logger), mRootDir(rootDir)
{
}

std::shared_ptr<Context> Context::instantiate(const LoggerBase::SharedPtr &logger, std::string_view rootDir)
{
    return std::shared_ptr<Context>(new Context(logger, rootDir));
}

bool Context::initDirectories() noexcept
{
    std::error_code ec;
    
    std::vector<std::filesystem::path> dirs {fmt::format("{}/db", mRootDir), fmt::format("{}/cache/journal", mRootDir)};

    for (auto &dir : dirs) {
        if (!std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir, ec)) {
                fatalError("Context::initDirectories", ec.message());
                return false;
            }
        }

        if (!std::filesystem::is_directory(dir)) {
            fatalError("Context::initDirectories", "directory path is occupied by an unknown file: {}", dir.u8string());
            return false;
        }
    }

    return true;
}

bool Context::initDBStruct() noexcept {

    constexpr std::pair<std::string_view, uint32_t> files[] = {
        {"game.sql", 1422224050}
    };


    for (auto &file : files) {
        std::ifstream ifs(fmt::format("{}/db/struct/{}", mRootDir, file.first), std::ios::binary);
        if (!ifs) {
            fatalError("Context::initDBStruct", "Unable to open {}", file.first);
            return false;
        }

        std::string sql(std::istreambuf_iterator<char>(ifs), {});
        
        auto crcValue = crc::Encoder(crc::CodeCrc32::instance(), sql).value();

        ifs.close();

        if (crcValue != file.second) {
            fatalError("Context::initDBStruct", "CRC check of {} fails. crc value: {}", file.first, crcValue);
            return false;
        } else {
            mLogger->debug("Context::initDBStruct", "{} passes the CRC check ", file.first);
        }

        if (!mGameDatabase.begin_transaction().exec(sql)) {
            fatalError("Context::initDBStruct", "{} fails to be executed", file.first);
            return false;
        } else {
            mLogger->debug("Context::initDBStruct", "exec {} completed", file.first);
        }
    }
 

    return true;
}

bool Context::init()
{
    if (isRunning()) {
        mLogger->debug("Context::init", "running...");
        return true;
    }

    //The error state cannot be recovered
    if (mGameStatus == GameStatus::error) {
        mLogger->error("Context::init", "error state cannot be recovered");
        return false;
    }

    if (!initDirectories()) {
        return false;
    }
    
    if (!mGameDatabase.open(fmt::format("{}/db/game.db", mRootDir), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)) {
        fatalError("Context::start", "failed to open database");
        return false;
    }

    if (!initDBStruct()) {
        mLogger->error("Context::init", "error init database struct");
        return false;
    }

    if (mGameDatabase.begin_transaction().exec("PRAGMA foreign_keys = ON;")) {
        mGameStatus = GameStatus::running;
    } else {
        fatalError("Context::start", "exec 'PRAGMA foreign_keys = ON' failure");
    }

    return isRunning();
}

bool Context::close()
{
    if (isRunning()) {
        mGameStatus = mGameDatabase.close() ? GameStatus::shutoff : mGameStatus;
    }
    return mGameStatus == GameStatus::shutoff;
}



