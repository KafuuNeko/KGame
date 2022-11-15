#include "cppcrc/code_crc32.hh"

#include <cinttypes>

#ifdef CPPCRC_THREADSAFE
#include <mutex>
#endif

namespace crc {

constexpr uint32_t kPoly = 0xEDB88320ul;

#ifdef CPPCRC_THREADSAFE
static std::mutex kLock;
#endif

static bool kTableInit = false;
static uint32_t kTable[256];

static void initTable() {
#ifdef CPPCRC_THREADSAFE
  std::lock_guard<std::mutex> lockGuard(kLock);
#endif

  if (kTableInit) {
    return;
  }

  for (uint32_t i = 0; i < 256; i++) {

    kTable[i] = i;

    for (uint32_t j = 0; j < 8; j++) {
      kTable[i] =
          (kTable[i] & 0x01L) ? (kTable[i] >> 1) ^ kPoly : kTable[i] >> 1;
    }
  }

  kTableInit = true;
}

CodeBase::ShareConstPtr CodeCrc32::instance() {
  static auto ptr = std::make_shared<CodeCrc32>();
  return ptr;
}

uint64_t CodeCrc32::updateCrc(uint64_t value, uint8_t c, uint8_t pre) const {
  if (!kTableInit) {
    initTable();
  }
  auto crc32Value = static_cast<uint32_t>(value);
  return (crc32Value >> 8) ^
         kTable[(crc32Value ^ static_cast<uint32_t>(c)) & 0x000000FFul];
}

} // namespace crc