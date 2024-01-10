#pragma once

#include <stdint.h>

#ifdef ARDUINO

#include <Arduino.h>

constexpr float SENSITIVITY  = 5.0f;

inline uint64_t getTime() {
    return micros();
}

#else

#include <chrono>

constexpr float SENSITIVITY  = 4.0f;

uint64_t getTime() {
    auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()/1000;
}

#endif