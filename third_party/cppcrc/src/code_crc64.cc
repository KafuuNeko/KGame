#include "cppcrc/code_crc64.hh"

#include <cinttypes>

#ifdef CPPCRC_THREADSAFE
#include <mutex>
#endif

namespace crc {

static constexpr uint64_t kPoly = 0x42F0E1EBA9EA3693ull;
static constexpr uint64_t kStartECMA = 0x0000000000000000ull;
static constexpr uint64_t kStartWE = 0xFFFFFFFFFFFFFFFFull;

#ifdef CPPCRC_THREADSAFE
static std::mutex kLock;
#endif

static bool kTableInit = false;
static uint64_t kTable[256];

static void initTable() {
#ifdef CPPCRC_THREADSAFE
  std::lock_guard<std::mutex> lockGuard(kLock);
#endif

  if (kTableInit) {
    return;
  }

  for (uint64_t i = 0; i < 256; i++) {

    uint64_t crcValue = 0;
    uint64_t c = i << 56;

    for (uint64_t j = 0; j < 8; j++) {

      crcValue = ((crcValue ^ c) & 0x8000000000000000ull)
                     ? ((crcValue << 1) ^ kPoly)
                     : (crcValue << 1);

      c = c << 1;
    }

    kTable[i] = crcValue;
  }

  kTableInit = true;
}

CodeBase::ShareConstPtr CodeCrc64::ecmaInstance() {
  static auto ptr = std::make_shared<CodeCrc64>(Type::ecma);
  return ptr;
}

CodeBase::ShareConstPtr CodeCrc64::weInstance() {
  static auto ptr = std::make_shared<CodeCrc64>(Type::we);
  return ptr;
}

uint64_t CodeCrc64::startValue() const noexcept {
  if (mType == Type::ecma) {
    return kStartECMA;
  } else {
    return kStartWE;
  }
}

uint64_t CodeCrc64::result(uint64_t value) const noexcept {
  if (mType == Type::we) {
    return value ^ static_cast<uint64_t>(-1);
  }
  return value;
}

uint64_t CodeCrc64::updateCrc(uint64_t value, uint8_t c, uint8_t pre) const {
  if (!kTableInit) {
    initTable();
  }
  return (value << 8) ^ kTable[((value >> 56) ^ static_cast<uint64_t>(c)) &
                               0x00000000000000FFull];
}

} // namespace crc