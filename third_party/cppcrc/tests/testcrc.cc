#include <iostream>
#include <string_view>
#include <vector>

#include "crc/code_crc16.hh"
#include "crc/code_crc32.hh"
#include "crc/code_crc64.hh"
#include "crc/code_crc8.hh"
#include "crc/code_crcdnp.hh"
#include "crc/code_crckrmit.hh"
#include "crc/code_crcsick.hh"
#include "crc/encoder.hh"

int main() {
  std::vector<std::pair<const char *, crc::CodeBase::ShareConstPtr>> codes = {
      {"crc8", crc::CodeCrc8::instance()},
      {"crc16", crc::CodeCrc16::instance()},
      {"crc32", crc::CodeCrc32::instance()},
      {"crc64_ecma", crc::CodeCrc64::ecmaInstance()},
      {"crc64_we", crc::CodeCrc64::weInstance()},
      {"crckrmit", crc::CodeCrcKrmit::instance()},
      {"crcsick", crc::CodeCrcSick::instance()},
      {"crcdnp", crc::CodeCrcDnp::instance()}};

  auto str = "123456789";
  for (auto &element : codes) {

    printf("%s('%s') = %lu\n", element.first, str,
           crc::Encoder{element.second, str}.value());
  }

  return 0;
}