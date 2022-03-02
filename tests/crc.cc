#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <list>

#include "crc/crc.hpp"

void test_a()
{
    std::string str;

    std::cout << "Input string: ";
    std::getline(std::cin, str);

    std::vector<std::pair<crc::Code, std::string>> codes = {
        {crc::Code::CRC8,       "CRC8"          },
        {crc::Code::CRC16,      "CRC16"         },
        {crc::Code::CRC32,      "CRC32"         },
        {crc::Code::CRC64_ECMA, "CRC64_ECMA"    },
        {crc::Code::CRC64_WE,   "CRC64_WE"      },
        {crc::Code::CCITT_1D0F, "CCITT_1D0F"    },
        {crc::Code::CCITT_FFFF, "CCITT_FFFF"    },
        {crc::Code::DNP,        "DNP"           },
        {crc::Code::KERMIT,     "KERMIT"        },
        {crc::Code::SICK,       "SICK"          }
    };

    crc::initTab();

    for (auto& code : codes) {
        crc::Crc crc{code.first, str};
        std::cout << code.second << " value is " << crc.value() << std::endl;
    }
}

void test_b() {
    std::string root;
    crc::Crc crc64_ecma(crc::Code::CRC64_ECMA);

    std::getline(std::cin, root);

    std::cout << "computing CRC64_ECMA..." << std::endl;
    std::unordered_map<uint64_t, std::list<std::filesystem::path>> map;
    for (auto& p : std::filesystem::recursive_directory_iterator(root)) {
        if (p.is_directory()) {
            continue;
        }

        std::ifstream is(p.path(), std::ios::binary);

        crc64_ecma.update(is);
        auto crcValue = crc64_ecma.value();
        crc64_ecma.reset();

        is.close();

        std::cout << "crc value: " << crcValue << std::endl;

        map[crcValue].push_back(p);
    }

    for (auto& element : map) {
        if (element.second.size() < 2) {
            continue;
        }

        std::cout << "crc: " << element.first << std::endl;

        for (auto& p : element.second) {
            std::cout << "\t" << p << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{

    test_a();
    //test_b();

    return 0;
}
