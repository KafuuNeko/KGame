#pragma once

/**
 * @file packet.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief 适用于网络通信的数据包工具
 * @version 0.1
 * @date 2021-09-27
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <algorithm>
#include <iterator>
#include <memory>
#include <stdio.h>
#include <variant>
#include <vector>

#include "bytes.hpp"
#include "debug.hpp"

namespace utils::packet {
using ByteT = bytes::ByteT;
using BytesT = bytes::BytesT;

// 定义数据封包最大字节数（数据头(16) + 数据(n)）
constexpr size_t MAX_DATAPACK_SIZE = 4096;

/**
 * @brief 计算数据crc，用于校验数据包是否正确接收
 *
 */
template <class Iterator, std::enable_if_t<std::is_integral_v<std::remove_reference_t<decltype(*(Iterator {}))>>, int> = 0>
uint16_t evalCrcSick(Iterator first, Iterator last)
{
    uint16_t crc;

    uint16_t lowByte;
    uint16_t highByte;

    uint16_t shortCur;
    uint16_t shortPre;

    crc = 0;
    shortPre = 0;

    while (first != last) {
        shortCur = 0x00FF & static_cast<uint16_t>(*first++);

        crc = (crc & 0x8000) ? ((crc << 1) ^ 0x8005) : (crc << 1);

        crc ^= (shortCur | shortPre);
        shortPre = shortCur << 8;
    }

    lowByte = (crc & 0xFF00) >> 8;
    highByte = (crc & 0x00FF) << 8;
    crc = lowByte | highByte;

    return crc;
}

/**
 * @brief 数据头
 * 每个完整的数据包由[数据头][数据]组成
 */
class PacketHead {
    friend class Packet;
    template <typename T>
    friend class Unpacker;

private:
    constexpr static auto kHeadSize = 16;

    uint32_t mVersion; // 协议版本

    uint16_t mOperation; // 操作码
    uint16_t mTag; // 操作标识

    uint32_t mDataSize; // 数据长度

    uint16_t mHeadCrc; // 数据头校验
    uint16_t mDataCrc; // 数据校验

    /**
     * @brief 基于字节迭代器构造一个数据头
     * 需要保证迭代器长度等于head_size，构造器并不负责校验数据
     * @tparam Iterator
     */
    template <class Iterator,
        std::enable_if_t<std::is_same_v<std::decay_t<typename Iterator::value_type>, ByteT>, int> = 0>
    PacketHead(Iterator it)
    {
        it += bytesToInteger(it, mVersion);
        it += bytesToInteger(it, mOperation);
        it += bytesToInteger(it, mTag);
        it += bytesToInteger(it, mDataSize);
        it += bytesToInteger(it, mHeadCrc);
        it += bytesToInteger(it, mDataCrc);
    }

    /**
     * @brief 基于数据头信息构造数据头
     * 调用此构造函数构造器不会计算数据头校验码
     * @param version
     * @param operation
     * @param tag
     * @param size
     * @param dataCrc
     */
    PacketHead(const uint32_t version, const uint16_t operation, const uint16_t tag, const uint32_t size, const uint16_t dataCrc) noexcept
        : mVersion(version)
        , mOperation(operation)
        , mTag(tag)
        , mDataSize(size)
        , mHeadCrc(0)
        , mDataCrc(dataCrc)
    {
    }

    /**
     * @brief 将数据头转为字节流
     * 此函数将自动计算数据头校验代码，但并不会修改类中head_check的值
     * @return decltype(auto)
     */
    auto toBytes() const
    {
        auto buffer = bytes::makeByteBuffer(kHeadSize);
        auto it = buffer.begin();

        it += bytes::integerToBytes(this->mVersion, it);
        it += bytes::integerToBytes(this->mOperation, it);
        it += bytes::integerToBytes(this->mTag, it);
        it += bytes::integerToBytes(this->mDataSize, it);

        //基于前面构造的数据计算数据头校验码
        it += bytes::integerToBytes(evalCrcSick(buffer.begin(), it), it);

        it += bytes::integerToBytes(this->mDataCrc, it);
        return buffer;
    }

