#include <vector>
#include <random>
#include <cstddef>
#include <iostream>
#include <cstdint>

#include "timer.h"
constexpr size_t SIZE = 4000;

int main() { 
    
    std::vector<uint64_t> array(SIZE * SIZE, 0);
    std::mt19937_64 rng(0);
    Timer timer;

    // Row Major 
    rng.seed(0);
    timer.restart();
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            array[i * SIZE + j] = rng();
        }
    }
    uint64_t timeRow = timer.click<Timer::Micros>();

    uint64_t sum = 0;

    
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            sum += array[i * SIZE + j];
        }
    }
    std::cout << timeRow << " " << sum << "\n";

    // Column Major
    rng.seed(0);
    uint64_t sum2 = 0;
    timer.restart();
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            array[j * SIZE + i] = rng();
        }
    }
    uint64_t timeCol = timer.click<Timer::Micros>();

    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            sum2 += array[j * SIZE + i];
        }
    }

    std::cout << timeCol << " " << sum2 << "\n";

    return 0; 


}
