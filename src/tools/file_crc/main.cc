#include "crc/crc.hpp"
#include <vector>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 0;
    }

    crc::initTab();

    std::vector<std::pair<crc::Crc, std::string>> codes = {
        {crc::Crc(crc::Code::CRC8),       "CRC8      " },
        {crc::Crc(crc::Code::CRC16),      "CRC16     " },
        {crc::Crc(crc::Code::CRC32),      "CRC32     " },
        {crc::Crc(crc::Code::CRC64_ECMA), "CRC64_ECMA" },
        {crc::Crc(crc::Code::CRC64_WE),   "CRC64_WE  " },
        {crc::Crc(crc::Code::CCITT_1D0F), "CCITT_1D0F" },
        {crc::Crc(crc::Code::CCITT_FFFF), "CCITT_FFFF" },
        {crc::Crc(crc::Code::DNP),        "DNP       " },
        {crc::Crc(crc::Code::KERMIT),     "KERMIT    " },
        {crc::Crc(crc::Code::SICK),       "SICK      " }
    };

    std::ifstream is(argv[1], std::ios::binary);

    uint8_t c = 0;

    if (!is) {
        std::cout << "Unable to open file" << std::endl;
        return 0;
    }

    while (is.read(reinterpret_cast<char*>(&c), 1)) {
        for (auto& code : codes) {
            code.first.update(c);
        }
    }

    is.close();

    for (auto& code : codes) {
        std::cout << code.second << "\t" << code.first.value() << std::endl;
    }

    return 0;

}
