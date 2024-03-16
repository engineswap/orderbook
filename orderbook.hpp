/**
 * @file orderbook.hpp
 * @brief This file contains the declaration of the Orderbook class.
 * 
 * The Orderbook class represents an order book, which is a collection of buy and sell orders.
 * It provides functionality to add orders, execute orders, and retrieve the best quote.
 * The order book is implemented using two maps, one for buy orders (bids) and one for sell orders (asks).
 * Each map is sorted based on the price of the orders.
 * The Orderbook class also provides methods to clean up empty keys and print the order book.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <stdlib.h> 
#include <map>
#include <thread>
#include <iomanip>
#include <memory>

#include "order.hpp"

using namespace std;

#pragma once

class Orderbook{
	map<double, vector<unique_ptr<Order>>, less<double>> bids; 
	map<double, vector<unique_ptr<Order>>, greater<double>> asks; 
	
public:
	void add_order(int qty, double price, BookSide side);
	
	Orderbook();

	template<typename T>
	void clean_leg(map<double, vector<unique_ptr<Order>>, T>& map);

	void remove_empty_keys();

	tuple<int,double> execute_order(OrderType type, int order_quantity, Side side, double price = 0);

	double best_quote(BookSide side);

	template<typename T>
	void print_leg(map<double, vector<unique_ptr<Order>>, T>& hashmap, BookSide side);

	void print();
};