    /**
     * @brief 将数据头转为字符串描述
     *
     * @return std::string
     */
    std::string toString() const
    {
        return std::string().append("[").append("version: ").append(std::to_string(this->mVersion)).append("; ").append("operation: ").append(std::to_string(this->mOperation)).append("; ").append("tag: ").append(std::to_string(this->mTag)).append("; ").append("size: ").append(std::to_string(this->mDataSize)).append("; ").append("header_crc: ").append(std::to_string(this->mHeadCrc)).append("; ").append("data_crc: ").append(std::to_string(this->mDataCrc)).append(" ]");
    }
};

/**
 * @brief 完整的数据封包
 * [数据头][数据]
 */
class Packet {
    template <typename T>
    friend class Unpacker;

private:
    PacketHead mHead;
    BytesT mData;

    /**
     * @brief 基于已有的数据头和数据构造一个完整的包
     * 构造器并不负责校验数据是否正确，这需要调用者保证
     * @tparam Iterator
     * @param head
     * @param first
     * @param last
     */
    template <class Iterator,
        std::enable_if_t<std::is_same_v<std::decay_t<typename Iterator::value_type>, ByteT>, int> = 0>
    Packet(const PacketHead& head, Iterator first, Iterator last)
        : mHead(head)
    {
        std::copy(first, last, std::back_inserter(mData));
    }

public:
    /**
     * @brief 通过制定头信息以及数据迭代器构造数据
     *
     * @tparam Iterator
     * @param version 数据包版本
     * @param operation 操作码
     * @param tag 标识码
     * @param first 迭代器首元素
     * @param last 迭代器最后一个元素之后
     */
    template <class Iterator,
        std::enable_if_t<std::is_same_v<std::decay_t<typename Iterator::value_type>, ByteT>, int> = 0>
    Packet(const uint32_t version, const uint16_t operation, const uint16_t tag, Iterator first, Iterator last) noexcept
        : mHead(version, operation, tag, static_cast<uint32_t>(std::distance(first, last)),
            hash_u16(first, last))
    {
        std::copy(first, last, std::back_inserter(mData));
    }

    /**
     * @brief 基于任意容器构造数据包
     */
    template <class Container,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<typename Container::value_type>, ByteT>, int> = 0>
    Packet(const uint32_t version, const uint16_t operation, const uint16_t tag, const Container& data) noexcept
        : Packet(version, operation, tag, data.cbegin(), data.cend())
    {
    }

    auto version() const noexcept
    {
        return this->mHead.mVersion;
    }

    auto operation() const noexcept
    {
        return this->mHead.mOperation;
    }

    auto tag() const noexcept
    {
        return this->mHead.mTag;
    }

    auto size() const noexcept
    {
        return this->mHead.mDataSize;
    }

    const BytesT& data() const noexcept
    {
        return this->mData;
    }

    /**
     * @brief 将数据封包内容转换为字节流
     *
     * @return 字节流缓存
     */
    auto toBytes() const
    {
        auto buff = this->mHead.toBytes();
        std::copy(this->mData.cbegin(), this->mData.cend(), std::back_insert_iterator(buff));
        return buff;
    }

    /**
     * @brief 数据封包内容转字符串描述
     *
     * @return std::string
     */
    std::string toString() const
    {
        return mHead.toString().append(" Data: ").append(debug::arraysDescribe<uint32_t>(mData.begin(), mData.end()));
    }
};

/**
 * @brief 解包器
 *
 */
template <class Callback>
class Unpacker {
private:
    //缓冲区
    BytesT mBuffer;
    //临时构造的数据头
    std::variant<std::monostate, PacketHead> mHead;
    //处理回调函数
    Callback mCallback;

    /**
     * @brief 重置解包器状态
     *
     */
    void reset() noexcept
    {
        mHead = std::monostate();
        mBuffer.clear();
    }

