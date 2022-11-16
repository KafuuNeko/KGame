#pragma once

/**
 * @file debug.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief 调试工具集
 * @version 0.1
 * @date 2021-09-27
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <string>

namespace utils::debug {

template <class ElementType, class Iterator>
std::string arraysDescribe(Iterator first, Iterator last)
{
    std::string describe("{ ");

    for (auto it = first; it != last; ++it) {
        if (it != first) {
            describe.append(", ");
        }

        describe.append(std::to_string(static_cast<ElementType>(*it)));
    }

    describe.append(" }");
    return describe;
}

template <class ElementToString, class Iterator>
std::string arraysDescribe(Iterator first, Iterator last, ElementToString toStringFunction)
{
    std::string describe("{ ");

    for (auto it = first; it != last; ++it) {
        if (it != first) {
            describe.append(", ");
        }

        describe.append(toStringFunction(*it));
    }

    describe.append(" }");
    return describe;
}

}
