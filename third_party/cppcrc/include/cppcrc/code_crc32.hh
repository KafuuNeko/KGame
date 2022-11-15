#pragma once

#include "code_base.hh"

namespace crc {

class CodeCrc32 : public CodeBase {
public:
  static CodeBase::ShareConstPtr instance();

private:
  virtual uint64_t startValue() const noexcept override { return 0xFFFFFFFFul; }

  virtual uint64_t updateCrc(uint64_t value, uint8_t c,
                             uint8_t pre) const override;

  virtual uint64_t result(uint64_t value) const noexcept override {
    return value ^ static_cast<uint32_t>(-1);
  }
};

} // namespace crc