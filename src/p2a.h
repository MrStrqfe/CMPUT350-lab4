#pragma once
#include <cstdint>
#include <cassert>

inline constexpr int d = 3;
inline constexpr int k = 64;

inline uint64_t expand(uint64_t input, uint32_t scale) {
    assert(scale >= 1);

    uint64_t result = 0;
    uint64_t targetPos = 0;

    while (input != 0 && targetPos < 64) {
        // Check if the least significant bit is 1
        // 1ULL to prevent integer overflow uding bitwise operations
        // If I used 1, it would be a 32-bit int, which would cause undefined behaviour
        if ((input & 1ULL) == 1ULL) {
            result |= (1ULL << targetPos);
        }

        // Shift input to inspect the next bit
        input >>= 1;

        // Move to the next destination position
        targetPos += scale;
    }
    return result;
}
/*
    Computes the 3D Morton code (Z-order curve index) for coordinates (x, y, z).
*
*  Interleaves the bits of three coordinates such that:
*  - Bits of x map to indices 0, 3, 6, ...
*  - Bits of y map to indices 1, 4, 7, ...
*  - Bits of z map to indices 2, 5, 8, ...
*/
inline uint64_t morton3d(uint64_t x, uint64_t y, uint64_t z) {
    return expand(x, 3) | (expand(y, 3) << 1) | (expand(z, 3) << 2);
}
