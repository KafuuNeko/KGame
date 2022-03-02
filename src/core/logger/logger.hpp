#pragma once

#include <string_view>

#include "spdlog/fmt/fmt.h"
#include "spdlog/formatter.h"

#include "spdlog/logger.h"
#include "spdlog/spdlog.h"

namespace core::logger {

#define DEFINE_LOG(Name)                                                                           \
    template <typename... Args>                                                                    \
    constexpr void Name(std::string_view source, std::string_view message, Args&&... messageArgs)  \
    {                                                                                              \
        spdlog::Name(fmt::format("[{}] {}", source, message), std::forward<Args>(messageArgs)...); \
    }

DEFINE_LOG(info)
DEFINE_LOG(debug)
DEFINE_LOG(warn)
DEFINE_LOG(error)

#undef LOG

}
