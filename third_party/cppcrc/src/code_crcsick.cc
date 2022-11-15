#include "cppcrc/code_crcsick.hh"
#include <cstdint>

namespace crc {
constexpr uint16_t kPoly = 0x8005;

CodeBase::ShareConstPtr CodeCrcSick::instance() {
  static auto ptr = std::make_shared<CodeCrcSick>();
  return ptr;
}

uint64_t CodeCrcSick::updateCrc(uint64_t value, uint8_t c, uint8_t pre) const {
  auto crcValue = static_cast<uint16_t>(value);

  uint16_t high = 0x00FF & static_cast<uint16_t>(c);
  uint16_t low = (0x00FF & static_cast<uint16_t>(pre)) << 8;

  crcValue = (crcValue & 0x8000) ? ((crcValue << 1) ^ kPoly) : (crcValue << 1);
  crcValue ^= (high | low);

  return crcValue;
}

} // namespace crc
