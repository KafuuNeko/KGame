#include "cppcrc.hh"
#include "cppcrc/code_crc8.hh"
#include "cppcrc/code_crcdnp.hh"
#include "cppcrc/code_crckrmit.hh"
#include "cppcrc/code_crcsick.hh"

#include <vector>
#include <fstream>
#include <iostream>

using namespace crc;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 0;
    }

    std::vector<std::pair<Encoder, std::string>> codes = {
        {Encoder(CodeCrc8::instance()),             "CRC8      " },
        {Encoder(CodeCrc16::instance()),            "CRC16     " },
        {Encoder(CodeCrc32::instance()),            "CRC32     " },
        {Encoder(CodeCrc64::ecmaInstance()),        "CRC64_ECMA" },
        {Encoder(CodeCrc64::weInstance()),          "CRC64_WE  " },
        {Encoder(CodeCrcDnp::instance()),           "DNP       " },
        {Encoder(CodeCrcKrmit::instance()),         "KERMIT    " },
        {Encoder(CodeCrcSick::instance()),          "SICK      " }
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
