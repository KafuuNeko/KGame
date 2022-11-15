#pragma once
#include "code_base.hh"

namespace crc {

// https://srecord.sourceforge.net/crc16-ccitt.html
class CodeCrcCcitt : public CodeBase {
public:
  static CodeBase::ShareConstPtr instance();

private:
  virtual uint64_t updateCrc(uint64_t value, uint8_t c,
                             uint8_t pre) const override;
};

} // namespace crc