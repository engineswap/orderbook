/**
 * @file order.cpp
 * @brief This file contains the implementation of the Order class.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <stdlib.h> 
#include <map>
#include <thread>
#include <iomanip>
#include <memory>

#include "../include/order.hpp"
#include "../include/helpers.hpp"

Order::Order(int quantity_, double price_, BookSide side_){
	timestamp = unix_time();
	quantity = quantity_;
	price = price_;
	side = side_;
}

void Order::set_quantity(int new_qty){
	quantity = new_qty;
}

int Order::get_quantity() { return quantity; }
double Order::get_price() { return price; }
time_t Order::get_timestamp() { return timestamp; }