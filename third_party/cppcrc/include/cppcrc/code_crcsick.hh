#pragma once
#include "code_base.hh"

namespace crc {

class CodeCrcSick : public CodeBase {
public:
  static CodeBase::ShareConstPtr instance();

private:
  virtual uint64_t updateCrc(uint64_t value, uint8_t c,
                             uint8_t pre) const override;

  virtual uint64_t result(uint64_t value) const noexcept override {
    uint16_t lowByte = (static_cast<uint16_t>(value) & 0xFF00) >> 8;
    uint16_t highByte = (static_cast<uint16_t>(value) & 0x00FF) << 8;
    return lowByte | highByte;
  }
};

} // namespace crc