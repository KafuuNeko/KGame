#include <iostream>

#include "logger/logger.hpp"
#include "context/context.hpp"

class Logger: public core::LoggerBase {
    virtual void info(const std::string &message) const override {
        std::cout << message << std::endl;
    }

    virtual void debug(const std::string &message) const override {
        std::cout << message << std::endl;
    }

    virtual void warn(const std::string &message) const override {
        std::cout << message << std::endl;
    }

    virtual void error(const std::string &message) const override {
        std::cout << message << std::endl;
    }
};

int main()
{

    auto context = core::Context::instantiate(std::make_shared<Logger>(), ".");

    if (!context->init()) {
        printf("error\n");
        return 0;
    }

    printf("completed\n");

    return 0;
}

