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

// Guards to avoid redifining twice
#ifndef ORDERS_H
#define ORDERS_H

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

#endif