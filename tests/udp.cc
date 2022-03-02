#include "simple_socket/udp.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

void listen() {
    auto udp = simple_socket::UDP::instance();

    uint16_t port;
    std::cout << "port: ";
    std::cin >> port;

    if (!udp->bind(simple_socket::UDP::makeAddress("127.0.0.1", port))) {
        std::cout << "bind failure" << std::endl;
        return;
    }

    std::vector<uint8_t> buffer(1024, 0);
    simple_socket::UDP::UDPAddressPtr remoteAddress;

    int32_t len = 0;

    while ((len = udp->receive(buffer, remoteAddress)) > 0) {
        std::cout << simple_socket::getIpString(remoteAddress) << ":" << simple_socket::getPort(remoteAddress) << " " << std::string(buffer.begin(), buffer.begin() + len) << std::endl;
        udp->sent(buffer.begin(), buffer.end(), remoteAddress);
        //std::cout << (udp->close() ? "true" : "false") << std::endl;
    }
}

void sent() {
    std::string ip;
    uint16_t port;

    std::cout << "ip: ";
    std::cin >> ip;
    std::cout << "port: ";
    std::cin >> port;

    auto udp = simple_socket::UDP::instance();

    std::string data;
    std::cout << "data: ";
    std::cin >> data;

    std::vector<uint8_t> buffer(1024, 0);
    auto remoteAddress = simple_socket::UDP::makeAddress(ip, port);
    udp->sent(data.begin(), data.end(), remoteAddress);

    int32_t len = 0;
    while ((len = udp->receive(buffer, remoteAddress)) > 0) {
        std::cout << simple_socket::getIpString(remoteAddress) << ":" << simple_socket::getPort(remoteAddress) << " " << std::string(buffer.begin(), buffer.begin() + len) << std::endl;
        udp->sent(buffer.begin(), buffer.end(), remoteAddress);
    }

}

int main() {
    int32_t choice;
    std::cout << "1.Listen or 2.send? " << std::endl;
    std::cin >> choice;

    if (choice == 1) {
        listen();
    } else if (choice == 2) {
        sent();
    }

    return 0;
}
