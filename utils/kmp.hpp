#pragma once

/**
 * @file kmp.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief 基于kmp算法实现字符串操作
 * @version 0.1
 * @date 2021-12-12
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <string_view>
#include <vector>

namespace utils::kmp {

using size_t = std::string_view::size_type;

constexpr auto npos = std::string_view::npos;

template<bool Optimize = false>
static std::vector<size_t> makeNextVector(std::string_view substr)
{
    if (substr.empty()) {
        return std::vector<size_t>();
    }

    auto next = std::vector<size_t>(substr.length(), npos);

    size_t j = 0, k = npos;

    while (j < substr.length() - 1) {
        if (k == npos || substr[j] == substr[k]) {
            ++j;

            k = (k == npos) ? 0 : k + 1;

            if constexpr (Optimize) {
                next[j] = (substr[j] == substr[k]) ? next[k] : k;
            } else {
                next[j] = k;
            }

        } else {
            k = next[k];
        }
    }

    return next;
}

static size_t find(std::string_view findstr, std::string_view substr, size_t pos = 0, size_t n = npos)
{
    if (pos >= substr.length()) {
        return npos;
    }
    
    substr = substr.substr(pos, n);

    if (findstr.length() < substr.length()) {
        return npos;
    }
    
    auto next = makeNextVector(substr);

    size_t j = 0, k = 0;
    while (j < findstr.length() && (k == npos || k < substr.length())) {
        if (k == npos || findstr[j] == substr[k]) {
            ++j;
            k = (k == npos) ? 0 : k + 1;
        } else {
            k = next[k];
        }
    }

    if (k == substr.length()) {
        return j - k;
    } else {
        return npos;
    }
}

static size_t count(std::string_view findstr, std::string_view substr, size_t pos = 0, size_t n = npos)
{
    if (pos >= substr.length()) {
        return npos;
    }
    
    substr = substr.substr(pos, n);

    if (findstr.length() < substr.length()) {
        return npos;
    }
    
    auto next = makeNextVector<true>(substr);

    size_t cnt = 0;

    size_t j = 0, k = 0;
    while (j < findstr.length()) {
        if (k == npos || findstr[j] == substr[k]) {
            //指示后移，或匹配成功
            ++j;

            k = (k == npos) ? 0 : k + 1;

            //子串匹配成功，计数+1，并回溯到匹配成功的位置，继续匹配
            if (k == substr.length()) {
                ++cnt;
                k = next[k - 1];
                --j;
            }
        } else {
            k = next[k];
        }
    }

    return cnt;
}

}