    /**
     * @brief 校验当前构造的临时包头是否正确
     * 校验成功则清理缓存，校验失败则重置解包器
     */
    bool headerCheck() noexcept
    {
        auto headPtr = std::get_if<PacketHead>(&mHead);
        if (headPtr != nullptr && mBuffer.size() == PacketHead::kHeadSize) {
            auto code = evalCrcSick(mBuffer.cbegin(), mBuffer.cbegin() + 12);

            if (headPtr->mHeadCrc == code && headPtr->mDataSize + PacketHead::kHeadSize < MAX_DATAPACK_SIZE) {
                //校验通过，清理缓冲区数据，以便于接下来用于构造数据
                mBuffer.clear();
                return true;
            } else {
                //校验失败 重置状态
                this->reset();
            }
        }
        return false;
    }

    /**
     * @brief 校验数据包数据是否正确
     *
     * @return true 数据准确
     * @return false 数据有误
     */
    bool dataCheck() const noexcept
    {
        auto headPtr = std::get_if<PacketHead>(&mHead);
        if (headPtr == nullptr || mBuffer.size() != headPtr->mDataSize) {
            return false;
        }
        return headPtr->mDataCrc == evalCrcSick(mBuffer.cbegin(), mBuffer.cend());
    }

    /**
     * @brief 当head和buffer可以组成一个完整的数据包时调用此函数
     * 此函数将校验数据是否正确，若时正确则构造数据包并调用回调
     * 无论是否校验通过，此函数都将重置解包器状态
     */
    void completed()
    {
        auto headPtr = std::get_if<PacketHead>(&mHead);
        if (headPtr != nullptr && this->dataCheck()) {
            //数据包校验成功，构造数据包并调用回调
            auto p = Packet(*headPtr, mBuffer.cbegin(), mBuffer.cend());
            //回调
            mCallback(p);
        }
        //重置解包器状态，以便构造下一个数据包
        this->reset();
    }

public:
    /**
     * @brief 构造一个解包器
     *
     * @param cb 成功解包回调函数，当解包器成功解除一个完整的数据封包并通过校验时将会调用此函数
     * 回调函数接收一个packet引用参数
     */
    Unpacker(Callback callback)
        : mCallback(callback)
    {
    }

    /**
     * @brief 处理数据流
     * 处理传入的数据流，当数据构造出一个完整的数据包时将提供callback回调函数传出一个完整的数据包
     *
     * @tparam Iterator 数据流迭代器
     * @tparam Callback
     * @param first
     * @param last
     * @param callback 回调函数
     */
    template <class Iterator,
        std::enable_if_t<std::is_same_v<std::decay_t<typename Iterator::value_type>, ByteT>, int> = 0>
    void process(Iterator first, Iterator last)
    {
        if (!(first < last)) {
            return;
        }

        if (auto head_ptr = std::get_if<PacketHead>(&mHead)) {
            // 基于数据头中的数据长度构造数据
            while (first != last && mBuffer.size() < head_ptr->mDataSize) {
                mBuffer.push_back(*first++);
            }

            if (mBuffer.size() == head_ptr->mDataSize) {
                //一个完整的数据包构造完成
                this->completed();
            }
        } else {
            // 如果不存在临时的数据头，则先构造数据头
            while (first != last && mBuffer.size() < PacketHead::kHeadSize) {
                mBuffer.push_back(*first++);
            }

            //构造临时的数据头
            if (mBuffer.size() == PacketHead::kHeadSize) {
                mHead = PacketHead(mBuffer.cbegin());
                //校验数据头，如果校验成功则判断数据长度是否为0，若是则构造完成
                if (this->headerCheck() && std::get_if<PacketHead>(&mHead)->mDataSize == 0) {
                    //校验通过，且数据长度是0，无需继续构造数据，通知构造完成
                    this->completed();
                }
            }
        }

        this->process(first, last);
    }
};

inline std::string toString(const Packet& packet)
{
    return packet.toString();
}

}
