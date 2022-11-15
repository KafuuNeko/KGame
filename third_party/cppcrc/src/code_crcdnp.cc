#include "cppcrc/code_crcdnp.hh"

#ifdef CPPCRC_THREADSAFE
#include <mutex>
#endif

namespace crc {

constexpr uint16_t kPoly = 0xA6BC;

#ifdef CPPCRC_THREADSAFE
static std::mutex kLock;
#endif

static bool kTableInit = false;
static uint16_t kTable[256];

CodeBase::ShareConstPtr CodeCrcDnp::instance() {
  static auto ptr = std::make_shared<CodeCrcDnp>();
  return ptr;
}

static void initCrcdnpTab() {

#ifdef CPPCRC_THREADSAFE
  std::lock_guard<std::mutex> lockGuard(kLock);
#endif

  if (kTableInit) {
    return;
  }

  for (auto i = 0; i < 256; i++) {
    auto crc = static_cast<uint16_t>(0);
    auto c = static_cast<uint16_t>(i);

    for (auto j = 0; j < 8; j++) {
      crc = ((crc ^ c) & 0x0001) ? ((crc >> 1) ^ kPoly) : (crc >> 1);
      c = c >> 1;
    }

    kTable[i] = crc;
  }

  kTableInit = true;
}

uint64_t CodeCrcDnp::updateCrc(uint64_t value, uint8_t c, uint8_t pre) const {
  if (!kTableInit) {
    initCrcdnpTab();
  }

  auto crcValue = static_cast<uint16_t>(value);

  return (crcValue >> 8) ^
         kTable[(crcValue ^ static_cast<uint16_t>(c)) & 0x00FF];
}

} // namespace crc