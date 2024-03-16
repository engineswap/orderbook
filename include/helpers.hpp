/**
 * @file helpers.cpp
 * @brief This file contains the little helper functions used elsewhere.
 */

#pragma once

inline uint64_t unix_time() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}