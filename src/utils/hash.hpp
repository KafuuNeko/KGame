#pragma once

/**
 * @file algorithm.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief 算法类工具集
 * @version 0.1
 * @date 2021-09-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <inttypes.h>
#include <type_traits>

namespace utils::hash
{
    /**
     * @brief 计算16位hash
     * 
     * @tparam Iterator 
     * @param first 
     * @param last 
     * @return uint16_t 
     */
    template <class Iterator, std::enable_if_t<std::is_integral_v<std::remove_reference_t<decltype(*(Iterator {}))>>, int> = 0>
    uint16_t hashU16(Iterator first, Iterator last) noexcept
    {
        constexpr auto mod = static_cast<uint16_t>(-1);
        constexpr uint32_t primes[] = {3, 5, 7, 11, 13};

        uint16_t value = 0xFF;
        size_t prime_index = 0;

        for (auto it = first; it != last; ++it)
        {
            auto element = *it;

            value = (static_cast<uint32_t>(value) * primes[prime_index] + element) % mod;
            value = value ^ element;

            prime_index = (prime_index + 1) % (sizeof(primes) / sizeof(*primes));
        }

        return value;
    }
}
