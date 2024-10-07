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

#pragma once

#include <vector>
#include <map>
#include <memory>

#include "order.hpp"

using namespace std;

class Orderbook{
private:
    // m_ indicates private members
	map<double, vector<unique_ptr<Order>>, less<double>> m_bids; 
	map<double, vector<unique_ptr<Order>>, greater<double>> m_asks; 
	
public:
	void add_order(int qty, double price, BookSide side);
	
	Orderbook(bool generate_dummies);

	template<typename T>
	void clean_leg(map<double, vector<unique_ptr<Order>>, T>& map);

	void remove_empty_keys();

    std::pair<int,double> handle_order(OrderType type, int order_quantity, Side side, double price = 0);
    
    template <typename T>
    std::pair<int,double> fill_order(map<double, vector<unique_ptr<Order>>, T>& offers, 
                                 const OrderType type, const Side side, int& order_quantity,
                                 const double price, int& units_transacted, double& total_value);

	double best_quote(BookSide side);

    const auto& get_bids(){ return m_bids; }
    const auto& get_asks(){ return m_asks; }

	template<typename T>
	void print_leg(map<double, vector<unique_ptr<Order>>, T>& hashmap, BookSide side);

	void print();
};
