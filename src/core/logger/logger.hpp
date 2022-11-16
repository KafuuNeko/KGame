#pragma once

#include <string_view>
#include <memory>
#include "fmt/format.h"

namespace core {

class LoggerBase {
protected:
    virtual void info(const std::string &message) const = 0;
    virtual void debug(const std::string &message) const = 0;
    virtual void warn(const std::string &message) const = 0;
    virtual void error(const std::string &message) const = 0;

public:
    using SharedPtr = std::shared_ptr<LoggerBase>;

    virtual ~LoggerBase() = default;

    template <typename... Args>
    constexpr void info(std::string_view source, std::string_view message, Args&&... messageArgs) const
    {
        this->info(fmt::format(fmt::format("[{}] {}", source, message), std::forward<Args>(messageArgs)...));
    }

    template <typename... Args>
    constexpr void debug(std::string_view source, std::string_view message, Args&&... messageArgs) const
    {
        this->debug(fmt::format(fmt::format("[{}] {}", source, message), std::forward<Args>(messageArgs)...));
    }

    template <typename... Args>                                                                             
    constexpr void warn(std::string_view source, std::string_view message, Args&&... messageArgs) const
    {
        this->warn(fmt::format(fmt::format("[{}] {}", source, message), std::forward<Args>(messageArgs)...));
    }

    template <typename... Args>
    constexpr void error(std::string_view source, std::string_view message, Args&&... messageArgs) const
    {
        this->error(fmt::format(fmt::format("[{}] {}", source, message), std::forward<Args>(messageArgs)...));
    }
};

}
