#pragma once

#include <cinttypes>
#include <memory>

namespace crc {

class CodeBase {
  friend class Encoder;

public:
  using ShareConstPtr = std::shared_ptr<const CodeBase>;
  virtual ~CodeBase() noexcept = default;

private:
  virtual uint64_t updateCrc(uint64_t value, uint8_t c, uint8_t pre) const = 0;

  virtual uint64_t startValue() const noexcept { return 0; }
  virtual uint64_t result(uint64_t value) const noexcept { return value; }
};

}; // namespace crc
