/**
 * @file helpers.cpp
 * @brief This file contains the little helper functions used elsewhere.
 */

#pragma once

#include <chrono>

inline uint64_t unix_time() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

void print_file_contents(std::string_view file_path);

void print_fill(std::pair<int, double> fill, int quantity, u_int64_t start_time, u_int64_t end_time);

#include <iostream>
#include <functional>

// Defer helper to run a function at scope exit
struct Defer {
    std::function<void()> f;
    ~Defer() { f(); }
    Defer(std::function<void()> f_) : f(f_) {}
};

// Macro to profile scope in nanoseconds
#define PROFILE_SCOPE(name_str) \
    auto start_time = std::chrono::high_resolution_clock::now(); \
    auto end_scope = [&]() { \
        auto end_time = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count(); \
        std::cout << name_str << " took " << duration << "ns\n"; \
    }; \
    Defer end_defer(end_scope);

