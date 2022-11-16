#pragma once

/**
 * @file bytes.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief 字节操作集
 * @version 0.1
 * @date 2021-09-27
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <cinttypes>
#include <string>
#include <type_traits>
#include <vector>

namespace utils::bytes {

using ByteT = uint8_t;
using BytesT = std::vector<ByteT>;
/**
 * @brief 开辟指定大小的缓冲区
 *
 * @param size
 * @return buffer_t
 */
inline BytesT makeByteBuffer(const size_t size)
{
    return BytesT(size, 0);
}

inline BytesT toBytes(const std::string& value)
{
    BytesT bytes;
    for (size_t i = 0; i < value.length(); ++i) {
        bytes.push_back(static_cast<ByteT>(value[i]));
    }
    return bytes;
}

/**
 * @brief 字节流还原整数
 * 将自动推断Output类型尺寸，并将基于字节流还原整数结果
 * @tparam InputIterator 字节迭代器
 * @tparam Output 输出
 * @tparam std::enable_if<std::is_integral<Output>::value, int32_t>::type
 * @param input
 * @param output
 * @return size_t Output类型所占字节数
 */
template <class InputIterator,
    class Output,
    std::enable_if_t<std::is_integral_v<Output>, int> = 0,
    std::enable_if_t<std::is_integral_v<std::remove_reference_t<decltype(*(InputIterator {}))>>, int> = 0>
size_t bytesToInteger(InputIterator input, Output& output)
{
    constexpr size_t size = sizeof(Output);

    output = 0;
    for (size_t i = 0; i < size; ++i) {
        auto byteValue = static_cast<uint8_t>(*input);
        output = (output << 8) | byteValue;
        ++input;
    }

    return size;
}

/**
 * @brief 整数转字节流
 * 将自动推断Value类型所占字节数，并从高位到低位输出每位字节
 * @tparam Value
 * @tparam OutputIterator
 * @param value 整数值
 * @param output 输出迭代器
 *
 * @return Value类型所占字节数
 */
template <class Value,
    class OutputIterator,
    std::enable_if_t<std::is_integral_v<Value>, int> = 0>
size_t integerToBytes(const Value value, OutputIterator output)
{
    constexpr size_t size = sizeof(Value);
    constexpr size_t bitCount = (size * 8);

    for (size_t i = 1; i <= size; ++i) {
        *output = (value >> (bitCount - i * 8)) & 0xFF;
        ++output;
    }

    return size;
}

}
