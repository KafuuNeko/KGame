#pragma once

#include <iterator>
#include <string>
#include <vector>

#include "bytes.hpp"
#include "debug.hpp"

namespace utils::chunk {
using BytesT = bytes::BytesT;
class ChunkSet {
private:
    std::vector<BytesT> mChunks;

public:
    /**
     * @brief 解析字节集
     *
     * @tparam Iterator
     * @param first
     * @param last
     * @return chunk
     */
    template <class Iterator>
    static decltype(auto) parser(Iterator first, Iterator last)
    {
        ChunkSet result {};

        uint32_t len = 0;

        for (auto it = first; it != last;) {
            if (len == 0) {
                if (std::distance(it, last) < sizeof(len)) {
                    break;
                }
                it += bytesToInteger(it, len);
            } else {
                if (std::distance(it, last) < len) {
                    break;
                }

                auto buffer = bytes::makeByteBuffer(len);
                std::copy(it, it + len, buffer.begin());
                result.pushBack(std::move(buffer));

                it += len;
                len = 0;
            }
        }

        return result;
    }

    BytesT toBytes() const
    {
        BytesT bytes;
        for (auto& element : this->mChunks) {
            bytes::integerToBytes(static_cast<uint32_t>(element.size()), std::back_inserter(bytes));
            std::copy(element.cbegin(), element.cend(), std::back_inserter(bytes));
        }
        return bytes;
    }

    /**
     * @brief 在尾部推入一个新的数据块在
     *
     * @tparam Bytes
     * @param bytes
     */
    void pushBack(const BytesT& chunk)
    {
        this->mChunks.push_back(chunk);
    }

    void pushBack(BytesT&& chunk)
    {
        this->mChunks.push_back(chunk);
    }

    /**
     * @brief 获取数据块的数量
     *
     * @return size_t
     */
    size_t size() const noexcept { return this->mChunks.size(); }

    bool empty() const noexcept { return this->mChunks.empty(); }

    /**
     * @brief 获取指定数据块数据
     *
     * @param index 数据块索引
     * @return bytes_t&
     */
    BytesT& get(const size_t index) noexcept { return this->mChunks[index]; }
    const BytesT& get(const size_t index) const noexcept { return this->mChunks[index]; }

    BytesT& operator[](const size_t index) noexcept { return this->get(index); }
    const BytesT& operator[](const size_t index) const noexcept { return this->get(index); }

    /**
     * @brief 取得对应迭代器
     *
     */
    decltype(auto) begin() noexcept { return this->mChunks.begin(); }
    decltype(auto) begin() const noexcept { return this->mChunks.cbegin(); }

    decltype(auto) end() noexcept { return this->mChunks.end(); }
    decltype(auto) end() const noexcept { return this->mChunks.cend(); }

    decltype(auto) cbegin() const noexcept { return this->begin(); }
    decltype(auto) cend() const noexcept { return this->end(); }
};

inline std::string toString(const ChunkSet& chunk_set) noexcept
{
    return debug::arraysDescribe(chunk_set.cbegin(), chunk_set.cend(), [](const BytesT& bytes) {
        return debug::arraysDescribe<int32_t>(bytes.cbegin(), bytes.cend());
    });
}
}
