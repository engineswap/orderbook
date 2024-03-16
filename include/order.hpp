/**
 * @file order.hpp
 * @brief This file contains the declaration of the Order class.
 * 
 * The Order class represents an order in an order book. It stores information such as the quantity, price, side, and timestamp of the order.
 * The class provides setters and getters to modify and access the order's properties.
 */
#include <iostream>
#include <vector>
#include <chrono>
#include <stdlib.h> 
#include <map>
#include <thread>
#include <iomanip>
#include <memory>

#include "enums.hpp"
using namespace std;

#pragma once

class Order{
	// Properties
	int quantity;
	double price;
	BookSide side;
	uint64_t timestamp;

public:
	Order(int quantity_, double price_, BookSide side_);

	// Setters
	void set_quantity(int new_qty);

	// Getters
	int get_quantity();
	double get_price();
	time_t get_timestamp();
};

