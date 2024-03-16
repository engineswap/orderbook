#include <iostream>
#include <vector>
#include <chrono>
#include <stdlib.h> 
#include <map>
#include <thread>
#include <iomanip>
#include <memory>

#include "order.hpp"

uint64_t unix_time() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

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