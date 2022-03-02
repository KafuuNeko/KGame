#pragma once

/**
 * @file udp.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief 简易的跨平台UDP
 * @version 0.1
 * @date 2022-02-25
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <cassert>
#include <cinttypes>
#include <cstring>

#if defined(WIN32)

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#elif defined(__linux__)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#endif

namespace simple_socket {

class UDP {
private:
    struct UDPContext;
    struct UDPAddress;

    std::unique_ptr<struct UDPContext> mContext;
    std::unique_ptr<struct UDPAddress> mAddress;

    UDP();

public:
    using UDPAddressPtr = std::shared_ptr<struct UDPAddress>;

    ~UDP()
    {
        assert(this->close());
    }

    /**
     * @brief 实例化一个UDP
     * @return 若返回的指针为空则失败, 可通过UDP::lastError()返回值确定失败原因
     */
    static std::unique_ptr<UDP> instance();

    /**
     * @brief 构造一个UDP地址
     * @param address 例如：127.0.0.1
     * @param port 端口号
     */
    static UDPAddressPtr makeAddress(std::string_view ip, uint16_t port);

    /**
     * @brief 关闭socket
     */
    bool close();

    /**
     * @brief 向指定的地址发送数据
     * @param data 将发送的数据
     * @param to 接收者地址
     */
    bool sent(const std::vector<uint8_t>& data, const UDPAddressPtr& to) const;

    /**
     * @brief 向指定的地址发送数据
     * 此函数将以大端字节序发送
     * @param first 指向首个待发数据的迭代器
     * @param last 指向最后一个待发数据后一位的迭代器
     * @param to 接收者地址
     * @example
     * '''
     * std::vector<int32_t> buffer {1, 2, 3};
     * udp->sent(buffer.cbegin(), buffer.cend(), to);
     * 以上将向目标发送12位字节 {0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3}
     * '''
     */
    template <class Iterator, std::enable_if_t<std::is_integral_v<std::remove_cv_t<std::remove_reference_t<decltype(*(Iterator {}))>>>, int> = 0>
    bool sent(Iterator first, Iterator last, const UDPAddressPtr& to) const
    {
        constexpr size_t elementSize = sizeof(decltype(*first));

        std::vector<uint8_t> buffer(std::distance(first, last) * elementSize, 0);

        for (size_t i = 0; first != last; ++first) {
            auto element = *first;
            for (int32_t j = elementSize * 8 - 8; j >= 0; j -= 8) {
                buffer[i++] = static_cast<uint8_t>(element >> j) & 0xff;
            }
        }

        return this->sent(buffer, to);
    }

    /**
     * @brief 向指定的地址发送字符串数据
     * @param data 待发送字符串
     * @param to 接收者地址
     */
    bool sent(std::string_view data, const UDPAddressPtr& to)
    {
        return this->sent(data.cbegin(), data.cend(), to);
    }

    /**
     * @brief 绑定指定地址
     * @param address 将要绑定的地址
     * @return 若绑定成功则返回true，此时可调用receive开始接收数据。
     * @example udp->bind(UDP::makeAddress("127.0.0.1", 8080)
     */
    bool bind(const UDPAddressPtr& address) const;

    /**
     * @brief 在绑定成功后可使用此函数接收数据
     * @param buffer 接收数据缓冲区，请注意不要传入空的缓冲区
     * @param remoteAddress 本次接收的数据的来源地址
     * @return 成功则返回接收到的字节数，失败则返回-1，可调用UDP::lastError取得错误代码
     */
    int32_t receive(std::vector<uint8_t>& buffer, UDPAddressPtr& remoteAddress) const;

    /**
     *  @brief 取最近一个次错误代码
     */
    static int32_t lastError() noexcept;
};

struct UDP::UDPContext {
#if defined(WIN32)

    SOCKET socket;

#elif defined(__linux__)

    int socket;

#endif
};

struct UDP::UDPAddress {
    sockaddr_in address;
};

/**
 * @brief 取得地址ip字符串
 */
inline std::string getIpString(UDP::UDPAddressPtr address)
{
    return std::string(::inet_ntoa(address->address.sin_addr));
}

/**
 * @brief 取得地址端口
 */
inline uint16_t getPort(UDP::UDPAddressPtr address)
{
    return address->address.sin_port;
}

inline std::unique_ptr<UDP> UDP::instance()
{
#if defined(WIN32)

    WSADATA wsadata;
    if (::WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        return nullptr;
    }

#endif

    return std::unique_ptr<UDP>(new UDP());
}

inline UDP::UDPAddressPtr UDP::makeAddress(std::string_view ip, uint16_t port)
{
    auto ptr = std::make_unique<UDPAddress>();
    memset(&(ptr->address), 0, sizeof(ptr->address));

    ptr->address.sin_family = AF_INET;
    ptr->address.sin_port = ::htons(port);
    ptr->address.sin_addr.s_addr = ::inet_addr(std::string(ip.cbegin(), ip.cend()).c_str());

    return ptr;
}

inline UDP::UDP()
    : mContext(std::make_unique<UDPContext>())
{
    mContext->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

inline bool UDP::close()
{
    if (!mContext) {
        return true;
    }

    int32_t rc = 0;

#if defined(WIN32)
    rc = ::closesocket(mContext->socket);
    if (rc == 0) {
        rc = ::WSACleanup();
    }
#elif defined(__linux__)
    rc = ::close(mContext->socket);
#endif

    if (rc == 0) {
        mContext = nullptr;
    }

    return rc == 0;
}

inline bool UDP::sent(const std::vector<uint8_t>& data, const UDPAddressPtr& to) const
{
    if (!to || !mContext) {
        return false;
    }

    auto rc = ::sendto(mContext->socket, reinterpret_cast<const char*>(data.data()), data.size(), 0, reinterpret_cast<sockaddr*>(&(to->address)), sizeof(to->address));
    return rc == 0;
}

inline bool UDP::bind(const UDPAddressPtr& address) const
{
    assert(address);
    if (!mContext) {
        return false;
    }

    return ::bind(mContext->socket, (sockaddr*)&(address->address), sizeof(address->address)) == 0;
}

inline int32_t UDP::receive(std::vector<uint8_t>& buffer, UDPAddressPtr& remoteAddress) const
{
    assert(buffer.size());

    if (!mContext) {
        return 0;
    }

    remoteAddress = std::make_shared<UDPAddress>();

#if defined(WIN32)

    int len = sizeof(remoteAddress->address);

#elif defined(__linux__)

    socklen_t len = sizeof(remoteAddress->address);

#endif

    return ::recvfrom(mContext->socket, reinterpret_cast<char*>(buffer.data()), buffer.size(), 0, reinterpret_cast<sockaddr*>(&(remoteAddress->address)), &len);
}

inline int32_t UDP::lastError() noexcept
{
    int32_t ec = 0;

#if defined(WIN32)

    ec = WSAGetLastError();

#elif defined(__linux__)

    ec = errno;

#endif

    return ec;
}

}
