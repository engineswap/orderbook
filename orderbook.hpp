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

// Guards to avoid redifining twice
#ifndef ORDERBOOK_H
#define ORDERBOOK_H

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

#endif