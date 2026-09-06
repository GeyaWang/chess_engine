#pragma once
#include <bit>
#include <cstdint>


namespace mf::utils {
    inline uint8_t lsb_index(const uint64_t x) {
        return x ? std::countr_zero(x) : 64;
    }

    inline uint8_t msb_index(const uint64_t x) {
        return x ? 63 - std::countl_zero(x) : 64;
    }
}
