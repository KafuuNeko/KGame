#ifndef _CRC_HPP_
#define _CRC_HPP_

/**
 * @file crc.hpp
 * @author kafuu (kafuuneko@gmail.com)
 * @brief Cyclic Redundancy Check based on libcrc(https://github.com/lammertb/libcrc)
 * @version 0.2
 * @date 2021-12-12
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <cstdint>
#include <string_view>
#include <type_traits>
#ifndef __cplusplus
#error Do not include the hpp header in a c project!
#endif //__cplusplus

extern "C" {
#include "crc.h"
}

#include <istream>

namespace crc {

enum class Code {
    CRC8,
    CRC16,
    CRC32,
    CRC64_WE,
    CRC64_ECMA,
    CCITT_1D0F,
    CCITT_FFFF,
    DNP,
    KERMIT,
    SICK
};

static void initTab()
{
    init_crc16_tab();
    init_crc32_tab();
    init_crc64_tab();
    init_crcccitt_tab();
    init_crcdnp_tab();
    init_crc_tab();
}

static uint64_t startValue(Code code) noexcept
{
    switch (code) {
    case Code::CRC8:
        return CRC_START_8;
    case Code::CRC16:
        return CRC_START_16;
    case Code::CRC32:
        return CRC_START_32;
    case Code::CRC64_WE:
        return CRC_START_64_WE;
    case Code::CRC64_ECMA:
        return CRC_START_64_ECMA;
    case Code::CCITT_1D0F:
        return CRC_START_CCITT_1D0F;
    case Code::CCITT_FFFF:
        return CRC_START_CCITT_FFFF;
    case Code::DNP:
        return CRC_START_DNP;
    case Code::KERMIT:
        return CRC_START_KERMIT;
    case Code::SICK:
        return CRC_START_SICK;
    default:
        return 0;
    }
}

class Crc {

public:
    explicit Crc(Code code) noexcept
        : mCode(code)
        , mValue(startValue(code))
        , mPreByte(0)
    {
    }

    explicit Crc(Code code, std::istream& input)
        : Crc(code)
    {
        this->update(input);
    }

    template <class Iterator, std::enable_if_t<std::is_same_v<std::make_unsigned_t<std::remove_cv_t<std::remove_reference_t<decltype(*(Iterator {}))>>>, uint8_t>, int> = 0>
    explicit Crc(Code type, Iterator first, Iterator last)
        : Crc(type)
    {
        this->update(first, last);
    }

    explicit Crc(Code type, std::string_view str)
        : Crc(type)
    {
        update(str);
    }

    void update(uint8_t c) noexcept
    {
        switch (mCode) {
        case Code::CRC8:
            mValue = update_crc_8(static_cast<uint8_t>(mValue), c);
            break;
        case Code::CRC16:
            mValue = update_crc_16(static_cast<uint16_t>(mValue), c);
            break;
        case Code::CRC32:
            mValue = update_crc_32(static_cast<uint32_t>(mValue), c);
            break;
        case Code::CRC64_WE:
            mValue = update_crc_64(static_cast<uint64_t>(mValue), c);
            break;
        case Code::CRC64_ECMA:
            mValue = update_crc_64(static_cast<uint64_t>(mValue), c);
            break;
        case Code::CCITT_1D0F:
            mValue = update_crc_ccitt(static_cast<uint16_t>(mValue), c);
            break;
        case Code::CCITT_FFFF:
            mValue = update_crc_ccitt(static_cast<uint16_t>(mValue), c);
            break;
        case Code::DNP:
            mValue = update_crc_dnp(static_cast<uint16_t>(mValue), c);
            break;
        case Code::KERMIT:
            mValue = update_crc_kermit(static_cast<uint16_t>(mValue), c);
            break;
        case Code::SICK:
            mValue = update_crc_sick(static_cast<uint16_t>(mValue), c, mPreByte);
            break;
        }

        mPreByte = c;
    }

    void update(std::istream& input)
    {
        uint8_t c = 0;

        if (!input) {
            return;
        }

        while (input.read(reinterpret_cast<char*>(&c), 1)) {
            update(c);
        }
    }

    template <class Iterator, std::enable_if_t<std::is_same_v<std::make_unsigned_t<std::remove_cv_t<std::remove_reference_t<decltype(*(Iterator {}))>>>, uint8_t>, int> = 0>
    void update(Iterator first, Iterator last)
    {
        uint8_t c = 0;
        for (; first != last; ++first) {
            update(*first);
        }
    }

    void update(std::string_view str)
    {
        this->update(str.cbegin(), str.cend());
    }

    void reset() noexcept
    {
        mValue = startValue(mCode);
        mPreByte = 0;
    }

    uint64_t value() noexcept
    {

        if (mCode == Code::CRC32) {
            return mValue ^ static_cast<uint32_t>(-1);
        }

        if (mCode == Code::CRC64_WE) {
            return mValue ^ static_cast<uint64_t>(-1);
        }

        if (mCode == Code::SICK || mCode == Code::KERMIT) {
            uint16_t lowByte = (mValue & 0xFF00) >> 8;
            uint16_t highByte = (mValue & 0x00FF) << 8;
            return lowByte | highByte;
        }

        if (mCode == Code::DNP) {
            uint16_t lowByte = (~mValue & 0xff00) >> 8;
            uint16_t highByte = (~mValue & 0x00ff) << 8;
            return lowByte | highByte;
        }

        return mValue;
    }

private:
    Code mCode;
    uint64_t mValue;
    uint8_t mPreByte;
};

}

#endif
