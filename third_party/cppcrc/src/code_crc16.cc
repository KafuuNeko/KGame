#include "cppcrc/code_crc16.hh"

#include <cinttypes>

#ifdef CPPCRC_THREADSAFE
#include <mutex>
#endif

namespace crc {

constexpr uint16_t kPoly = 0xA001;

#ifdef CPPCRC_THREADSAFE
static std::mutex kLock;
#endif

static bool kTableInit = false;
static uint16_t kTable[256];

static void initTable() {
#ifdef CPPCRC_THREADSAFE
  std::lock_guard<std::mutex> lockGuard(kLock);
#endif

  if (kTableInit) {
    return;
  }

  for (uint16_t i = 0; i < 256; i++) {

    uint16_t crc = 0;
    uint16_t c = i;

    for (uint16_t j = 0; j < 8; j++) {

      if ((crc ^ c) & 0x0001) {
        crc = (crc >> 1) ^ kPoly;
      } else {
        crc = crc >> 1;
      }

      c = c >> 1;
    }

    kTable[i] = crc;
  }

  kTableInit = true;
}

CodeBase::ShareConstPtr CodeCrc16::instance() {
  static auto ptr = std::make_shared<CodeCrc16>();
  return ptr;
}

uint64_t CodeCrc16::updateCrc(uint64_t value, uint8_t c, uint8_t pre) const {
  if (!kTableInit) {
    initTable();
  }
  auto crc16Value = static_cast<uint16_t>(value);
  return (crc16Value >> 8) ^
         kTable[(crc16Value ^ static_cast<uint16_t>(c)) & 0x00FF];
}

} // namespace crc