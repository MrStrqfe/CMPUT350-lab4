#include <random>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <iostream>

#include "p2a.h"
#include "timer.h"

constexpr size_t SIZE_Z = 256;
constexpr size_t SIZE_Y = 256;
constexpr size_t SIZE_X = 256;
constexpr size_t SIZE_K = 4;
constexpr size_t SIZE_CONV = SIZE_Z / SIZE_K;

// Initialize the 3D Matrixs in 1D
std::vector<uint64_t> A(SIZE_Z * SIZE_Y * SIZE_X, 0);
std::vector<uint64_t> B(SIZE_Z * SIZE_Y * SIZE_X, 0);
std::vector<uint64_t> Ka(SIZE_K * SIZE_K * SIZE_K, 0);
std::vector<uint64_t> Kb(SIZE_K * SIZE_K * SIZE_K, 0);
std::vector<uint64_t> outA(SIZE_CONV * SIZE_CONV * SIZE_CONV, 0);
std::vector<uint64_t> outB(SIZE_CONV * SIZE_CONV * SIZE_CONV, 0);


// Helper Functions
size_t rowMajorIndexA(size_t x, size_t y, size_t z) {
    assert(x < SIZE_X);
    assert(y < SIZE_Y);
    assert(z < SIZE_Z);
    return z * SIZE_Y * SIZE_X + y * SIZE_X + x;
}

size_t rowMajorIndexK(size_t x, size_t y, size_t z) {
    assert(x < SIZE_K);
    assert(y < SIZE_K);
    assert(z < SIZE_K);
    return z * (SIZE_K * SIZE_K) + y * SIZE_K + x;
}

size_t rowMajorIndexConv(size_t x, size_t y, size_t z) {
    assert(x < SIZE_CONV);
    assert(y < SIZE_CONV);
    assert(z < SIZE_CONV);
    return z * (SIZE_CONV * SIZE_CONV) + y * SIZE_CONV + x;
}

int main() { 
    std::mt19937_64 rng(0);
    Timer timer;

    // Initialize A
    rng.seed(0);

    for (size_t z = 0; z < SIZE_Z; ++z) {
        for (size_t y = 0; y < SIZE_Y; ++y) {
            for (size_t x = 0; x < SIZE_X; ++x) {
               size_t idx = rowMajorIndexA(x, y, z);
               A[idx] = rng();
            }
        }
    }
    

    // Copy A into B using Morton indexing
    for (size_t z = 0; z < SIZE_Z; ++z) {
        for (size_t y = 0; y < SIZE_Y; ++y) {
            for (size_t x = 0; x < SIZE_X; ++x) {
                B[morton3d(x, y, z)] = A[rowMajorIndexA(x, y, z)];
            }
        }
    }

    // Initialize Ka and Kb
    for (size_t z = 0; z < SIZE_K; ++z) {
        for (size_t y = 0; y < SIZE_K; ++y) {
            for (size_t x = 0; x < SIZE_K; ++x) {
                uint64_t val = x + y + z;
                Ka[rowMajorIndexK(x, y, z)] = val;
                Kb[morton3d(x, y, z)] = val;
            }
        }
    }

    // Row-major convolution (A * Ka)
    timer.restart();
    for (size_t oz = 0; oz < SIZE_CONV; ++oz) {
        for (size_t oy = 0; oy < SIZE_CONV; ++oy) {
            for (size_t ox = 0; ox < SIZE_CONV; ++ox) {
                uint64_t sum = 0;
                for (size_t kz = 0; kz < SIZE_K; ++kz) {
                    for (size_t ky = 0; ky < SIZE_K; ++ky) {
                        for (size_t kx = 0; kx < SIZE_K; ++kx) {
                            sum += A[rowMajorIndexA(ox * SIZE_K + kx, oy * SIZE_K + ky, oz * SIZE_K + kz)] * Ka[rowMajorIndexK(kx, ky, kz)];
                        }
                    }
                }
                outA[rowMajorIndexConv(ox, oy, oz)] = sum;
            }
        }
    }
    uint64_t timeRow = timer.click<Timer::Micros>();

    // Morton-order convolution (B * Kb)
    timer.restart();
    constexpr size_t TOTAL_BLOCKS = SIZE_CONV * SIZE_CONV * SIZE_CONV;
    constexpr size_t BLOCK_SIZE = SIZE_K * SIZE_K * SIZE_K;

    for (size_t block = 0; block < TOTAL_BLOCKS; ++block) {
        uint64_t sum = 0;
        size_t blockOffset = block * BLOCK_SIZE;
        for (size_t k = 0; k < BLOCK_SIZE; ++k) {
            sum += B[blockOffset + k] * Kb[k];
        }
        outB[block] = sum;
    }
    uint64_t timeMorton = timer.click<Timer::Micros>();

    // Print times in microseconds, one per line
    std::cout << timeRow << "\n";
    std::cout << timeMorton << "\n";

    // Assert that every output entry in A * Ka equals the equivalent entry in B * Kb
    for (size_t z = 0; z < SIZE_CONV; ++z) {
        for (size_t y = 0; y < SIZE_CONV; ++y) {
            for (size_t x = 0; x < SIZE_CONV; ++x) {
                assert(outA[rowMajorIndexConv(x, y, z)] == outB[morton3d(x, y, z)]);
            }
        }
    }

    return 0;
}
