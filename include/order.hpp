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

struct Order{
    int quantity; // 4 
    BookSide side; // 4
    double price; // 8
    uint64_t timestamp; //8

    Order(int q, double p, BookSide s, uint64_t t = unix_time()){
        quantity = q;    
        price = p;
        side = s;
        timestamp = t;
    }
};
