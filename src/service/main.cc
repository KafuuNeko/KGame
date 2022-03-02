#include <iostream>

#include "logger/logger.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include "context/context.hpp"


int main()
{
    core::logger::info("Service", "Initializing core ...");

    auto context = core::Context::instantiate(".");

    if (!context->init()) {
        core::logger::error("Service", "Initialization failure");
        return 0;
    }

    core::logger::info("Service", "Initialization completed");

    return 0;
}

