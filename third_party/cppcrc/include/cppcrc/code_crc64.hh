#pragma once

#include "code_base.hh"

namespace crc {

class CodeCrc64 : public CodeBase {
public:
  enum class Type { ecma, we };

  CodeCrc64(Type type) noexcept : mType(type) {}

  static CodeBase::ShareConstPtr ecmaInstance();
  static CodeBase::ShareConstPtr weInstance();

private:
  const Type mType;

  virtual uint64_t startValue() const noexcept override;

  virtual uint64_t result(uint64_t value) const noexcept override;

  virtual uint64_t updateCrc(uint64_t value, uint8_t c,
                             uint8_t pre) const override;
};

} // namespace crc