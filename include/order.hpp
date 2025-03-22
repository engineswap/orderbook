/**
 * @file order.hpp
 * @brief This file contains the declaration of the Order class.
 * 
 * The Order class represents an order in an order book. It stores information such as the quantity, price, side, and timestamp of the order.
 * The class provides setters and getters to modify and access the order's properties.
 */

#pragma once

#include <cstdint>
#include "enums.hpp"
#include "helpers.hpp"

inline uint64_t generate_unique_id() {
    static uint64_t s_next_id{0};
    return ++s_next_id;
}

struct Order {
    uint64_t id;
    int quantity;
    BookSide side;
    double price;
    uint64_t timestamp;

    Order(int q, double p, BookSide s, uint64_t t = unix_time())
        : id(generate_unique_id()), quantity(q), price(p), side(s), timestamp(t) {}
};
